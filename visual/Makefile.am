noinst_LIBRARIES = libvisual.a

INCLUDES = -I$(top_srcdir)

#3dsRenderer.cpp 3dsRenderer.h: $(top_srcdir)/../apogee/visual/3dsRenderer.cpp $(top_srcdir)/../apogee/visual/3dsRenderer.h
#	$(LN_S) $(top_srcdir)/../apogee/visual/3dsRenderer.cpp 3dsRenderer.cpp
#	$(LN_S) $(top_srcdir)/../apogee/visual/3dsRenderer.h 3dsRenderer.h

libvisual_a_SOURCES = GL.h GLU.h \
                      Texture.cpp Texture.h \
                      Renderer.cpp Renderer.h \
                      Model.cpp Model.h \
                      EntityRenderer.cpp EntityRenderer.h \
                      3dsRenderer.cpp 3dsRenderer.h \
                      Cal3dRenderer.cpp Cal3dRenderer.h \
                      TerrainRenderer.cpp TerrainRenderer.h
