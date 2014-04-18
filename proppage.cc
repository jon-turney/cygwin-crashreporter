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

// This is the implementation of the PropertyPage class.  It works closely with
// the PropSheet class to implement a single page of the property sheet.

#include "proppage.h"
#include "propsheet.h"

#include <shellapi.h>
#include "resource.h"

bool PropertyPage::DoOnceForSheet = true;

PropertyPage::PropertyPage ()
{
  proc = NULL;
  cmdproc = NULL;
  IsFirst = false;
  IsLast = false;
}

PropertyPage::~PropertyPage ()
{
}

bool PropertyPage::Create (int TemplateID)
{
  return Create (NULL, NULL, TemplateID);
}

bool PropertyPage::Create (DLGPROC dlgproc, int TemplateID)
{
  return Create (dlgproc, NULL, TemplateID);
}

bool
PropertyPage::Create (DLGPROC dlgproc,
                      BOOL (*cproc) (HWND h, int id, HWND hwndctl,
                                     UINT code), int TemplateID)
{
  psp.dwSize = sizeof (PROPSHEETPAGE);
  psp.dwFlags = 0;
  psp.hInstance = GetInstance ();
  psp.pfnDlgProc = FirstDialogProcReflector;
  psp.pszTemplate = MAKEINTRESOURCE(TemplateID);
  psp.lParam = (LPARAM) this;
  psp.pfnCallback = NULL;

  proc = dlgproc;
  cmdproc = cproc;

  return true;
}

INT_PTR CALLBACK
PropertyPage::FirstDialogProcReflector (HWND hwnd, UINT message,
                                        WPARAM wParam, LPARAM lParam)
{
  PropertyPage *This;

  if (message != WM_INITDIALOG)
    {
      // Don't handle anything until we get a WM_INITDIALOG message, which
      // will have our 'this' pointer with it.
      return FALSE;
    }

  This = (PropertyPage *) (((PROPSHEETPAGE *) lParam)->lParam);

  SetWindowLongPtr (hwnd, DWLP_USER, (LONG_PTR) This);
  SetWindowLongPtr (hwnd, DWLP_DLGPROC, (LONG_PTR) DialogProcReflector);

  This->SetHWND (hwnd);
  return This->DialogProc (message, wParam, lParam);
}

INT_PTR CALLBACK
PropertyPage::DialogProcReflector (HWND hwnd, UINT message, WPARAM wParam,
                                   LPARAM lParam)
{
  PropertyPage *This;

  This = (PropertyPage *) GetWindowLongPtr (hwnd, DWLP_USER);

  return This->DialogProc (message, wParam, lParam);
}

INT_PTR CALLBACK
PropertyPage::DialogProc (UINT message, WPARAM wParam, LPARAM lParam)
{
  {
    if (proc != NULL)
    {
      proc (GetHWND (), message, wParam, lParam);
    }

    switch (message)
    {
      case WM_INITDIALOG:
        {
          OnInit ();

          // TRUE = Set focus to default control (in wParam).
          return TRUE;
        }
      case WM_NOTIFY:
        switch (((NMHDR FAR *) lParam)->code)
        {
          case PSN_APPLY:
            {
              SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, PSNRET_NOERROR);
              return TRUE;
            }
          case PSN_SETACTIVE:
            {
              if (DoOnceForSheet)
              {
                // Tell our parent PropSheet what its own HWND is.
                GetOwner ()->SetHWNDFromPage (((NMHDR FAR *) lParam)->
                                              hwndFrom);
                //                GetOwner ()->CenterWindow ();
                DoOnceForSheet = false;
              }

              // Set the wizard buttons appropriately
              if (IsFirst)
              {
                // Disable "Back" on first page.
                GetOwner ()->SetButtons (PSWIZB_NEXT);
              }
              else if (IsLast)
              {
                // Disable "Next", enable "Finish" on last page
                GetOwner ()->SetButtons (PSWIZB_BACK | PSWIZB_FINISH);
                PropSheet_SetFinishText(GetOwner()->GetHWND(), "&Finish");
              }
              else
              {
                // Middle page, enable both "Next" and "Back" buttons
                GetOwner ()->SetButtons (PSWIZB_BACK | PSWIZB_NEXT);
              }

              if(!wantsActivation())
              {
                ::SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, -1);
                return TRUE;
              }

              OnActivate ();

              // 0 == Accept activation, -1 = Don't accept
              ::SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, 0);
              return TRUE;
            }
            break;
          case PSN_KILLACTIVE:
            {
              OnDeactivate ();
              // FALSE = Allow deactivation
              SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, FALSE);
              return TRUE;
            }
          case PSN_WIZNEXT:
            {
              LONG retval;
              retval = OnNext ();
              SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, retval);
              return TRUE;
            }
          case PSN_WIZBACK:
            {
              LONG retval;
              retval = OnBack ();
              SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, retval);
              return TRUE;
            }
          case PSN_WIZFINISH:
            {
              OnFinish ();
              // False = Allow the wizard to finish
              SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, FALSE);
              return TRUE;
            }
          default:
            {
              // Unrecognized notification
              return FALSE;
            }
        }
        break;
      case WM_COMMAND:
        {
          bool retval;

          retval =
            OnMessageCmd (LOWORD (wParam), (HWND) lParam, HIWORD (wParam));
          if (retval == true)
          {
            // Handled, return 0
            SetWindowLongPtr (GetHWND (), DWLP_MSGRESULT, 0);
            return TRUE;
          }
          else if (cmdproc != NULL)
          {
            cmdproc (GetHWND(), LOWORD(wParam), (HWND)lParam, HIWORD(wParam));
            return 0;
          }
          break;
        }
      default:
        break;
    }

    if ((message >= WM_APP) && (message < 0xC000))
    {
      // It's a private app message
      return OnMessageApp (message, wParam, lParam);
    }
  }

  // Wasn't handled
  return FALSE;
}
