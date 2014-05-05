/*
  splash.cc

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

// This is the implementation of the SplashPage class.

#include "resource.h"
#include "splash.h"
#include "crash_reporter.h"

#include "sstream"

static HFONT hBoldFont;

static INT_PTR WINAPI
dlgproc(HWND hWnd __attribute__((unused)), UINT message, WPARAM wParam,
        LPARAM lParam)
{
  switch (message)
    {
    case WM_CTLCOLORSTATIC:
      {
        HDC hdc = (HDC)wParam;
        HWND hwndCtl = (HWND)lParam;

        if (GetDlgCtrlID(hwndCtl) == IDC_BOLD)
          {
            // get the current font, add bold, and set it back
            if (hBoldFont == 0)
              {
                TEXTMETRIC tm;
                GetTextMetrics(hdc, &tm);

                LOGFONT lf;
                memset ((void *)&lf, 0, sizeof(LOGFONT));
                lf.lfUnderline = FALSE;
                lf.lfHeight = tm.tmHeight;
                lf.lfWeight = FW_BOLD;
                lf.lfItalic = tm.tmItalic;
                lf.lfStrikeOut = tm.tmStruckOut;
                lf.lfCharSet = tm.tmCharSet;
                lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
                lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
                lf.lfQuality = DEFAULT_QUALITY;
                lf.lfPitchAndFamily = tm.tmPitchAndFamily;
                GetTextFace(hdc, LF_FACESIZE, lf.lfFaceName);

                hBoldFont = CreateFontIndirect(&lf);
              }

            if (hBoldFont)
              SelectObject(hdc, hBoldFont);

            return 0; // use default colors
          }
      }
    }

  return 0;
}

SplashPage::SplashPage()
{
}

bool
SplashPage::Create()
{
  return PropertyPage::Create(dlgproc, IDD_SPLASH);
}

void
SplashPage::SetControlText(int nIDDlgItem, const std::wstring &s)
{
  ::SetWindowTextW(GetDlgItem(nIDDlgItem), s.c_str());
}

void
SplashPage::OnActivate()
{
  std::wstringstream format;
  format << L"The crashed process is ";
  if (!crashreporter->process_name.empty())
    format << crashreporter->process_name << ", ";
  format << L"winpid " << crashreporter->pid << L".";

  SetControlText(IDC_PROCESS_INFO, format.str().c_str());
}
