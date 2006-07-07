noinst_LIBRARIES = libsearenvironment.a

INCLUDES = -I$(top_srcdir)/sear

sear_srcdir = $(top_srcdir)/../sear

Environment.cpp Environment.h:
	$(LN_S) -f $(sear_srcdir)/environment/Environment.cpp $(sear_srcdir)/environment/Environment.h .

SkyDome.cpp SkyDome.h:
	$(LN_S) -f $(sear_srcdir)/environment/SkyDome.cpp $(sear_srcdir)/environment/SkyDome.h .

Stars.cpp Stars.h:
	$(LN_S) -f $(sear_srcdir)/environment/Stars.cpp $(sear_srcdir)/environment/Stars.h .

TerrainRenderer.cpp TerrainRenderer.h:
	$(LN_S) -f $(sear_srcdir)/environment/TerrainRenderer.cpp $(sear_srcdir)/environment/TerrainRenderer.h .

Weather.cpp Weather.h:
	$(LN_S) -f $(sear_srcdir)/environment/Weather.cpp $(sear_srcdir)/environment/Weather.h .

EnvironmentHeaders.cpp: 
	touch EnvironmentHeaders.cpp

libsearenvironment_a_SOURCES = EnvironmentHeaders.cpp \
                               SkyDome.cpp \
                               Stars.cpp \
                               TerrainRenderer.cpp \
                               Weather.cpp \
                               Environment.cpp
