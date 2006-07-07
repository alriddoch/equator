noinst_LIBRARIES = libsearrenderers.a

INCLUDES = -I$(top_srcdir)/sear

sear_srcdir = $(top_srcdir)/../sear

Render.h:
	$(LN_S) -f $(sear_srcdir)/renderers/Render.h

RenderSystem.h:
	$(LN_S) -f $(sear_srcdir)/renderers/RenderSystem.h

RenderTypes.h:
	$(LN_S) -f $(sear_srcdir)/renderers/RenderTypes.h

Graphics.h:
	$(LN_S) -f $(sear_srcdir)/renderers/Graphics.h

Light.h:
	$(LN_S) -f $(sear_srcdir)/renderers/Light.h

StateManager.h:
	$(LN_S) -f $(sear_srcdir)/renderers/StateManager.h

RenderersHeaders.cpp: Render.h RenderSystem.h RenderTypes.h Graphics.h \
                      Light.h StateManager.h
	touch RenderersHeaders.cpp

libsearrenderers_a_SOURCES = \
                             RenderersHeaders.cpp
