bin_PROGRAMS=cygwin_crash_reporter_cli cygwin_crash_reporter_gui

AM_LDFLAGS=-static-libgcc -static-libstdc++ -municode

cygwin_crash_reporter_cli_SOURCES=main.cpp
cygwin_crash_reporter_gui_SOURCES=winmain.cc window.cc proppage.cc propsheet.cc \
				  splash.cc notes.cc progress.cc done.cc res.rc \
				  crash_reporter.cc

cygwin_crash_reporter_cli_LDADD=-lbreakpad_client -lwininet
cygwin_crash_reporter_gui_LDADD=-lbreakpad_client -lwininet -lgdi32 -lcomctl32
cygwin_crash_reporter_gui_LDFLAGS=$(AM_LDFLAGS) -mwindows

# manifest is included in res.rc
res.o: cygwin_crash_reporter.exe.manifest

.rc.o:
	$(AM_V_GEN)$(WINDRES) --include-dir $(srcdir) -o $@ $<
