#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "GDAL::GDAL" for configuration ""
set_property(TARGET GDAL::GDAL APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(GDAL::GDAL PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NOCONFIG "GEOS::geos_c;expat::expat"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libgdal.so.31.0.0"
  IMPORTED_SONAME_NOCONFIG "libgdal.so.31"
  )

list(APPEND _IMPORT_CHECK_TARGETS GDAL::GDAL )
list(APPEND _IMPORT_CHECK_FILES_FOR_GDAL::GDAL "${_IMPORT_PREFIX}/lib/libgdal.so.31.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
