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

#ifndef DONE_H
#define DONE_H

// This is the header for the DonePage class.
#include "proppage.h"

class DonePage: public PropertyPage
{
public:
  DonePage();
  virtual ~DonePage()
  {
  };

  bool Create();
  virtual void OnActivate(void);

 private:
  void SetControlText(int nIDDlgItem, const std::wstring &s);
};

#endif /* DONE_H */
