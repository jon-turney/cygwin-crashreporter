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

#include "splash.h"
#include "notes.h"
#include "progress.h"
#include "done.h"

#include "propsheet.h"
#include "crash_reporter.h"

static void
main_display(void)
{
  SplashPage Splash;
  NotesPage Notes;
  ProgressPage Progress;
  DonePage Done;

  PropSheet MainWindow;

  // Create pages
  Splash.Create();
  Notes.Create();
  Progress.Create();
  Done.Create();

  // Add pages to sheet
  MainWindow.AddPage(&Splash);
  MainWindow.AddPage(&Notes);
  MainWindow.AddPage(&Progress);
  MainWindow.AddPage(&Done);

  // Create and run the PropSheet main window
  MainWindow.Create();
}

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
  crashreporter->process_command_line(argc, argv);

  main_display();

  return crashreporter->overall_succeeded ? 0 : -1;
}
