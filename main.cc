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

#include "crash_reporter.h"

extern "C" int
wmain(int argc, wchar_t **argv)
{
  crashreporter = new CygwinCrashReporter;
  crashreporter->process_command_line(argc, argv);
  bool success = crashreporter->do_dump();

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

  return success ? 0 : -1;
}
