/*
  test.cc

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

#include "breakpad/client/windows/handler/exception_handler.h"
#include "server.h"

DEFINE_ENUM_FLAG_OPERATORS(MINIDUMP_TYPE);

static bool
ShowDumpResults(const wchar_t* dump_path  __attribute__((unused)),
                const wchar_t* minidump_id  __attribute__((unused)),
                void* context  __attribute__((unused)),
                EXCEPTION_POINTERS* exinfo  __attribute__((unused)),
                MDRawAssertionInfo* assertion  __attribute__((unused)),
                bool succeeded)
{
  printf("Dump generation request %s\r\n",
         succeeded ? "succeeded" : "failed");
  fflush(stdout);

  return succeeded;
}

extern "C" int
wmain(int argc __attribute__((unused)),
      wchar_t **argv __attribute__((unused)))
{
  MINIDUMP_TYPE dump_type =  MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithFullMemoryInfo
        | MiniDumpWithThreadInfo | MiniDumpWithFullAuxiliaryState
        | MiniDumpIgnoreInaccessibleMemory | MiniDumpWithTokenInformation
        | MiniDumpWithIndirectlyReferencedMemory;

  // register exception handler with out-of-process dumping
  new google_breakpad::ExceptionHandler(L"C:\\dumps\\",
                                        NULL,
                                        ShowDumpResults,
                                        NULL,
                                        google_breakpad::ExceptionHandler::HANDLER_ALL,
                                        dump_type,
                                        PIPENAME,
                                        NULL);

  // deref zero
  int* x = 0;
  *x = 1;

  return 0;
}
