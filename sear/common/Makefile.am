noinst_LIBRARIES = libsearcommon.a

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/sear

sear_srcdir = $(top_srcdir)/../sear

Log.cpp Log.h:
	$(LN_S) -f $(sear_srcdir)/common/Log.cpp $(sear_srcdir)/common/Log.h .

Matrix.h:
	$(LN_S) -f $(sear_srcdir)/common/Matrix.h .

Mesh.h:
	$(LN_S) -f $(sear_srcdir)/common/Mesh.h .

SPtr.h:
	$(LN_S) -f $(sear_srcdir)/common/SPtr.h .

types.h:
	$(LN_S) -f $(sear_srcdir)/common/types.h .

Utility.cpp Utility.h:
	$(LN_S) -f $(sear_srcdir)/common/Utility.cpp $(sear_srcdir)/common/Utility.h .

CommonHeaders.cpp: Log.h Matrix.h Mesh.h SPtr.h types.h Utility.h
	touch CommonHeaders.cpp

libsearcommon_a_SOURCES = CommonHeaders.cpp \
                          Log.cpp Utility.cpp \
                          Log.h Matrix.h Mesh.h SPtr.h types.h Utility.h
