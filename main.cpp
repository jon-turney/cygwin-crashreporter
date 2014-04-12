/* main.cc

   Copyright 2014 Jon TURNEY

   This file is part of cygwin_crash_reporter

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License (file COPYING.dumper) for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
*/

//
// A refinement on cygwin's minidumper tool, this is compatible with dumper, but
// uses breakpad to write a minidump and upload it to a crash report server.
//

#include <breakpad/client/windows/handler/exception_handler.h>
#include <breakpad/client/windows/sender/crash_report_sender.h>

#define TITLE L"Cygwin Crash Reporter"
#define DIRECTORY L"\\dumps"
#define CHECKPOINT_FILE L"crash_checkpoint.dat"
#define SERVER_URL L"http://wollstonecraft/addreport.php"

bool verbose = FALSE;
bool nokill = FALSE;
wchar_t dumps_dir[MAX_PATH+1];
std::wstring server_url = SERVER_URL;

bool
callback(const wchar_t* dump_path,
         const wchar_t* minidump_id,
         void* context __attribute__((unused)),
         EXCEPTION_POINTERS* exinfo __attribute__((unused)),
         MDRawAssertionInfo* assertion __attribute__((unused)),
         bool succeeded)
{
  std::wstring report_code;
  std::map<std::wstring,std::wstring> parameters;

  parameters[L"Uploader"] = L"" PACKAGE_NAME "/" PACKAGE_VERSION;
  parameters[L"Notes"] = L"Custom Note from Breakpad Test";

  wchar_t minidump_path[MAX_PATH];
  swprintf(minidump_path, MAX_PATH, L"%s\\%s.dmp", dump_path, minidump_id);

 if (verbose)
   wprintf(L"minidump file '%ls'\n", minidump_path);

  if (!succeeded)
    {
      MessageBoxW(NULL, L"Minidump generation failed", TITLE, MB_OK);
      return FALSE;
    }

  wchar_t checkpoint_file[MAX_PATH+1];
  wcscpy(checkpoint_file, dumps_dir);
  wcscat(checkpoint_file, L"\\" CHECKPOINT_FILE);

 if (verbose)
   {
     wprintf(L"checkpoint file '%ls'\n", checkpoint_file);
     wprintf(L"server URL is '%ls'\n", server_url.c_str());
   }

  google_breakpad::CrashReportSender *pSender = new google_breakpad::CrashReportSender(checkpoint_file);
  pSender->set_max_reports_per_day(10);
  google_breakpad::ReportResult result = pSender->SendCrashReport(server_url, parameters, minidump_path, &report_code);

  if (verbose)
    wprintf(L"minidump upload result %d, report code '%s'\n", result, report_code.c_str());

  if (result == google_breakpad::RESULT_SUCCEEDED)
    {
      MessageBoxW(NULL, L"Sent! Thank you!", TITLE, MB_OK);
    }
  else
    {
      const wchar_t *reason = L"Unknown error";

      switch (result)
        {
        case google_breakpad::RESULT_FAILED:
          reason =  L"Upload failed";
          break;
        case google_breakpad::RESULT_REJECTED:
          reason = L"Upload rejected";
          break;
        case google_breakpad::RESULT_THROTTLED:
          reason = L"Upload throttled";
          break;
        default:
          ;
        }

      MessageBoxW(NULL, reason, TITLE, MB_OK);
    }

  delete pSender;

  // XXX: delete dump file after sending?

  return (result == google_breakpad::RESULT_SUCCEEDED);
}

static void
usage(FILE *stream, int status)
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
 -h, --help     output help information and exit\n\
 -n, --nokill   don't terminate the dumped process\n\
 -q, --quiet    be quiet while dumping (default)\n\
 -s, --server   set upload server URL\n\
 -V, --version  output version information and exit\n\
\n", buffer);
  exit (status);
}

static void
print_version(void)
{
  printf(PACKAGE_NAME " " PACKAGE_VERSION "\n"
         "Minidump crash reporter for Cygwin\n"
         "This is free software; see the source for copying conditions.  There is NO\n"
         "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

extern "C" int
wmain(int argc, wchar_t **argv)
{
  DWORD pid;
  int i;

  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] != L'-')
        break;

      switch (argv[i][1])
        {
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

  HANDLE process  = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE,
                                FALSE,
                                pid);
  if (process == INVALID_HANDLE_VALUE)
    {
      MessageBoxW(NULL, L"Error opening process", TITLE, MB_OK);
      return -1;
    }

  if (!GetTempPathW(MAX_PATH, dumps_dir))
    wcscpy(dumps_dir, L"C:");

  if (dumps_dir[wcslen(dumps_dir)-1] == L'\\')
    dumps_dir[wcslen(dumps_dir)-1] = 0;

  if (wcslen(dumps_dir) + wcslen(DIRECTORY) < MAX_PATH)
    wcscat(dumps_dir, DIRECTORY);

  if (_wmkdir(dumps_dir) && (errno != EEXIST)) {
    MessageBoxW(NULL, L"Unable to create dump directory", TITLE, MB_OK);
    return -1;
  }

  bool success = google_breakpad::ExceptionHandler::WriteMinidumpForChild(
     process, // process handle
     0, // child_blamed_thread
     dumps_dir, // dump path
     callback, // Uploader callback
     NULL // callback context
     );

  /* Unless nokill is given, behave like dumper and terminate the dumped process */
  if (!nokill)
    {
      TerminateProcess(process, 128 + 9);
      WaitForSingleObject(process, INFINITE);
    }

  CloseHandle(process);

  return success ? 0 : -1;
}
