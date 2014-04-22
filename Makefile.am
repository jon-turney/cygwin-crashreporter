bin_PROGRAMS = cygwin-crashreporter-cli cygwin-crashreporter-gui
noinst_LIBRARIES = libcrash_reporter.a

AM_LDFLAGS = -static-libgcc -static-libstdc++ -municode

libcrash_reporter_a_SOURCES = crash_reporter.cc crash_reporter.h

cygwin_crashreporter_cli_SOURCES = main.cc

cygwin_crashreporter_gui_SOURCES = winmain.cc window.cc window.h proppage.cc \
				  proppage.h propsheet.cc propsheet.h splash.cc\
				  splash.h notes.cc notes.h progress.cc \
				  progress.h done.cc done.h res.rc resource.h \
				  cygwin-crashreporter-gui.exe.manifest \
				  cygwin.ico hippo.ico

cygwin_crashreporter_cli_LDADD = libcrash_reporter.a -lbreakpad_client -lwininet
cygwin_crashreporter_gui_LDADD = libcrash_reporter.a -lbreakpad_client -lwininet -lgdi32 -lcomctl32
cygwin_crashreporter_gui_LDFLAGS = $(AM_LDFLAGS) -mwindows

# files included in res.rc
res.o: cygwin-crashreporter-gui.exe.manifest hippo.ico cygwin.ico

.rc.o:
	$(AM_V_GEN)$(WINDRES) --include-dir $(srcdir) -o $@ $<

man1_MANS=cygwin-crashreporter.man cygwin-crashreporter-cli.1 cygwin-crashreporter-gui.1

BUILT_SOURCES = cygwin-crashreporter-cli.1 cygwin-crashreporter-gui.1

%.1:
	$(AM_GEN_V)echo .so man1/cygwin-crashreporter.1 >$@

CLEANFILES = $(BUILT_SOURCES)

# must be built as a native Windows application when distchecked
AM_DISTCHECK_CONFIGURE_FLAGS = "--host=i686-w64-mingw32"

EXTRA_DIST = update-version version
