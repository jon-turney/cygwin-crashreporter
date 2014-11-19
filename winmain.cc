/*
  winmain.cc

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
// cygwin-crashreporter-gui is compatible with CYGWIN's error_start facility
// provides a simple GUI to inform the user of progress as it uses breakpad to
// write a minidump and upload it to a crash report server.
//

#include "window.h"
#include "wizard.h"
#include "crash_reporter.h"

int WINAPI
wWinMain(HINSTANCE hInstance,
         HINSTANCE hPrevInstance __attribute__((unused)),
         PWSTR pCmdLine __attribute__((unused)),
         int nCmdShow __attribute__((unused)))
{
  if (_wgetenv(L"CYGWIN_CRASHREPORTER_DISABLE"))
    return -1;

  // Init window class lib
  Window::SetAppInstance(hInstance);

  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  crashreporter = new CygwinCrashReporter;
  crashreporter->process_environment();
  crashreporter->process_command_line(argc, argv);
  crashreporter->get_process_info();

  // It doesn't make much sense to put up our GUI if we can't receive user
  // input, which can happen if the crashed program was run from a service.
  HDESK hDesk = OpenInputDesktop(DF_ALLOWOTHERACCOUNTHOOK, FALSE, GENERIC_WRITE);
  if (hDesk)
    {
      main_display();

      CloseDesktop(hDesk);
    }
  else
    {
      wprintf(L"No access to interactive desktop\n");
    }

  // Unless nokill is given, behave like dumper and terminate the dumped process
  crashreporter->kill_process();

  return crashreporter->overall_succeeded ? 0 : -1;
}
