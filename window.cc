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

// This is the implementation of the Window class.  It serves both as a window
// class in its own right and as a base class for other window-like classes
// (e.g. PropertyPage, PropSheet).

#include "window.h"
#include "resource.h"

#include <windows.h>
#include <assert.h>

ATOM Window::WindowClassAtom = 0;
HINSTANCE Window::AppInstance = NULL;

#define WINDOW_CLASS_NAME "MainWindowClass"

Window::Window ()
{
  WindowHandle = NULL;
  Parent = NULL;
}

Window::~Window()
{
  // FIXME: Maybe do some reference counting and do this Unregister
  // when there are no more of us left.  Not real critical unless
  // we're in a DLL which we're not right now.
  //UnregisterClass(WindowClassAtom, InstanceHandle);
}

LRESULT CALLBACK
Window::FirstWindowProcReflector(HWND hwnd, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam)
{
  Window *wnd = NULL;

  if(uMsg == WM_NCCREATE)
    {
      // This is the first message a window gets (so MSDN says anyway).
      // Take this opportunity to "link" the HWND to the 'this' ptr, steering
      // messages to the class instance's WindowProc().
      wnd = reinterpret_cast<Window *>(((LPCREATESTRUCT)lParam)->lpCreateParams);

      // Set a backreference to this class instance in the HWND.
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));

      // Set a new WindowProc now that we have the peliminaries done.
      // We could instead simply do the contents of Window::WindowProcReflector
      // in the 'else' clause below, but this way we eliminate an unnecessary 'if/else' on
      // every message.  Yeah, it's probably not worth the trouble.
      SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) &Window::WindowProcReflector);

      // Finally, store the window handle in the class.
      wnd->WindowHandle = hwnd;
    }
  else
    {
      // Should never get here.
      assert(0);
    }

  return wnd->WindowProc(uMsg, wParam, lParam);
}

LRESULT CALLBACK
Window::WindowProcReflector(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam)
{
  Window *This;

  // Get our this pointer
  This = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  return This->WindowProc(uMsg, wParam, lParam);
}

bool
Window::Create(Window * parent, DWORD Style)
{
  // First register the window class, if we haven't already
  if (registerWindowClass() == false)
    {
      // Registration failed
      return false;
    }

  // Save our parent, we'll probably need it eventually.
  Parent = parent;

  // Create the window instance
  WindowHandle = CreateWindowEx (
                   0,                   // Extended Style
                   WINDOW_CLASS_NAME,   // Window class name
                   "",                  // No title-bar string yet
                   Style,               // Style bits
                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Default positions and size
                   parent == NULL ? (HWND) NULL : parent->GetHWND (),          // Parent Window
                   (HMENU) NULL,        // Use class menu
                   GetInstance (),      // The application instance
                   (LPVOID) this);      // The this ptr, which we'll use to set up the WindowProc reflection.

  if (WindowHandle == NULL)
    {
      // Failed
      return false;
    }

  return true;
}

bool
Window::registerWindowClass()
{
  if (WindowClassAtom == 0)
    {
      // We're not registered yet
      WNDCLASSEX wc;

      wc.cbSize = sizeof (wc);
      wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;   // Some sensible style defaults
      wc.lpfnWndProc = Window::FirstWindowProcReflector; // Our default window procedure.  This replaces itself
                                                         // on the first call with the simpler Window::WindowProcReflector().
      wc.cbClsExtra = 0;                                 // No class bytes
      wc.cbWndExtra = 0; // XXX: 4;       // One pointer to REFLECTION_INFO in the extra window instance bytes
      wc.hInstance = GetInstance();                      // The app instance
      wc.hIcon = NULL;                                   // Use a bunch of system defaults for the GUI elements
      wc.hIconSm = NULL;
      wc.hCursor = NULL;
      wc.hbrBackground = (HBRUSH) (COLOR_BACKGROUND + 1);
      wc.lpszMenuName = NULL;                            // No menu
      wc.lpszClassName = WINDOW_CLASS_NAME;              // The class name

      // All set, try to register
      WindowClassAtom = RegisterClassEx(&wc);

      if (WindowClassAtom == 0)
        {
          // Failed
          return false;
        }
    }

  // We're registered, or already were before the call,
  // return success in either case.
  return true;
}

LRESULT
Window::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(WindowHandle, uMsg, wParam, lParam);
}

void
Window::PostMessageNow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  ::PostMessage(GetHWND(), uMsg, wParam, lParam);
}
