/*
  progress.cc

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

// This is the implementation of the ProgressPage class.

#include "resource.h"
#include "progress.h"
#include "propsheet.h"
#include "crash_reporter.h"

#include <math.h>

//
// The all important animated hippo
// "The hippos will dance for you while your crash is reported..."
//

#define ID_TIMER 1
#define ANIMATION_FREQUENCY 50 /* Hz */
#define RPS 0.1
#define NUM_HIPPOS_INNER 5
#define NUM_HIPPOS_OUTER 7
#define GEARING 1.1

static double theta = 0;
static HICON hippo;

static void
draw_hippo(HDC hdcBuffer, RECT *rc, double theta, double frac)
{
  int x = rc->right/2 + rc->right*frac*sin(theta) - GetSystemMetrics(SM_CXICON)/2;
  int y = rc->bottom/2 + rc->bottom*frac*cos(theta) - GetSystemMetrics(SM_CYICON)/2;

  DrawIcon(hdcBuffer, x, y, hippo);
}

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

        // draw hippos
        draw_hippo(hdcBuffer, &rc, 0, 0);
        for (int i = 0; i < NUM_HIPPOS_INNER; i++)
          draw_hippo(hdcBuffer, &rc, theta + i*M_PI*2/NUM_HIPPOS_INNER, 0.16);
        for (int i = 0; i < NUM_HIPPOS_OUTER; i++)
          draw_hippo(hdcBuffer, &rc, -theta*GEARING + i*M_PI*2/NUM_HIPPOS_OUTER, 0.33);

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

  // start the progress animation
  hippo = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_HIPPO));
  SetTimer(GetHWND(), ID_TIMER, 1000/ANIMATION_FREQUENCY, NULL);

  // do crash reporting in a separate thread so it doesn't block UI
  DWORD threadID;
  CreateThread (NULL, 0, crash_report_thread, GetHWND(), 0, &threadID);
}

void
ProgressPage::OnDeactivate()
{
  KillTimer(GetHWND(), ID_TIMER);
  DestroyIcon(hippo);
}

bool
ProgressPage::OnMessageApp(UINT message, WPARAM wParam __attribute__((unused)),
                           LPARAM lParam __attribute__((unused)))
{
  switch (message)
    {
    case WM_APP:
      {
        // enable and press the next button
        GetOwner()->SetButtons(PSWIZB_NEXT);
        GetOwner()->PressButton(PSBTN_NEXT);
      }
      break;
    }

  return true;
}
