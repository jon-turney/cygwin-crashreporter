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
#include <vector>
#include <stdio.h>
#include <windows.h>

namespace google_breakpad
{
  class ClientInfo;
}

class CygwinCrashReporter
{
public:
  CygwinCrashReporter();
  void process_environment(void);
  int process_command_line(int argc, wchar_t **argv);
  void process_client_info(const google_breakpad::ClientInfo* client_info);
  void get_process_info(void);
  void do_dump(void);
  typedef void (*DumpCallback)(CygwinCrashReporter *);
  void set_dump_callback(DumpCallback dump_callback);
  void set_notes(const wchar_t *notes);
  void set_verbose(bool b);
  void kill_process(void);
  static std::wstring get_dumps_dir(void);
  bool upload_callback(const wchar_t* dump_path,
                       const wchar_t* minidump_id,
                       bool succeeded);

  // info state
  DWORD pid;
  std::wstring process_name;
  const google_breakpad::ClientInfo* client_info;

  // result state -- XXX: should be private!
  bool overall_succeeded;
  std::wstring minidump_filename;
  bool dump_succeeded;
  bool upload_succeeded;
  std::wstring upload_result;
  std::wstring upload_report_code;

private:
  static wchar_t dumps_dir[MAX_PATH+1];

  // input state
  bool verbose;
  bool nokill;
  bool nodelete;
  bool noreport;
  std::wstring server_url;
  std::wstring reporter_notes;
  std::vector<std::wstring> extra_files;

  //
  DumpCallback dump_callback;

  // implementation helpers
  void usage(FILE *stream, int status);
  void print_version(void);
  friend bool callback_friend(CygwinCrashReporter *context,
                              const wchar_t* dump_path,
                              const wchar_t* minidump_id,
                              bool succeeded);
  static void create_dumps_dir(void);
  void dump(void);
};

// the global instance we use
extern CygwinCrashReporter *crashreporter;
