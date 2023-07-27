add_gdal_driver(
  TARGET ogr_XODR
  SOURCES ogr_xodr.h 
          ogrxodrdatasource.cpp 
          ogrxodrlayer.cpp 
          ogrxodrdriver.cpp
  PLUGIN_CAPABLE)

gdal_standard_includes(ogr_XODR)

find_package(OpenDrive REQUIRED)

target_include_directories(ogr_XODR PUBLIC $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR> OpenDrive::OpenDrive)
gdal_target_link_libraries(ogr_XODR PRIVATE OpenDrive::OpenDrive)