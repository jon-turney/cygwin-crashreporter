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
//

#define ID_TIMER 1
#define ANIMATION_FREQUENCY 20 /* Hz */
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
        HDC hdc = GetDC(hWnd);

        // pixels, not dialog units!
        int x =  170 + 50*sin(theta);
        int y = 90 + 50*cos(theta);
        DrawIcon(hdc, x, y, hippo);
        theta = theta + (2*M_PI)*(RPS/ANIMATION_FREQUENCY);

        ReleaseDC(hWnd, hdc);
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
