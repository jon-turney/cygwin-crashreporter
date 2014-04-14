/* crash_reporter.h

   Copyright 2014 Jon TURNEY

   This file is part of cygwin_crash_reporter

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License (file COPYING.dumper) for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <string>
#include <stdio.h>
#include <windows.h>

class CygwinCrashReporter
{
public:
  CygwinCrashReporter();
  int process_command_line(int argc, wchar_t **argv);
  int do_dump(void);
  void set_notes(const wchar_t *notes);

  // should be made private via friend callback
  bool crash_reporter_callback(const wchar_t* dump_path,
                               const wchar_t* minidump_id,
                               bool succeeded);

  // result state -- XXX: should be private!
  std::wstring minidump_filename;
  bool dump_succeeded;
  bool upload_succeeded;
  std::wstring upload_result;
  std::wstring upload_report_code;

private:
  // input state
  DWORD pid;
  wchar_t dumps_dir[MAX_PATH+1];
  bool verbose;
  bool nokill;
  std::wstring server_url;
  std::wstring reporter_notes;

  // implementation helpers
  void usage(FILE *stream, int status);
  void print_version(void);
  int create_temp_dir(void);
};

// the global instance we use
extern CygwinCrashReporter *crashreporter;
