/*
  pipename.cc

  Copyright 2015 Jon TURNEY

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

#include <windows.h>
#include <stdio.h>

#include "pipename.h"

//
// Construct a pipename containing sessionid
//

const wchar_t *
get_pipename(void)
{
  const size_t buflen = PIPENAME_FORMAT_LENGTH + 10;
  static wchar_t pipename[buflen];
  DWORD processId = GetCurrentProcessId();
  DWORD sessionId = -1;
  ProcessIdToSessionId(processId, &sessionId);
  swprintf(pipename, buflen, PIPENAME_FORMAT, sessionId);
  return pipename;
}
