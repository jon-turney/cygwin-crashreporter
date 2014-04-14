/*
 * Copyright (c) 2001, 2002, 2003 Gary R. Van Sickle.
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     A copy of the GNU General Public License can be found at
 *     http://www.gnu.org/
 *
 * Written by Gary R. Van Sickle <g.r.vansickle@worldnet.att.net>
 *
 */

/* This is the implementation of the NotesPage class. */

#include "resource.h"
#include "notes.h"
#include "crash_reporter.h"

NotesPage::NotesPage ()
{
}

bool
NotesPage::Create ()
{
  return PropertyPage::Create (IDD_NOTES);
}

void
NotesPage::OnDeactivate()
{
  HWND hwnd = GetDlgItem(IDC_NOTES_EDIT);

  int length = GetWindowTextLengthW(hwnd) + 1;
  wchar_t *buf = new wchar_t[length];
  if (buf)
    {
      GetWindowTextW(hwnd, buf, length);
      buf[length] = 0;

      crashreporter->set_notes(buf);
    }

  delete buf;
}
