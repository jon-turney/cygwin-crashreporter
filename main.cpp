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

#define DUMPS_DIR L"C:\\dumps\\"
#define CHECKPOINT_FILE L"crash_checkpoint.dat"
#define SERVER_URL L"http://wollstonecraft/addreport.php"
#define TITLE L"Crash Reporter"

bool
callback(const wchar_t* dump_path,
         const wchar_t* minidump_id,
         void* context,
         EXCEPTION_POINTERS* exinfo,
         MDRawAssertionInfo* assertion,
         bool succeeded)
{
  std::wstring report_code;
  std::map<std::wstring,std::wstring> parameters;

  wchar_t minidump_path[MAX_PATH];
  swprintf(minidump_path, MAX_PATH, L"%s\\%s.dmp", dump_path, minidump_id);

  if (!succeeded)
    {
      MessageBoxW(NULL, L"Minidump generation failed", TITLE, MB_OK);
      return FALSE;
    }

  google_breakpad::CrashReportSender *pSender = new google_breakpad::CrashReportSender(CHECKPOINT_FILE);
  google_breakpad::ReportResult result = pSender->SendCrashReport(SERVER_URL, parameters, minidump_path, &report_code);

  if (result == google_breakpad::RESULT_SUCCEEDED)
    {
      report_code.c_str();
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

int
main (int argc, char **argv)
{
  char *filename;
  DWORD pid;

  if (argc > 2)
    {
      filename = argv[1]; // ignored
      pid = strtoul(argv[2], NULL, 10);
    }

  HANDLE process  = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE,
                                FALSE,
                                pid);
  if (process == INVALID_HANDLE_VALUE)
    {
      MessageBoxW(NULL, L"Error opening process", TITLE, MB_OK);
      return 1;
    }

  if (_wmkdir(DUMPS_DIR) && (errno != EEXIST)) {
    MessageBoxW(NULL, L"Unable to create dump directory", TITLE, MB_OK);
    return 1;
  }

  bool success = google_breakpad::ExceptionHandler::WriteMinidumpForChild(
     process, // process handle
     0, // child_blamed_thread
     DUMPS_DIR, // dump path
     callback, // Uploader callback
     NULL // callback context
     );

#if 0
  /* Unless nokill is given, behave like dumper and terminate the dumped process */
  if (!nokill)
    {
      TerminateProcess(process, 128 + 9);
      WaitForSingleObject(process, INFINITE);
    }
#endif
  CloseHandle(process);

  return success ? 0 : 1;
}
