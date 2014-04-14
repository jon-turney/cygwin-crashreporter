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

/* This is the implementation of the DonePage class. */

#include "resource.h"
#include "done.h"
#include "propsheet.h"
#include "crash_reporter.h"

DonePage::DonePage ()
{
}

bool
DonePage::Create ()
{
  return PropertyPage::Create (IDD_DONE);
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
