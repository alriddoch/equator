INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = equator

if BUILD_STATIC
equator_LDFLAGS = -nodefaultlibs

EQUATOR_EXT_LIBS = \
        /usr/local/lib/libgtkglextmm-x11-1.1.a \
        /usr/local/lib/libgdkglextmm-x11-1.1.a \
        /usr/lib/libgtkglext-x11-1.0.a \
        /usr/lib/libgdkglext-x11-1.0.a \
        /usr/lib/libgtkmm-2.4.a \
        /usr/lib/libatkmm-1.6.a \
        /usr/lib/libpangomm-1.4.a \
        /usr/lib/libgdkmm-2.4.a \
        /usr/lib/libglibmm-2.4.a \
        /usr/lib/libSDL_image.a \
        /usr/lib/libtiff.a \
        /usr/lib/libjpeg.a \
        /usr/lib/libpng12.a \
        /usr/lib/libz.a \
        /usr/lib/libSDL_mixer.a \
        /usr/lib/libvorbisfile.a \
        /usr/lib/libvorbis.a \
        /usr/lib/libogg.a \
        /usr/lib/libSDL.a \
        /usr/lib/libasound.a \
        /usr/lib/libmercator-0.2.a \
        /usr/lib/libvarconf-1.0.a \
        /usr/local/lib/liberis-1.3.a \
        /usr/lib/libsigc-2.0.a \
        /usr/lib/libskstream-0.3.a \
        /usr/lib/libAtlasObjects-0.6.a \
        /usr/lib/libAtlasNet-0.6.a \
        /usr/lib/libAtlasCodecs-0.6.a \
        /usr/lib/libAtlasMessage-0.6.a \
        /usr/lib/libAtlas-0.6.a \
        /usr/lib/libwfmath-0.3.a \
        /usr/local/lib/libsage.a \
        /usr/lib/lib3ds.a \
        /usr/local/lib/libcal3d.a \
        /usr/X11R6/lib/libGLU.a \
        $(STATIC_LIBSTDCPP) \
        $(STATIC_LIBGCC) \
        /usr/lib/libgtk-x11-2.0.so \
        /usr/lib/libGL.so \
        /usr/X11R6/lib/libXmu.so \
        -lc -lm -lpthread -lgcc_s

else
EQUATOR_EXT_LIBS = $(EQUATOR_LIBS)
endif

equator_SOURCES = MainWindow.cpp MainWindow.h \
		  ViewWindow.cpp ViewWindow.h \
		  LayerWindow.cpp LayerWindow.h \
		  InheritanceWindow.cpp InheritanceWindow.h \
		  ServerWindow.cpp ServerWindow.h \
		  NewLayerWindow.cpp NewLayerWindow.h \
		  ConnectWindow.cpp ConnectWindow.h \
		  LoginWindow.cpp LoginWindow.h \
		  CharacterWindow.cpp CharacterWindow.h \
		  Palette.cpp Palette.h \
		  AtlasMapWidget.cpp AtlasMapWidget.h \
		  GlView.cpp GlView.h \
		  Model.cpp Model.h \
		  WorldEntity.cpp WorldEntity.h \
		  Server.cpp Server.h \
		  Layer.cpp Layer.h \
		  Holo.cpp Holo.h \
		  Terrain.cpp Terrain.h \
		  EntityLayer.cpp EntityLayer.h \
		  ServerEntities.cpp ServerEntities.h \
		  GameView.cpp GameView.h \
		  SearView.cpp SearView.h \
		  Cal3dStore.cpp Cal3dStore.h \
		  types.h GL.h operations.h \
		  arrow_mesh.h \
		  equator_plugin.cpp

equator_LDADD = $(top_builddir)/visual/libvisual.a \
                $(top_builddir)/common/libcommon.a \
                $(top_builddir)/gui/gtkmm/libguigtkmm.a \
                $(EQUATOR_EXT_LIBS)
