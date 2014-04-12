CXX=i686-w64-mingw32-g++

all:
	$(CXX) -g -O0 -Wall -Wextra -o cygwin_crash_reporter.exe main.cpp -lbreakpad_client -lwininet -static-libgcc -static-libstdc++
