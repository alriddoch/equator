noinst_LIBRARIES = libvisual.a

INCLUDES = -I$(top_srcdir)

apogee_srcdir = $(top_srcdir)/../apogee

GL.h GLU.h:
	$(LN_S) $(apogee_srcdir)/visual/GL.h $(apogee_srcdir)/visual/GLU.h .

default_texture.h:
	$(LN_S) $(apogee_srcdir)/visual/default_texture.h .

Texture.cpp Texture.h: GL.h default_texture.h
	$(LN_S) $(apogee_srcdir)/visual/Texture.cpp $(apogee_srcdir)/visual/Texture.h .

Model.cpp Model.h:
	$(LN_S) $(apogee_srcdir)/visual/Model.cpp $(apogee_srcdir)/visual/Model.h .

EntityRenderer.cpp EntityRenderer.h:
	$(LN_S) $(apogee_srcdir)/visual/EntityRenderer.cpp $(apogee_srcdir)/visual/EntityRenderer.h .

3dsRenderer.cpp 3dsRenderer.h:
	$(LN_S) $(apogee_srcdir)/visual/3dsRenderer.cpp $(apogee_srcdir)/visual/3dsRenderer.h .

Cal3dRenderer.cpp Cal3dRenderer.h:
	$(LN_S) $(apogee_srcdir)/visual/Cal3dRenderer.cpp $(apogee_srcdir)/visual/Cal3dRenderer.h .

TerrainRenderer.cpp TerrainRenderer.h:
	$(LN_S) $(apogee_srcdir)/visual/TerrainRenderer.cpp $(apogee_srcdir)/visual/TerrainRenderer.h .

ForestRenderer.cpp ForestRenderer.h:
	$(LN_S) $(apogee_srcdir)/visual/ForestRenderer.cpp $(apogee_srcdir)/visual/ForestRenderer.h .

libvisual_a_SOURCES = GL.h GLU.h default_texture.h \
                      Texture.cpp Texture.h \
                      Renderer.cpp Renderer.h \
                      Model.cpp Model.h \
                      EntityRenderer.cpp EntityRenderer.h \
                      3dsRenderer.cpp 3dsRenderer.h \
                      Cal3dRenderer.cpp Cal3dRenderer.h \
                      TerrainRenderer.cpp TerrainRenderer.h \
                      ForestRenderer.cpp ForestRenderer.h
