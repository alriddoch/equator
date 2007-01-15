SUBDIRS = cal3d

noinst_LIBRARIES = libsearloaders.a

INCLUDES = -I$(top_srcdir)/sear

sear_srcdir = $(top_srcdir)/../sear

3ds.cpp 3ds.h:
	$(LN_S) -f $(sear_srcdir)/loaders/3ds.cpp $(sear_srcdir)/loaders/3ds.h .

3ds_Loader.cpp 3ds_Loader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/3ds_Loader.cpp $(sear_srcdir)/loaders/3ds_Loader.h .

AreaModel.cpp AreaModel.h:
	$(LN_S) -f $(sear_srcdir)/loaders/AreaModel.cpp $(sear_srcdir)/loaders/AreaModel.h .

AreaModelLoader.cpp AreaModelLoader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/AreaModelLoader.cpp $(sear_srcdir)/loaders/AreaModelLoader.h .

BoundBox.cpp BoundBox.h:
	$(LN_S) -f $(sear_srcdir)/loaders/BoundBox.cpp $(sear_srcdir)/loaders/BoundBox.h .

BoundBox_Loader.cpp BoundBox_Loader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/BoundBox_Loader.cpp $(sear_srcdir)/loaders/BoundBox_Loader.h .

DynamicObject.cpp DynamicObject.h:
	$(LN_S) -f $(sear_srcdir)/loaders/DynamicObject.cpp $(sear_srcdir)/loaders/DynamicObject.h .

LibModelFile.cpp LibModelFile.h:
	$(LN_S) -f $(sear_srcdir)/loaders/LibModelFile.cpp $(sear_srcdir)/loaders/LibModelFile.h .

LibModelFile_Loader.cpp LibModelFile_Loader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/LibModelFile_Loader.cpp $(sear_srcdir)/loaders/LibModelFile_Loader.h .

ModelHandler.cpp Model.h NullModel.h ModelHandler.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ModelHandler.cpp $(sear_srcdir)/loaders/Model.h $(sear_srcdir)/loaders/NullModel.h $(sear_srcdir)/loaders/ModelHandler.h .

ModelLoader.cpp ModelLoader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ModelLoader.cpp $(sear_srcdir)/loaders/ModelLoader.h .

ModelRecord.cpp ModelRecord.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ModelRecord.cpp $(sear_srcdir)/loaders/ModelRecord.h .

ModelSystem.cpp ModelSystem.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ModelSystem.cpp $(sear_srcdir)/loaders/ModelSystem.h .

NPlane.cpp NPlane.h:
	$(LN_S) -f $(sear_srcdir)/loaders/NPlane.cpp $(sear_srcdir)/loaders/NPlane.h .

NPlane_Loader.cpp NPlane_Loader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/NPlane_Loader.cpp $(sear_srcdir)/loaders/NPlane_Loader.h .

ObjectHandler.cpp ObjectHandler.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ObjectHandler.cpp $(sear_srcdir)/loaders/ObjectHandler.h .

ObjectRecord.cpp ObjectRecord.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ObjectRecord.cpp $(sear_srcdir)/loaders/ObjectRecord.h .

ParticleSystem.cpp ParticleSystem.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ParticleSystem.cpp $(sear_srcdir)/loaders/ParticleSystem.h .

ParticleSystemLoader.cpp ParticleSystemLoader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/ParticleSystemLoader.cpp $(sear_srcdir)/loaders/ParticleSystemLoader.h .

StaticObject.cpp StaticObject.h:
	$(LN_S) -f $(sear_srcdir)/loaders/StaticObject.cpp $(sear_srcdir)/loaders/StaticObject.h .

WireFrame.cpp WireFrame.h:
	$(LN_S) -f $(sear_srcdir)/loaders/WireFrame.cpp $(sear_srcdir)/loaders/WireFrame.h .

WireFrame_Loader.cpp WireFrame_Loader.h:
	$(LN_S) -f $(sear_srcdir)/loaders/WireFrame_Loader.cpp $(sear_srcdir)/loaders/WireFrame_Loader.h .

LoadersHeaders.cpp: 3ds.h \
                    3ds_Loader.h \
                    AreaModel.h \
                    AreaModelLoader.h \
                    BoundBox.h \
                    BoundBox_Loader.h \
                    DynamicObject.h \
                    LibModelFile.h \
                    LibModelFile_Loader.h \
                    Model.h \
                    ModelHandler.h \
                    ModelLoader.h \
                    ModelRecord.h \
                    ModelSystem.h \
                    NPlane.h \
                    NPlane_Loader.h \
                    NullModel.h \
                    ObjectHandler.h \
                    ObjectRecord.h \
                    ParticleSystem.h \
                    ParticleSystemLoader.h \
                    StaticObject.h \
                    WireFrame.h \
                    WireFrame_Loader.h
	touch LoadersHeaders.cpp

libsearloaders_a_SOURCES = LoadersHeaders.cpp \
                           3ds.cpp \
                           3ds_Loader.cpp \
                           AreaModel.cpp \
                           AreaModelLoader.cpp \
                           BoundBox.cpp \
                           BoundBox_Loader.cpp \
                           DynamicObject.cpp \
                           LibModelFile.cpp \
                           LibModelFile_Loader.cpp \
                           ModelHandler.cpp \
                           ModelLoader.cpp \
                           ModelRecord.cpp \
                           ModelSystem.cpp \
                           NPlane.cpp \
                           NPlane_Loader.cpp \
                           ObjectHandler.cpp \
                           ObjectRecord.cpp \
                           ParticleSystem.cpp \
                           ParticleSystemLoader.cpp \
                           StaticObject.cpp \
                           WireFrame.cpp \
                           WireFrame_Loader.cpp \
                           3ds.h \
                           3ds_Loader.h \
                           AreaModel.h \
                           AreaModelLoader.h \
                           BoundBox.h \
                           BoundBox_Loader.h \
                           DynamicObject.h \
                           LibModelFile.h \
                           LibModelFile_Loader.h \
                           Model.h \
                           ModelHandler.h \
                           ModelLoader.h \
                           ModelRecord.h \
                           ModelSystem.h \
                           NPlane.h \
                           NPlane_Loader.h \
                           NullModel.h \
                           ObjectHandler.h \
                           ObjectRecord.h \
                           ParticleSystem.h \
                           ParticleSystemLoader.h \
                           StaticObject.h \
                           WireFrame.h \
                           WireFrame_Loader.h
