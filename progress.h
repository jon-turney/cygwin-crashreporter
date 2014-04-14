/*
 * Copyright (c) 2001, Gary R. Van Sickle.
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

#ifndef PROGRESS_H
#define PROGRESS_H

// This is the header for the ProgressPage class.
#include "proppage.h"

class ProgressPage: public PropertyPage
{
public:
  ProgressPage ();
  virtual ~ ProgressPage ()
  {
  };

  bool Create ();
  virtual void OnActivate(void);
  virtual bool OnMessageApp(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif /* PROGRESS_H */
