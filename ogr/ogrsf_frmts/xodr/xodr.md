# xodr driver 
## CMakeLists.txt 
add_gdal_driver for define target, target purpose and source files for it. 
reference: ogr_sqlite


```
target_include_directories(ogr_XODR PRIVATE $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR>)
if(GDAL_USE_OPENDRIVE)
  target_compile_definitions(ogr_XODR PRIVATE -DHAVE_OPENDRIVE)
  gdal_target_link_libraries(ogr_XODR PRIVATE openDrive)
  message(STATUS "Found OpenDRIVE")
else()
  message(WARNING "OpenDRIVE not found, please read the README.md")
endif()

```
# Git errors*