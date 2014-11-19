/*
  server.cc

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
// cygwin-crashreporter-server is a breakpad crash generation server,
// which will receive requests to write a minidump from other processes, and
// uses the reporting wizard GUI to inform the user of progress as it uses
// breakpad to write a minidump and upload it to a crash report server.
//

// XXX: it doesn't make much sense for crash reporter to be shown to user A
// when a process for user B has crashed, so the server should be per-desktop?
// What about doing automated reporting for crashes in services which don't
// have access to an interactive desktop?

#include "breakpad/client/windows/crash_generation/crash_generation_server.h"
#include "breakpad/client/windows/crash_generation/client_info.h"
#include "server.h"
#include "crash_reporter.h"
#include "window.h"
#include "wizard.h"

#include <stdio.h>

static void clientConnected(void* context __attribute__((unused)),
                            const google_breakpad::ClientInfo* client_info)
{
  wprintf(L"Client pid %ld connected\n", client_info->pid());
  fflush(stdout);
}

static void clientCrashed(void* context __attribute__((unused)),
                          const google_breakpad::ClientInfo* client_info,
                          const std::wstring* dump_path  __attribute__((unused)))
{
  wprintf(L"Client pid %ld requested dump\n", client_info->pid());
  fflush(stdout);

  crashreporter = new CygwinCrashReporter;
  crashreporter->process_environment();
  crashreporter->pid = client_info->pid();
  crashreporter->set_verbose(TRUE); // XXX: debug
  crashreporter->get_process_info();

  main_display();

  delete crashreporter;
}

static void clientExited(void* context __attribute__((unused)),
                         const google_breakpad::ClientInfo* client_info)
{
  wprintf(L"Client pid %ld exited\n", client_info->pid());
  fflush(stdout);
}

int WINAPI
wWinMain(HINSTANCE hInstance,
         HINSTANCE hPrevInstance __attribute__((unused)),
         PWSTR pCmdLine __attribute__((unused)),
         int nCmdShow __attribute__((unused)))
{
  // Init window class lib
  Window::SetAppInstance(hInstance);

  // Get dump directory
  std::wstring dump_dir = crashreporter->get_dumps_dir();

  // Start crash generation server
  // This will fail if one is already listening on the named pipe
  static google_breakpad::CrashGenerationServer* crash_server = NULL;
  crash_server = new google_breakpad::CrashGenerationServer(PIPENAME,
                                                            NULL,
                                                            clientConnected,
                                                            NULL,
                                                            clientCrashed,
                                                            NULL,
                                                            clientExited,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            false, // we will generate the dump in ClientCrashed
                                                            &dump_dir);

  if (!crash_server->Start()) {
    wprintf(L"Unable to start server (maybe one is already running?)\n");
    delete crash_server;
    crash_server = NULL;
    return 1;
  }

  // Pump message queue until we receive notice to terminate
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    if (msg.message == WM_ENDSESSION)
      PostQuitMessage(0);

    DispatchMessage(&msg);
  }

  // Shut down cleanly
  delete crash_server;
  crash_server = NULL;

  return 0;
}
