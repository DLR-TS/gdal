add_gdal_driver(
  TARGET ogr_XODR
  SOURCES ogr_xodr.h 
          ogrxodrdriver.cpp
          ogrxodrdatasource.cpp 
          ogrxodrlayer.cpp
          ogrxodrlayerlane.cpp 
          ogrxodrlayerlaneborder.cpp 
          ogrxodrlayerreferenceline.cpp
          ogrxodrlayerroadmark.cpp
          ogrxodrlayerroadobject.cpp
  PLUGIN_CAPABLE)

gdal_standard_includes(ogr_XODR)
target_include_directories(ogr_XODR PUBLIC $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR> OpenDrive::OpenDrive)
gdal_target_link_libraries(ogr_XODR PRIVATE OpenDrive::OpenDrive)