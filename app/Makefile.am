INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = equator

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
		  Cal3dStore.cpp Cal3dStore.h \
		  types.h GL.h operations.h \
		  arrow_mesh.h \
		  equator_plugin.cpp

equator_LDADD = $(top_builddir)/visual/libvisual.a \
                $(top_builddir)/common/libcommon.a \
                $(top_builddir)/gui/gtkmm/libguigtkmm.a
