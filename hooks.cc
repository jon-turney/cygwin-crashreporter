/*
  hooks.cc

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

//
// cygwin-crashreporter-hooks DLL exposes functions to start a crash generation
// server process (if one is not already running), and to request it to perform
// a dump
//

#include "breakpad/client/windows/crash_generation/crash_generation_client.h"
#include "server.h"

DEFINE_ENUM_FLAG_OPERATORS(MINIDUMP_TYPE);

static google_breakpad::CrashGenerationClient *crash_generation_client = NULL;

static void
addCustomInfoEntry(google_breakpad::CustomInfoEntry** entries, size_t *count, const wchar_t *name, const char *value)
{
  if (!value)
    return;

  // convert value to wchar string
  size_t l = mbstowcs(NULL, value, 0);
  wchar_t wvalue[l+1];
  mbstowcs(wvalue, value, l+1);

  google_breakpad::CustomInfoEntry entry(name, wvalue);

  *entries = (google_breakpad::CustomInfoEntry*)realloc(*entries, sizeof(const google_breakpad::CustomInfoEntry) * ((*count) + 1));
  (*entries)[*count] = entry;
  (*count)++;
}

//
// CygwinCrashReporterInit()
//
// connect to crash generation server process, starting one if necessary
//
// returns non-zero on success
//

extern "C"  __attribute__ ((dllexport))
int CygwinCrashReporterInit(const char *url, const char *logfile)
{
  size_t count = 0;
  google_breakpad::CustomInfoEntry* entries = NULL;

  addCustomInfoEntry(&entries, &count, L"CYGWIN_CRASHREPORTER_URL", url);
  addCustomInfoEntry(&entries, &count, L"CYGWIN_CRASHREPORTER_LOGFILE", logfile);

  google_breakpad::CustomClientInfo info = { entries, count };

  // XXX: needs ability to customize
  MINIDUMP_TYPE dump_type =  MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithFullMemoryInfo
        | MiniDumpWithThreadInfo | MiniDumpWithFullAuxiliaryState
        | MiniDumpIgnoreInaccessibleMemory | MiniDumpWithTokenInformation
        | MiniDumpWithIndirectlyReferencedMemory;

  // if we've already been called, disconnect
  if (crash_generation_client)
    delete crash_generation_client;

  // locate an out-of-process crash dump generation server, if one is already
  // running
  crash_generation_client = new google_breakpad::CrashGenerationClient(PIPENAME,
                                                                       dump_type,
                                                                       &info);
  if (crash_generation_client->Register())
    {
      return 1;
    }

  // if that failed, start an out-of-process crash dump generation server
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  memset(&si, 0, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);

  char *cmd = strdup("cygwin-crashreporter-server.exe");
  if (CreateProcess(NULL,
                    cmd,
                    NULL,
                    NULL,
                    FALSE,
                    CREATE_NO_WINDOW,
                    NULL,
                    NULL,
                    &si,
                    &pi))
    {
      for (int i = 0; i < 10 ; i++)
        {
          // and try to connect...
          if (crash_generation_client->Register())
            {
              return 2+i;
            }

          // allow a little time for the server to become ready to accept registrations
          Sleep(100);
        }
    }

  return 0;
}

//
// CygwinCrashReporterReport()
//
// request an out-of-process dump for this process
//
// EXCEPTION_POINTERS can be NULL
//

extern "C"  __attribute__ ((dllexport))
void CygwinCrashReporterReport(EXCEPTION_POINTERS *ep)
{

  if (crash_generation_client)
    crash_generation_client->RequestDump(ep);
}
