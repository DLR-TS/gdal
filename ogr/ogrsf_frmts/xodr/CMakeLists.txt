add_gdal_driver(
  TARGET ogr_XODR
  BUILTIN
  SOURCES ogr_xodr.h 
  ogrxodrdatasource.cpp 
  ogrxodrlayer.cpp 
  ogrxodrdriver.cpp)

gdal_standard_includes(ogr_XODR)


find_package(OpenDrive REQUIRED)
#include_directories(${OpenDrive_INCLUDE_DIRS})

target_include_directories(ogr_XODR PUBLIC $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR> OpenDrive::OpenDrive)
#if(GDAL_USE_OPENDRIVE_INTERNAL)
#  gdal_add_vendored_lib(ogr_XODR PRIVATE OpenDrive::OpenDrive)
#else()
#  gdal_target_link_libraries(ogr_XODR PRIVATE OpenDrive::OpenDrive)
#endif()
gdal_target_link_libraries(ogr_XODR PRIVATE OpenDrive::OpenDrive)


#include(FetchContent)
#set(FETCHCONTENT_BASE_DIR libopendrive)
#FetchContent_Declare(
#    openDRIVE
#    GIT_REPOSITORY "https://github.com/gulsenbardk/libOpenDRIVE.git" #after merge request&acception, link should be change*
#    GIT_TAG master
#)
#
#FetchContent_MakeAvailable(openDrive)
#target_link_libraries(ogr_XODR PUBLIC OpenDrive)