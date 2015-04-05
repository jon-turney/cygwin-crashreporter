# cygwin-crashreporter

[![Build status](https://ci.appveyor.com/api/projects/status/be72tif4nk9tndbp?svg=true)](https://ci.appveyor.com/project/jon-turney/cygwin-crashreporter)

## A simple breakpad crashreporter for Cygwin.

By design, this is not a Cygwin application.  If there is a failure mode which 
affects all Cygwin applications on a particular host, we still want to be able 
to report the crash.

This can be invoked using Cygwin's error_start JIT facility. (e.g. 
CYGWIN='"error_start=C:\cygwin\bin\cygwin-crashreporter-gui -r -k"')

Or, it could be invoked from a signal handler for SIGSEGV, SIGABORT etc.

Invoking the crashreporter like that isn't quite ideal as we don't capture the 
EXCEPTION_POINTERS exception record into the minidump, so the minidump doesn't 
record exactly which thread crashed, and with what exception.

Addressing these shortcomings required a couple of changes in the Cygwin DLL to 
give the crash reporter the information it needs to construct the 
EXCEPTION_POINTERS:

- Providing a ucontext_t parameter to SA_SIGINFO signal handlers, giving access
  to the CONTEXT

- A cygwin_internal() operation to retrieve the EXCEPTION_RECORD from a
  siginfo_t *

The cygwin-crashreporter-hooks DLL is designed to be optionally dynamically 
loaded by a cygwin application (or perhaps one day, the cygwin DLL), and 
contains functions to be invoked at process creation (to ensure a crash 
generation server is running), and at exception time (to request a crashdump)
