file(REMOVE_RECURSE
  "CMakeFiles/python_binding"
  "extensions/gdal_wrap.cpp"
  "extensions/gdalconst_wrap.c"
  "extensions/gnm_wrap.cpp"
  "extensions/ogr_wrap.cpp"
  "extensions/osr_wrap.cpp"
  "osgeo/__init__.py"
  "osgeo/_gdal.cpython-310-x86_64-linux-gnu.so"
  "osgeo/_gdalconst.cpython-310-x86_64-linux-gnu.so"
  "osgeo/_gnm.cpython-310-x86_64-linux-gnu.so"
  "osgeo/_ogr.cpython-310-x86_64-linux-gnu.so"
  "osgeo/_osr.cpython-310-x86_64-linux-gnu.so"
  "osgeo/gdal.py"
  "osgeo/gdalconst.py"
  "osgeo/gdalnumeric.py"
  "osgeo/gnm.py"
  "osgeo/ogr.py"
  "osgeo/osr.py"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/python_binding.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
