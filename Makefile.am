bin_PROGRAMS = cygwin-crashreporter-cli cygwin-crashreporter-gui
noinst_LIBRARIES = libcrash_reporter.a libwizard.a

AM_LDFLAGS = -static-libgcc -static-libstdc++ -municode

libcrash_reporter_a_SOURCES = crash_reporter.cc crash_reporter.h

libwizard_a_SOURCES = window.cc window.h proppage.cc \
		      proppage.h propsheet.cc propsheet.h splash.cc\
		      splash.h notes.cc notes.h progress.cc \
		      progress.h done.cc done.h wizard.cc wizard.h \
		      res.rc resource.h manifest.xml hippo.ico cygwin.ico

cygwin_crashreporter_cli_SOURCES = main.cc
cygwin_crashreporter_gui_SOURCES = winmain.cc

cygwin_crashreporter_cli_LDADD = libcrash_reporter.a -lpsapi
cygwin_crashreporter_gui_LDADD = res.o libcrash_reporter.a libwizard.a -lgdi32 -lcomctl32 -lpsapi
cygwin_crashreporter_gui_LDFLAGS = $(AM_LDFLAGS) -mwindows

# files included in res.rc
res.o: res.rc resource.h manifest.xml hippo.ico cygwin.ico

.rc.o:
	$(AM_V_GEN)$(WINDRES) -o $@ $< --include-dir $(srcdir) -DPACKAGE_VERSION=\"@PACKAGE_VERSION@\" --use-temp-file

man1_MANS=cygwin-crashreporter.man cygwin-crashreporter-cli.1 cygwin-crashreporter-gui.1

BUILT_SOURCES = cygwin-crashreporter-cli.1 cygwin-crashreporter-gui.1

%.1:
	$(AM_V_GEN)echo .so man1/cygwin-crashreporter.1 >$@

CLEANFILES = $(BUILT_SOURCES)

# must be built as a native Windows application when distchecked
AM_DISTCHECK_CONFIGURE_FLAGS = "--host=i686-w64-mingw32"

EXTRA_DIST = update-version version
