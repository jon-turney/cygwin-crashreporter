/*
  notes.cc

  Copyright 2014 Jon TURNEY

  This file is part of cygwin_crash_reporter

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

// This is the implementation of the NotesPage class.

#include "resource.h"
#include "notes.h"
#include "propsheet.h"
#include "crash_reporter.h"

NotesPage::NotesPage()
{
}

bool
NotesPage::Create()
{
  return PropertyPage::Create (IDD_NOTES);
}

void
NotesPage::OnDeactivate()
{
  HWND hwnd = GetDlgItem(IDC_NOTES_EDIT);

  int length = GetWindowTextLengthW(hwnd);
  if (length > 0)
    {
      wchar_t *buf = new wchar_t[length + 1];

      if (buf)
        {
          GetWindowTextW(hwnd, buf, length);
          buf[length] = 0;

          crashreporter->set_notes(buf);
        }

      delete buf;
    }
}
