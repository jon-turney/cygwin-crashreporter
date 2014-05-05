/*
  main.cc

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

//
// cygwin-crashreporter-cli is compatible with CYGWIN's error_start facility
// and uses breakpad to write a minidump and upload it to a crash report server.
//

#include "crash_reporter.h"

extern "C" int
wmain(int argc, wchar_t **argv)
{
  if (_wgetenv(L"CYGWIN_CRASHREPORTER_DISABLE"))
    return -1;

  crashreporter = new CygwinCrashReporter;
  crashreporter->process_command_line(argc, argv);
  crashreporter->get_process_info();
  crashreporter->do_dump();

  if (!crashreporter->dump_succeeded)
    {
      wprintf(L"Minidump generation failed\n");
    }
  else
    {
      if (!crashreporter->upload_succeeded)
        {
          wprintf(L"Crash report upload failed: %ls\n", crashreporter->upload_result.c_str());
        }
      else
        {
          if (!crashreporter->upload_report_code.empty())
            {
              wprintf(L"Your crash report's ID is %ls\n", crashreporter->upload_report_code.c_str());
            }

          wprintf(L"Crash report sent! Thank you!\n");
        }
    }

  crashreporter->kill_process();

  return crashreporter->overall_succeeded ? 0 : -1;
}
