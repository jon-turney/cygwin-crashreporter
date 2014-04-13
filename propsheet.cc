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

// This is the implementation of the PropSheet class.  This class encapsulates a
// Windows property sheet / wizard and interfaces with the PropertyPage class.
// It's named PropSheet instead of PropertySheet because the latter conflicts
// with the Windows function of the same name.

#include "propsheet.h"
#include "proppage.h"
#include "resource.h"

#include <shlwapi.h>

// Sort of a "hidden" Windows structure.  Used in the PropSheetCallback.
#include <pshpack1.h>
typedef struct DLGTEMPLATEEX
{
  WORD dlgVer;
  WORD signature;
  DWORD helpID;
  DWORD exStyle;
  DWORD style;
  WORD cDlgItems;
  short x;
  short y;
  short cx;
  short cy;
}
DLGTEMPLATEEX, *LPDLGTEMPLATEEX;
#include <poppack.h>

PropSheet::PropSheet ()
{
}

PropSheet::~PropSheet ()
{
}

HPROPSHEETPAGE *
PropSheet::CreatePages ()
{
  HPROPSHEETPAGE *retarray;

  // Create the return array
  retarray = new HPROPSHEETPAGE[PropertyPages.size()];

  // Create the pages with CreatePropertySheetPage().
  // We do it here rather than in the PropertyPages themselves
  // because, for reasons known only to Microsoft, these handles will be
  // destroyed by the property sheet before the PropertySheet() call returns,
  // at least if it's modal (don't know about modeless).
  unsigned int i;
  for (i = 0; i < PropertyPages.size(); i++)
    {
      retarray[i] =
        CreatePropertySheetPage (PropertyPages[i]->GetPROPSHEETPAGEPtr ());

      // Set position info
      if (i == 0)
        {
          PropertyPages[i]->YouAreFirst ();
        }
      else if (i == PropertyPages.size() - 1)
        {
          PropertyPages[i]->YouAreLast ();
        }
      else
        {
          PropertyPages[i]->YouAreMiddle ();
        }
    }

  return retarray;
}

// Stuff needed by the PropSheet wndproc hook
struct PropSheetData
{
  WNDPROC oldWndProc;
  bool gotPage;

  PropSheetData ()
  {
    oldWndProc = 0;
    gotPage = false;
  }

// @@@ Ugly. Really only works because only one PropSheet is used now.
  static PropSheetData& Instance()
  {
    static PropSheetData TheInstance;
    return TheInstance;
  }
};

static LRESULT CALLBACK PropSheetWndProc (HWND hwnd, UINT uMsg,
  WPARAM wParam, LPARAM lParam)
{
  PropSheetData& psd = PropSheetData::Instance();
  switch (uMsg)
    {
    case WM_SYSCOMMAND:
      if ((wParam & 0xfff0) == SC_CLOSE)
        goto areyousure;
      break;
    case WM_COMMAND:
      if (wParam != 2)
        break;
    areyousure:
      if (MessageBox(hwnd, "Are you sure you want to cancel?",
                     "Cygwin crash reporter", MB_YESNO) == IDNO)
        return 0;
      break;
    }

  return CallWindowProc (psd.oldWndProc,
    hwnd, uMsg, wParam, lParam);
}

static int CALLBACK
PropSheetProc (HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
  switch (uMsg)
    {
    case PSCB_PRECREATE:
      return TRUE;

    case PSCB_INITIALIZED:
      {
        /*
          PropSheet() with PSH_USEICONID only sets the small icon,
          so we must set the big icon ourselves
        */
        SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_HIPPO)));
        /*
          Hook into the window proc.
          We need to catch some messages for resizing.
        */
        PropSheetData::Instance().oldWndProc =
          (WNDPROC)GetWindowLongPtr (hwndDlg, GWLP_WNDPROC);
        SetWindowLongPtr (hwndDlg, GWLP_WNDPROC,
                          (LONG_PTR)&PropSheetWndProc);
      }
      return TRUE;
    }
  return TRUE;
}

bool
PropSheet::Create (const Window * Parent, DWORD Style __attribute__((unused)))
{
  PROPSHEETHEADER p;

  PageHandles = CreatePages ();

  p.dwSize = sizeof (PROPSHEETHEADER);
  p.dwFlags = PSH_NOAPPLYNOW | PSH_WIZARD | PSH_USECALLBACK | PSH_USEICONID;
  if (Parent != NULL)
    {
      p.hwndParent = Parent->GetHWND ();
    }
  else
    {
      p.hwndParent = NULL;
    }
  p.hInstance = GetInstance ();
  p.nPages = PropertyPages.size();
  p.pszIcon = MAKEINTRESOURCE(IDI_HIPPO);
  p.nStartPage = 0;
  p.phpage = PageHandles;
  p.pfnCallback = PropSheetProc;

  // Create and run the modal property sheet
  PropertySheet (&p);

  SetHWND (NULL);

  return true;
}

void
PropSheet::SetHWNDFromPage (HWND h)
{
  // If we're a modal dialog, there's no way for us to know our window handle unless
  // one of our pages tells us through this function.
  SetHWND (h);
}

void
PropSheet::AddPage (PropertyPage * p)
{
  // Add a page to the property sheet.
  p->YouAreBeingAddedToASheet (this);
  PropertyPages.push_back(p);
}

bool
PropSheet::SetActivePage (int i)
{
  // Posts a message to the message queue, so this won't block
  return static_cast < bool > (PropSheet_SetCurSel (GetHWND (), NULL, i));
}

bool
PropSheet::SetActivePageByID (int resource_id)
{
  // Posts a message to the message queue, so this won't block
  return static_cast < bool >
    (PropSheet_SetCurSelByID (GetHWND (), resource_id));
}

void
PropSheet::SetButtons (DWORD flags)
{
  // Posts a message to the message queue, so this won't block
  PropSheet_SetWizButtons (GetHWND (), flags);
}

void
PropSheet::PressButton (int button)
{
  PropSheet_PressButton (GetHWND (), button);
}
