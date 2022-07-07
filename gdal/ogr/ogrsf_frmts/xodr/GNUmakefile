include ../../../GDALmake.opt
include XODRmake.opt

OBJ = ogrxodrdriver.o ogrxodrdatasource.o ogrxodrlayer.o XODR.o MatrixTransformations2D.o CubicPolynomials.o CenterLaneSF.o  LaneOffsetCalculator.o LaneSectionProcessor.o LaneSectionSF.o LaneSF.o LanesSF.o ObjectProcessor.o ObjectSF.o PlanViewCalculator.o PlanViewGeometry.o RoadSF.o SignalProcessor.o SignalSF.o WidthLaneType.o BorderLaneType.o PlanViewGeometryFunctionArc.o PlanViewGeometryFunctionLine.o PlanViewGeometryFunctionParamPoly3.o PlanViewGeometryFunctionPoly3.o PlanViewGeometryFunctionSpiral.o

CPPFLAGS	:=	$(GDAL_INCLUDE) $(XSD_INCLUDE) $(ODRSPIRAL_INCLUDE) $(XODR_INCLUDE) $(CPPFLAGS)

default:	$(O_OBJ:.o=.$(OBJ_EXT))

clean:
	rm -f *.o *.lo $(O_OBJ) $(PLUGIN_SO)

$(O_OBJ):	ogr_xodr.h

PLUGIN_SO	= ogr_XODR.$(SO_EXT)

plugin:		$(PLUGIN_SO)

$(PLUGIN_SO):	$(OBJ)
		$(LD_SHARED) $(LNK_FLAGS) $(OBJ) $(CONFIG_LIBS_INS) $(LIBS) $(ODRSPIRAL_LIB) $(XODR_LIB) \
		-o $(PLUGIN_SO)

plugin-install: plugin
		$(INSTALL_LIB) $(PLUGIN_SO) $(INST_LIB)/gdalplugins
