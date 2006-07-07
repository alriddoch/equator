noinst_LIBRARIES = libsearsrc.a

INCLUDES = -I$(top_srcdir)/sear

sear_srcdir = $(top_srcdir)/../sear

Console.h:
	$(LN_S) -f $(sear_srcdir)/src/Console.h

System.h:
	$(LN_S) -f $(sear_srcdir)/src/System.h

WorldEntity.h:
	$(LN_S) -f $(sear_srcdir)/src/WorldEntity.h

FileHandler.h:
	$(LN_S) -f $(sear_srcdir)/src/FileHandler.h

Calendar.h:
	$(LN_S) -f $(sear_srcdir)/src/Calendar.h

SrcHeaders.cpp: Console.h System.h WorldEntity.h FileHandler.h Calendar.h
	touch SrcHeaders.cpp

libsearsrc_a_SOURCES = \
                       SrcHeaders.cpp
