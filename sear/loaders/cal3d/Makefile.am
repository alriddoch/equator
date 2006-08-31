noinst_LIBRARIES = libsearloaderscal3d.a

INCLUDES = -I$(top_srcdir)/sear -I$(top_srcdir)/sear/loaders

sear_srcdir = $(top_srcdir)/../sear

CoreModelHandler.cpp CoreModelHandler.h:
	$(LN_S) -f $(sear_srcdir)/loaders/cal3d/CoreModelHandler.cpp $(sear_srcdir)/loaders/cal3d/CoreModelHandler.h .

Cal3dCoreModel.cpp Cal3dCoreModel.h:
	$(LN_S) -f $(sear_srcdir)/loaders/cal3d/Cal3dCoreModel.cpp $(sear_srcdir)/loaders/cal3d/Cal3dCoreModel.h .

Cal3dModel.cpp Cal3dModel.h: Cal3dCoreModel.h
	$(LN_S) -f $(sear_srcdir)/loaders/cal3d/Cal3dModel.cpp $(sear_srcdir)/loaders/cal3d/Cal3dModel.h .

Cal3d_Loader.cpp Cal3d_Loader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/cal3d/Cal3d_Loader.cpp $(sear_srcdir)/loaders/cal3d/Cal3d_Loader.h .

libsearloaderscal3d_a_SOURCES = \
                                Cal3dModel.cpp \
                                Cal3dCoreModel.cpp \
                                CoreModelHandler.cpp \
                                Cal3d_Loader.cpp
