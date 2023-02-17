#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "GDAL::GDAL" for configuration "Debug"
set_property(TARGET GDAL::GDAL APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(GDAL::GDAL PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "GEOS::geos_c"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libgdal.so.31.0.0"
  IMPORTED_SONAME_DEBUG "libgdal.so.31"
  )

list(APPEND _IMPORT_CHECK_TARGETS GDAL::GDAL )
list(APPEND _IMPORT_CHECK_FILES_FOR_GDAL::GDAL "${_IMPORT_PREFIX}/lib/libgdal.so.31.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
