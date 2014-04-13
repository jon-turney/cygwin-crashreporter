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
#include "propsheet.h"

HINSTANCE hinstance;

static void
main_display(void)
{
  SplashPage Splash;
  //
  //
  //

  PropSheet MainWindow;

  // Init window class lib
  Window::SetAppInstance (hinstance);

  // Create pages
  Splash.Create ();
  // Notes.Create ();
  // Progress.Create ();
  // Done.Create ();

  // Add pages to sheet
  MainWindow.AddPage (&Splash);
  // MainWindow.AddPage (&Notes);
  // MainWindow.AddPage (&Progress);
  // MainWindow.AddPage (&Done);

  // Create the PropSheet main window
  MainWindow.Create ();

}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
  hinstance = hInstance;

  main_display();
}
