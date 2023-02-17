include(FetchContent)
set(FETCHCONTENT_BASE_DIR openDrive)
FetchContent_Declare( openDRIVE
    GIT_REPOSITORY "https://github.com/gulsenbardk/libOpenDRIVE.git" #after merge request&acception, link should be change*
    GIT_TAG master
)

FetchContent_MakeAvailable(openDrive)


add_gdal_driver(
  TARGET ogr_XODR
  BUILTIN
  SOURCES ogr_xodr.h 
  ogrxodrdatasource.cpp 
  ogrxodrlayer.cpp 
  ogrxodrdriver.cpp 
  ogrxodrgeo.cpp)

gdal_standard_includes(ogr_XODR)

# TODO return to find_package
# TODO openDrive cannot find by main driver


target_include_directories(ogr_XODR PRIVATE $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR>)
if(GDAL_USE_OPENDRIVE)
  target_compile_definitions(ogr_XODR PRIVATE -DHAVE_OPENDRIVE)
  gdal_target_link_libraries(ogr_XODR PRIVATE openDrive)
  message(STATUS "Found OpenDRIVE")
else()
  message(WARNING "OpenDRIVE not found, please read the README.md")
endif()