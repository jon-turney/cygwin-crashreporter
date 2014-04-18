/*
 * Copyright (c) 2001, 2002, 2003 Gary R. Van Sickle.
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

#ifndef WINDOW_H
#define WINDOW_H

// This is the header for the Window class.  It serves both as a window class in
// its own right and as a base class for other window-like classes
// (e.g. PropertyPage, PropSheet).

#include <vector>
#include <map>
#include <string>

#include <windows.h>

class Window
{
public:
  Window();
  virtual ~Window();

  virtual bool Create(Window * Parent = NULL,
                       DWORD Style =
                       WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN);

  static void SetAppInstance(HINSTANCE h)
  {
    // This only has to be called once in the entire app, before
    // any Windows are created.
    AppInstance = h;
  };

  virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

  //  void Show(int State);

  HWND GetHWND() const
  {
    return WindowHandle;
  };

  static HINSTANCE GetInstance()
  {
    return AppInstance;
  };

  Window *GetParent() const
  {
    return Parent;
  };

  HWND GetDlgItem(int id) const
  {
    return ::GetDlgItem(GetHWND (), id);
  };

  void PostMessageNow(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

  virtual bool OnMessageApp(UINT uMsg  __attribute__((unused)),
                            WPARAM wParam __attribute__((unused)),
                            LPARAM lParam __attribute__((unused)))
  {
    // Not processed by default.  Override in derived classes to
    // do something with app messages if you need to.
    return false;
  };

  virtual bool OnMessageCmd(int id __attribute__((unused)),
                            HWND hwndctl __attribute__((unused)),
                            UINT code __attribute__((unused)))
  {
    // Not processed by default.  Override in derived classes to
    // do something with command messages if you need to.
    return false;
  };

protected:
  static LRESULT CALLBACK FirstWindowProcReflector(HWND hwnd, UINT uMsg,
                                                   WPARAM wParam,
                                                   LPARAM lParam);

  void SetHWND(HWND h)
  {
    WindowHandle = h;
  };

  void setParent(Window *aParent)
  {
    Parent = aParent;
  };

private:
  static LRESULT CALLBACK WindowProcReflector(HWND hwnd, UINT uMsg,
                                              WPARAM wParam, LPARAM lParam);

  virtual bool registerWindowClass();

  static ATOM WindowClassAtom;
  static HINSTANCE AppInstance;

  HWND WindowHandle;   // Our Windows(tm) window handle.
  Window *Parent;
};

#endif /* WINDOW_H */
