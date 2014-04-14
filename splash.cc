/* splash.cc

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

// This is the implementation of the SplashPage class.  Since the splash page
// has little to do, there's not much here.

#include "resource.h"
#include "splash.h"

SplashPage::SplashPage()
{
}

bool
SplashPage::Create()
{
  return PropertyPage::Create(IDD_SPLASH);
}
