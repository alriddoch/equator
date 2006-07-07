noinst_LIBRARIES = libsearinterfaces.a

sear_srcdir = $(top_srcdir)/../sear

ConsoleObject.h:
	$(LN_S) -f $(sear_srcdir)/interfaces/ConsoleObject.h

InterfacesHeaders.cpp: ConsoleObject.h
	touch InterfacesHeaders.cpp

libsearinterfaces_a_SOURCES = InterfacesHeaders.cpp \
                              ConsoleObject.h
