/*
  done.cc

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

// This is the implementation of the DonePage class.

#include "resource.h"
#include "done.h"
#include "propsheet.h"
#include "crash_reporter.h"

DonePage::DonePage()
{
}

bool
DonePage::Create()
{
  return PropertyPage::Create(IDD_DONE);
}

void
DonePage::SetControlText(int nIDDlgItem, const std::wstring &s)
{
  ::SetWindowTextW(GetDlgItem(nIDDlgItem), s.c_str());
}

void
DonePage::OnActivate(void)
{
  // disable back button
  GetOwner()->SetButtons(PSWIZB_FINISH);

  // disable the cancel button
  // XXX: sysmenu close is still hooked up to cancel
  SendMessage(GetOwner()->GetHWND(), PSM_CANCELTOCLOSE, 0, 0);

  // show the results of the report operation
  if (!crashreporter->dump_succeeded)
    {
      SetControlText(IDC_DUMP_STATUS, L"Minidump generation failed");
      SetControlText(IDC_UPLOAD_STATUS, L"");
      SetControlText(IDC_UPLOAD_REPORT_CODE, L"");
    }
  else
    {
      SetControlText(IDC_DUMP_STATUS, L"Minidump file " + crashreporter->minidump_filename + L".dmp generated");

      if (!crashreporter->upload_succeeded)
        {
          SetControlText(IDC_UPLOAD_STATUS, L"Upload " + crashreporter->upload_result);
          SetControlText(IDC_UPLOAD_REPORT_CODE, L"");
        }
      else
        {
          SetControlText(IDC_UPLOAD_STATUS, L"Upload to server succeeded");

          if (crashreporter->upload_report_code.empty())
            SetControlText(IDC_UPLOAD_REPORT_CODE, L"");
          else
            SetControlText(IDC_UPLOAD_REPORT_CODE, L"Crash report ID " + crashreporter->upload_report_code);
        }
    }
}
