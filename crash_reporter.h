/*
  crash_reporter.h

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

#include <string>
#include <stdio.h>
#include <windows.h>

class CygwinCrashReporter
{
public:
  CygwinCrashReporter();
  int process_command_line(int argc, wchar_t **argv);
  void do_dump(void);
  void set_notes(const wchar_t *notes);
  void kill_process(void);

  // result state -- XXX: should be private!
  bool overall_succeeded;
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
  bool nodelete;
  std::wstring server_url;
  std::wstring reporter_notes;

  // implementation helpers
  void usage(FILE *stream, int status);
  void print_version(void);
  int create_temp_dir(void);
  bool crash_reporter_callback(const wchar_t* dump_path,
                               const wchar_t* minidump_id,
                               bool succeeded);

  friend bool callback_friend(CygwinCrashReporter *context,
                              const wchar_t* dump_path,
                              const wchar_t* minidump_id,
                              bool succeeded);
};

// the global instance we use
extern CygwinCrashReporter *crashreporter;
