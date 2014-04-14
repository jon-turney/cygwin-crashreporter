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

/* This is the implementation of the ProgressPage class. */

#include "resource.h"
#include "progress.h"
#include "propsheet.h"
#include "crash_reporter.h"

#include <math.h>

//
// The all important animated hippo
// "The hippo will dance for you while your crash is reported..."
//

#define ID_TIMER 1
#define ANIMATION_FREQUENCY 30 /* Hz */
#define RPS 0.1

static double theta = 0;
static HICON hippo;

// XXX: do this through subclassing DialogProc and remove hook???
static INT_PTR WINAPI
dlgproc(HWND hWnd, UINT message, WPARAM wParam __attribute__((unused)),
        LPARAM lParam __attribute__((unused)))
{
  switch (message)
    {
    case WM_TIMER:
      {
        HWND hAnimWnd = ::GetDlgItem(hWnd, IDC_PROGRESS_ANIMATION);
        HDC hdc = GetDC(hAnimWnd);

        RECT rc;
        GetClientRect(hAnimWnd, &rc);

        int x = (rc.right-rc.left)/2 + (rc.right-rc.left)/4*sin(theta);
        int y = (rc.bottom-rc.top)/2 + (rc.bottom-rc.top)/4*cos(theta);
        theta = theta + (2*M_PI)*(RPS/ANIMATION_FREQUENCY);

        // create a bitmap to do our drawing to
        HDC hdcBuffer = CreateCompatibleDC(hdc);
        HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HGDIOBJ hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

        // erase to background and then draw hippo
        COLORREF c = GetSysColor(COLOR_3DLIGHT);
        HBRUSH b = CreateSolidBrush(c);
        FillRect(hdcBuffer, &rc, b);
        DeleteObject(b);
        DrawIcon(hdcBuffer, x, y, hippo);

        // blt to window
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcBuffer, 0, 0, SRCCOPY);

        SelectObject(hdcBuffer, hbmOldBuffer);
        DeleteDC(hdcBuffer);
        DeleteObject(hbmBuffer);

        ReleaseDC(hAnimWnd, hdc);
      }
      break;
    }

  return 0;
}

ProgressPage::ProgressPage ()
{
}

bool
ProgressPage::Create ()
{
  return PropertyPage::Create (dlgproc, IDD_PROGRESS);
}

static DWORD WINAPI
crash_report_thread(void *p)
{
  // do the dump and upload
  crashreporter->do_dump();

  // signal the dialog to proceed
  PostMessage((HWND)p, WM_APP, 0, 0);

  return 0;
}

void
ProgressPage::OnActivate(void)
{
  // disable next and back buttons
  GetOwner()->SetButtons(0);

  //
  hippo = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_HIPPO));

  SetTimer(GetHWND(), ID_TIMER, 1000/ANIMATION_FREQUENCY, NULL);

  //
  //KillTimer(ID_TIMER);
  //DestroyIcon(hippo);
  //

  DWORD threadID;
  CreateThread (NULL, 0, crash_report_thread, GetHWND(), 0, &threadID);
}

bool
ProgressPage::OnMessageApp(UINT message, WPARAM wParam __attribute__((unused)),
                           LPARAM lParam __attribute__((unused)))
{
  switch (message)
    {
    case WM_APP:
      {
        // enable the next button
        GetOwner()->SetButtons(PSWIZB_NEXT);
      }
      break;
    }

  return true;
}
