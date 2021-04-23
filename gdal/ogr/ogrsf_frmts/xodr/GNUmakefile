include ../../../GDALmake.opt
include XODRmake.opt

OBJ = ogrxodrdriver.o ogrxodrdatasource.o ogrxodrlayer.o xodr.o MatrixTransformations2D.o CubicPolynomials.o CenterLaneSF.o  LaneOffsetCalculator.o LaneSectionProcessor.o LaneSectionSF.o LaneSF.o LanesSF.o objectprocessor.o objectsf.o ogr_xodr.o planviewcalculator.o planviewgeometry.o roadsf.o signalprocessor.o signalsf.o WidthLaneType.o BorderLaneType.o planviewgeometryfunctionarc.o planviewgeometryfunctionline.o planviewgeometryfunctionparampoly3.o planviewgeometryfunctionpoly3.o planviewgeometryfunctionspiral.o

CPPFLAGS	:=	$(GDAL_INCLUDE) $(XSD_INCLUDE) $(ODRSPIRAL_INCLUDE) $(XODR_INCLUDE) $(CPPFLAGS)

default:	$(O_OBJ:.o=.$(OBJ_EXT))

clean:
	rm -f *.o $(O_OBJ) $(PLUGIN_SO)

$(O_OBJ):	ogr_xodr.h

PLUGIN_SO	= ogr_XODR.$(SO_EXT)

plugin:		$(PLUGIN_SO)

$(PLUGIN_SO):	$(OBJ)
		$(LD_SHARED) $(LNK_FLAGS) $(OBJ) $(CONFIG_LIBS_INS) $(LIBS) $(ODRSPIRAL_LIB) $(XODR_LIB) \
		-o $(PLUGIN_SO)

plugin-install: plugin
		$(INSTALL_LIB) $(PLUGIN_SO) $(INST_LIB)/gdalplugins
