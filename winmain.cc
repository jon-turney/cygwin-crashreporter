/*
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     A copy of the GNU General Public License can be found at
 *     http://www.gnu.org/
 *
 */

#include "splash.h"
#include "notes.h"
#include "progress.h"
#include "done.h"

#include "propsheet.h"
#include "crash_reporter.h"

HINSTANCE hinstance;

static void
main_display(void)
{
  SplashPage Splash;
  NotesPage Notes;
  ProgressPage Progress;
  DonePage Done;

  PropSheet MainWindow;

  // Init window class lib
  Window::SetAppInstance (hinstance);

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
  hinstance = hInstance;

  // use __wargv ???
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  crashreporter = new CygwinCrashReporter;
  crashreporter->process_command_line(argc, argv);

  main_display();

  return 0;
}
