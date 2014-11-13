/*
  wizard.cc

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

#include "splash.h"
#include "notes.h"
#include "progress.h"
#include "done.h"

#include "propsheet.h"

void
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
