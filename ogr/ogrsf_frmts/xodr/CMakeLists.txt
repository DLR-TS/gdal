
add_gdal_driver(
  TARGET ogr_XODR
  BUILTIN
  SOURCES ogr_xodr.h 
  ogrxodrdatasource.cpp 
  ogrxodrlayer.cpp 
  ogrxodrdriver.cpp 
  ogrxodrgeo.cpp)

gdal_standard_includes(ogr_XODR)

find_package(OpenDrive REQUIRED)
include_directories(${OpenDrive_INCLUDE_DIRS})

target_include_directories(ogr_XODR PRIVATE $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR> ${OpenDrive_INCLUDE_DIRS})
target_link_libraries(ogr_XODR ${OpenDrive_LIBRARIES})