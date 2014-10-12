/*
  crash_reporter.cc

  Copyright 2014 Jon TURNEY

  This file is part of cygwin-crashreporter

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "crash_reporter.h"

#include <breakpad/client/windows/handler/exception_handler.h>
#include <breakpad/client/windows/sender/crash_report_sender.h>

#include <sstream>
#include <time.h>

#define PSAPI_VERSION 1
#include "psapi.h"

#define DIRECTORY L"\\dumps"
#define SERVER_URL L"http://crashreportserver/addreport.php"
#define CHECKPOINT_FILE L"crash_checkpoint.dat"

// global singleton instance
CygwinCrashReporter *crashreporter = NULL;

CygwinCrashReporter::CygwinCrashReporter()
{
  pid = 0;
  dumps_dir[0] = 0;
  verbose = FALSE;
  nokill = FALSE;
  server_url = SERVER_URL;
  nodelete = FALSE;
  noreport = FALSE;

  const wchar_t *env_server_url = _wgetenv(L"CYGWIN_CRASHREPORTER_URL");
  if (env_server_url)
    server_url = env_server_url;

  nodelete = (_wgetenv(L"CYGWIN_CRASHREPORTER_NO_DELETE_DUMP") ||
              _wgetenv(L"CYGWIN_CRASHREPORTER_NO_REPORT"));

  noreport = _wgetenv(L"CYGWIN_CRASHREPORTER_NO_REPORT");

  overall_succeeded = FALSE;
  dump_succeeded = FALSE;
  upload_succeeded = FALSE;
  upload_result = google_breakpad::RESULT_FAILED;
}

void
CygwinCrashReporter::usage(FILE *stream, int status)
{
  wchar_t buffer[MAX_PATH];
  GetModuleFileNameW(NULL, buffer, MAX_PATH);
  buffer[MAX_PATH -1] = 0;

  fwprintf(stream, L"\
Usage: %ls [OPTION] IGNORED_FILENAME WIN32PID\n\
\n\
Write and upload minidump from WIN32PID\n\
\n\
 -d, --verbose  be verbose\n\
 -f, --file     additional file to upload\n\
 -h, --help     output help information and exit\n\
 -k, --keep     don't remove the minidump after uploading\n\
 -n, --nokill   don't terminate the dumped process\n\
 -q, --quiet    be quiet while dumping (default)\n\
 -r, --dry-run  don't upload the dump file\n\
 -s, --server   set upload server URL\n\
 -V, --version  output version information and exit\n\
\n", buffer);
  exit (status);
}

void
CygwinCrashReporter::print_version(void)
{
  printf(PACKAGE_NAME " " PACKAGE_VERSION "\n"
         "Minidump crash reporter for Cygwin\n"
         "This is free software; see the source for copying conditions.  There is NO\n"
         "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

int
CygwinCrashReporter::process_command_line(int argc, wchar_t **argv)
{
  int i;

  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] != L'-')
        break;

      switch (argv[i][1])
        {
        case L'f':
          if (argc > i)
            {
              i++;
              extra_files.push_back(argv[i]);
            }
          else
            usage(stderr, 1);
          break;
        case L'n':
          nokill = TRUE;
          break;
        case L'd':
          verbose = TRUE;
          break;
        case L'q':
          verbose = FALSE;
          break;
        case L's':
          if (argc > i)
            {
              i++;
              server_url = argv[i];
            }
          else
            usage(stderr, 1);
          break;
        case L'k':
          nodelete = TRUE;
          break;
        case L'r':
          noreport = TRUE;
          break;
        case L'h':
          usage(stdout, 0);
        case L'V':
          print_version();
          exit (0);
        default:
          usage(stderr, 1);
          return -1;
        }
    }

  if ((i + 2) == argc)
    {
      // filename in argv[i] is ignored
      pid = wcstoul(argv[i+1], NULL, 10);
    }
  else
    {
      usage (stderr, 1);
      return -1;
    }

  return 0;
}

int
CygwinCrashReporter::create_temp_dir(void)
{
  // in the very limited environment a Win32 process inherits from a Cygwin
  // process, GetTempPath() may not return anything useful, so try harder to
  // find a useful path.

  const wchar_t *systemdrive = _wgetenv(L"SYSTEMDRIVE");
  if (systemdrive)
    wcscpy(dumps_dir, systemdrive);
  else
    wcscpy(dumps_dir, L"C:");

  if (_wgetenv(L"TMP") || _wgetenv(L"TEMP") || _wgetenv(L"USERPROFILE"))
      GetTempPathW(MAX_PATH, dumps_dir);

  if (dumps_dir[wcslen(dumps_dir)-1] == L'\\')
    dumps_dir[wcslen(dumps_dir)-1] = 0;

  if (wcslen(dumps_dir) + wcslen(DIRECTORY) < MAX_PATH)
    wcscat(dumps_dir, DIRECTORY);

  if (_wmkdir(dumps_dir) && (errno != EEXIST)) {
    wprintf(L"Unable to create temporary directory '%s' for writing minidump\n", dumps_dir);

    // fall back to the working directory
    dumps_dir[0] = 0;
  }

  return 0;
}

void
CygwinCrashReporter::set_notes(const wchar_t *notes)
{
  reporter_notes = notes;
}

bool
CygwinCrashReporter::crash_reporter_callback(const wchar_t* dump_path,
                                             const wchar_t* minidump_id,
                                             bool succeeded)
{
  wchar_t minidump_path[MAX_PATH];
  swprintf(minidump_path, MAX_PATH, L"%s\\%s.dmp", dump_path, minidump_id);
  minidump_filename = minidump_id;

 if (verbose)
   wprintf(L"minidump file is '%ls'\n", minidump_path);

  dump_succeeded = succeeded;
  if (!succeeded)
    {
      return FALSE;
    }

  if (noreport)
    {
      upload_result = L"disabled";

      if (verbose)
        wprintf(L"Crash report uploading is disabled\n");

      return TRUE;
    }

  wchar_t checkpoint_file[MAX_PATH+1];
  wcscpy(checkpoint_file, dumps_dir);
  wcscat(checkpoint_file, L"\\" CHECKPOINT_FILE);

 if (verbose)
   {
     wprintf(L"checkpoint file is '%ls'\n", checkpoint_file);
     wprintf(L"server URL is '%ls'\n", server_url.c_str());
   }

  std::map<std::wstring,std::wstring> parameters;

  parameters[L"Uploader"] = L"" PACKAGE_NAME "/" PACKAGE_VERSION;
  parameters[L"MinidumpID"] = minidump_id;

  std::wstringstream s;
  s << time(NULL);
  parameters[L"CrashTime"] = s.str();

  if (!reporter_notes.empty())
    parameters[L"Notes"] = reporter_notes;

  std::map<std::wstring,std::wstring> files;

  files[L"upload_file_minidump"] = minidump_path;
  if (!extra_files.empty())
    {
      for (std::vector<std::wstring>::iterator i = extra_files.begin();
           i != extra_files.end();
           i++)
        {
          files[*i] = *i;
          if (verbose)
            wprintf(L"file is '%ls'\n", i->c_str());
        }
    }
  else if (_wgetenv(L"LOGFILE"))
    {
      files[L"logfile"] = _wgetenv(L"LOGFILE");
    }

  google_breakpad::CrashReportSender *pSender = new google_breakpad::CrashReportSender(checkpoint_file);
  pSender->set_max_reports_per_day(10);
  google_breakpad::ReportResult result = pSender->SendCrashReport(server_url, parameters, files, &upload_report_code);

  if (verbose)
    wprintf(L"crash report upload result %d, report code '%ls'\n", result, upload_report_code.c_str());

  if (result == google_breakpad::RESULT_SUCCEEDED)
    {
      upload_succeeded = true;
      upload_result = L"success";
    }
  else
    {
      upload_succeeded = false;
      upload_result = L"unknown error";

      switch (result)
        {
        case google_breakpad::RESULT_FAILED:
          upload_result = L"failed";
          break;
        case google_breakpad::RESULT_REJECTED:
          upload_result = L"rejected";
          break;
        case google_breakpad::RESULT_THROTTLED:
          upload_result = L"throttled";
          break;
        default:
          ;
        }
    }

  delete pSender;

  if (!nodelete)
    {
      int result = _wremove(minidump_path);
      if (result < 0)
        {
          wprintf(L"failed to delete minidump file '%ls'\n", minidump_path);
        }
      else
        {
          if (verbose)
            wprintf(L"minidump file '%ls' deleted\n", minidump_path);
        }
    }
 else
   {
     if (verbose)
       wprintf(L"not deleting minidump file '%ls'\n", minidump_path);
   }

  return (result == google_breakpad::RESULT_SUCCEEDED);
}

bool
callback_friend(CygwinCrashReporter *context, const wchar_t* dump_path,
                const wchar_t* minidump_id, bool succeeded)
{
  return context->crash_reporter_callback(dump_path, minidump_id, succeeded);
}

static bool
callback(const wchar_t* dump_path, const wchar_t* minidump_id, void* context,
         EXCEPTION_POINTERS* exinfo __attribute__((unused)),
         MDRawAssertionInfo* assertion  __attribute__((unused)), bool succeeded)
{
  return callback_friend((CygwinCrashReporter *)context, dump_path, minidump_id, succeeded);
}

void
CygwinCrashReporter::do_dump(void)
{
  if (create_temp_dir())
    return;

  HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE,
                                pid);
  if (process == NULL)
    {
      wprintf(L"Error opening process for dumping%d\n", pid);
      return;
    }

  overall_succeeded = google_breakpad::ExceptionHandler::WriteMinidumpForChild(
     process, // process handle
     0, // child_blamed_thread
     dumps_dir, // dump path
     callback, // Uploader callback
     this // callback context
     );

  CloseHandle(process);

  fflush(stdout);
}

void
CygwinCrashReporter::kill_process(void)
{
  /* Unless nokill is given, behave like dumper and terminate the dumped process */
  if (nokill)
    {
      return;
    }

  HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
  if (process == NULL)
    {
      wprintf(L"Error opening process for termination%d\n", pid);
      return;
    }

  TerminateProcess(process, 128 + 9);
  WaitForSingleObject(process, INFINITE);

  CloseHandle(process);
}

void
CygwinCrashReporter::get_process_info(void)
{
  HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE,
                                pid);
  if (process == NULL)
    {
      wprintf(L"Error opening process for info%d\n", pid);
      return;
    }

  wchar_t buffer[MAX_PATH+1];
  GetProcessImageFileNameW(process, buffer, MAX_PATH);
  buffer[MAX_PATH] = 0;

  if (verbose)
    wprintf(L"crashed process is %ls, winpid %d\n", buffer, pid);

  wchar_t *last = wcsrchr(buffer, '\\');
  if (last)
    process_name = last+1;

  CloseHandle(process);
}
