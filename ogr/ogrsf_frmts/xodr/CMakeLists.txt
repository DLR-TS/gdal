
add_gdal_driver(
  TARGET ogr_XODR
  BUILTIN
  SOURCES ogr_xodr.h 
  ogrxodrdatasource.cpp 
  ogrxodrlayer.cpp 
  ogrxodrdriver.cpp 
  ogrxodrgeo.cpp
  #PLUGIN_CAPABLE
  BUILTIN)

gdal_standard_includes(ogr_XODR)

find_package(OpenDrive REQUIRED)
include_directories(${OpenDrive_INCLUDE_DIRS})

target_include_directories(ogr_XODR PUBLIC $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR> ${OpenDrive_INCLUDE_DIRS})
if(GDAL_USE_OPENDRIVE_INTERNAL)
  gdal_add_vendored_lib(ogr_XODR libOpenDrive)
else()
  gdal_target_link_libraries(ogr_XODR PRIVATE ${OpenDrive_LIBRARIES})
endif()