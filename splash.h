/*
  splash.h

  Copyright 2014 Jon TURNEY

  This file is part of cygwin_crash_reporter

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

#ifndef SPLASH_H
#define SPLASH_H

// This is the header for the SplashPage class.

#include "proppage.h"

class SplashPage: public PropertyPage
{
public:
  SplashPage();
  virtual ~SplashPage()
  {
  };

  bool Create();
};

#endif /* SPLASH_H */
