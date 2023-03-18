# Install script for directory: /home/gdal

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(REMOVE "/home/gdal/build/install_manifest_extra.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/ogr/ogrsf_frmts/geojson/libjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/port/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/frmts/jpeg/libjpeg12/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/third_party/LercLib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/frmts/gtiff/libgeotiff/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/frmts/gif/giflib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/frmts/png/libpng/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/alg/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/ogr/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/gnm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/frmts/iso8211/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/frmts/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/ogr/ogrsf_frmts/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/gcore/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/swig/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/apps/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/scripts/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/doc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/man/cmake_install.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gdalplugins" TYPE FILE FILES "/home/gdal/frmts/drivers.ini")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so.31.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so.31"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/gdal/build/libgdal.so.31.0.0"
    "/home/gdal/build/libgdal.so.31"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so.31.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so.31"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/usr/local/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/gdal/build/libgdal.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so"
         OLD_RPATH "/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgdal.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/gdal/port/cpl_atomic_ops.h"
    "/home/gdal/port/cpl_auto_close.h"
    "/home/gdal/port/cpl_compressor.h"
    "/home/gdal/port/cpl_config_extras.h"
    "/home/gdal/port/cpl_conv.h"
    "/home/gdal/port/cpl_csv.h"
    "/home/gdal/port/cpl_error.h"
    "/home/gdal/port/cpl_hash_set.h"
    "/home/gdal/port/cpl_http.h"
    "/home/gdal/port/cpl_json.h"
    "/home/gdal/port/cplkeywordparser.h"
    "/home/gdal/port/cpl_list.h"
    "/home/gdal/port/cpl_minixml.h"
    "/home/gdal/port/cpl_multiproc.h"
    "/home/gdal/port/cpl_port.h"
    "/home/gdal/port/cpl_progress.h"
    "/home/gdal/port/cpl_quad_tree.h"
    "/home/gdal/port/cpl_spawn.h"
    "/home/gdal/port/cpl_string.h"
    "/home/gdal/port/cpl_time.h"
    "/home/gdal/port/cpl_vsi.h"
    "/home/gdal/port/cpl_vsi_error.h"
    "/home/gdal/port/cpl_vsi_virtual.h"
    "/home/gdal/port/cpl_virtualmem.h"
    "/home/gdal/port/gdal_csv.h"
    "/home/gdal/port/cpl_minizip_ioapi.h"
    "/home/gdal/port/cpl_minizip_unzip.h"
    "/home/gdal/port/cpl_minizip_zip.h"
    "/home/gdal/alg/gdal_alg.h"
    "/home/gdal/alg/gdal_alg_priv.h"
    "/home/gdal/alg/gdalgrid.h"
    "/home/gdal/alg/gdalgrid_priv.h"
    "/home/gdal/alg/gdalwarper.h"
    "/home/gdal/alg/gdal_simplesurf.h"
    "/home/gdal/alg/gdalpansharpen.h"
    "/home/gdal/ogr/ogr_api.h"
    "/home/gdal/ogr/ogr_recordbatch.h"
    "/home/gdal/ogr/ogr_core.h"
    "/home/gdal/ogr/ogr_feature.h"
    "/home/gdal/ogr/ogr_featurestyle.h"
    "/home/gdal/ogr/ogr_geocoding.h"
    "/home/gdal/ogr/ogr_geometry.h"
    "/home/gdal/ogr/ogr_p.h"
    "/home/gdal/ogr/ogr_spatialref.h"
    "/home/gdal/ogr/ogr_swq.h"
    "/home/gdal/ogr/ogr_srs_api.h"
    "/home/gdal/ogr/ogrsf_frmts/ogrsf_frmts.h"
    "/home/gdal/gnm/gnm.h"
    "/home/gdal/gnm/gnm_api.h"
    "/home/gdal/gnm/gnmgraph.h"
    "/home/gdal/frmts/mem/memdataset.h"
    "/home/gdal/frmts/vrt/vrtdataset.h"
    "/home/gdal/frmts/vrt/gdal_vrt.h"
    "/home/gdal/build/gcore/gdal_version_full/gdal_version.h"
    "/home/gdal/gcore/gdal.h"
    "/home/gdal/gcore/gdaljp2metadata.h"
    "/home/gdal/gcore/gdaljp2abstractdataset.h"
    "/home/gdal/gcore/gdal_frmts.h"
    "/home/gdal/gcore/gdal_pam.h"
    "/home/gdal/gcore/gdal_priv.h"
    "/home/gdal/gcore/gdal_proxy.h"
    "/home/gdal/gcore/gdal_rat.h"
    "/home/gdal/gcore/gdalcachedpixelaccessor.h"
    "/home/gdal/gcore/rawdataset.h"
    "/home/gdal/gcore/gdalgeorefpamdataset.h"
    "/home/gdal/gcore/gdal_mdreader.h"
    "/home/gdal/apps/gdal_utils.h"
    "/home/gdal/build/port/cpl_config.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gdal" TYPE FILE FILES
    "/home/gdal/LICENSE.TXT"
    "/home/gdal/data/GDALLogoBW.svg"
    "/home/gdal/data/GDALLogoColor.svg"
    "/home/gdal/data/GDALLogoGS.svg"
    "/home/gdal/data/bag_template.xml"
    "/home/gdal/data/cubewerx_extra.wkt"
    "/home/gdal/data/default.rsc"
    "/home/gdal/data/ecw_cs.wkt"
    "/home/gdal/data/eedaconf.json"
    "/home/gdal/data/epsg.wkt"
    "/home/gdal/data/esri_StatePlane_extra.wkt"
    "/home/gdal/data/gdalicon.png"
    "/home/gdal/data/gdalmdiminfo_output.schema.json"
    "/home/gdal/data/gdalvrt.xsd"
    "/home/gdal/data/gml_registry.xml"
    "/home/gdal/data/gmlasconf.xml"
    "/home/gdal/data/gmlasconf.xsd"
    "/home/gdal/data/grib2_table_versions.csv"
    "/home/gdal/data/grib2_center.csv"
    "/home/gdal/data/grib2_process.csv"
    "/home/gdal/data/grib2_subcenter.csv"
    "/home/gdal/data/grib2_table_4_2_0_0.csv"
    "/home/gdal/data/grib2_table_4_2_0_13.csv"
    "/home/gdal/data/grib2_table_4_2_0_14.csv"
    "/home/gdal/data/grib2_table_4_2_0_15.csv"
    "/home/gdal/data/grib2_table_4_2_0_16.csv"
    "/home/gdal/data/grib2_table_4_2_0_17.csv"
    "/home/gdal/data/grib2_table_4_2_0_18.csv"
    "/home/gdal/data/grib2_table_4_2_0_190.csv"
    "/home/gdal/data/grib2_table_4_2_0_191.csv"
    "/home/gdal/data/grib2_table_4_2_0_19.csv"
    "/home/gdal/data/grib2_table_4_2_0_1.csv"
    "/home/gdal/data/grib2_table_4_2_0_20.csv"
    "/home/gdal/data/grib2_table_4_2_0_2.csv"
    "/home/gdal/data/grib2_table_4_2_0_3.csv"
    "/home/gdal/data/grib2_table_4_2_0_4.csv"
    "/home/gdal/data/grib2_table_4_2_0_5.csv"
    "/home/gdal/data/grib2_table_4_2_0_6.csv"
    "/home/gdal/data/grib2_table_4_2_0_7.csv"
    "/home/gdal/data/grib2_table_4_2_10_0.csv"
    "/home/gdal/data/grib2_table_4_2_10_191.csv"
    "/home/gdal/data/grib2_table_4_2_10_1.csv"
    "/home/gdal/data/grib2_table_4_2_10_2.csv"
    "/home/gdal/data/grib2_table_4_2_10_3.csv"
    "/home/gdal/data/grib2_table_4_2_10_4.csv"
    "/home/gdal/data/grib2_table_4_2_1_0.csv"
    "/home/gdal/data/grib2_table_4_2_1_1.csv"
    "/home/gdal/data/grib2_table_4_2_1_2.csv"
    "/home/gdal/data/grib2_table_4_2_20_0.csv"
    "/home/gdal/data/grib2_table_4_2_20_1.csv"
    "/home/gdal/data/grib2_table_4_2_20_2.csv"
    "/home/gdal/data/grib2_table_4_2_2_0.csv"
    "/home/gdal/data/grib2_table_4_2_2_3.csv"
    "/home/gdal/data/grib2_table_4_2_2_4.csv"
    "/home/gdal/data/grib2_table_4_2_2_5.csv"
    "/home/gdal/data/grib2_table_4_2_3_0.csv"
    "/home/gdal/data/grib2_table_4_2_3_1.csv"
    "/home/gdal/data/grib2_table_4_2_3_2.csv"
    "/home/gdal/data/grib2_table_4_2_3_3.csv"
    "/home/gdal/data/grib2_table_4_2_3_4.csv"
    "/home/gdal/data/grib2_table_4_2_3_5.csv"
    "/home/gdal/data/grib2_table_4_2_3_6.csv"
    "/home/gdal/data/grib2_table_4_2_4_0.csv"
    "/home/gdal/data/grib2_table_4_2_4_10.csv"
    "/home/gdal/data/grib2_table_4_2_4_1.csv"
    "/home/gdal/data/grib2_table_4_2_4_2.csv"
    "/home/gdal/data/grib2_table_4_2_4_3.csv"
    "/home/gdal/data/grib2_table_4_2_4_4.csv"
    "/home/gdal/data/grib2_table_4_2_4_5.csv"
    "/home/gdal/data/grib2_table_4_2_4_6.csv"
    "/home/gdal/data/grib2_table_4_2_4_7.csv"
    "/home/gdal/data/grib2_table_4_2_4_8.csv"
    "/home/gdal/data/grib2_table_4_2_4_9.csv"
    "/home/gdal/data/grib2_table_4_2_local_Canada.csv"
    "/home/gdal/data/grib2_table_4_2_local_HPC.csv"
    "/home/gdal/data/grib2_table_4_2_local_index.csv"
    "/home/gdal/data/grib2_table_4_2_local_MRMS.csv"
    "/home/gdal/data/grib2_table_4_2_local_NCEP.csv"
    "/home/gdal/data/grib2_table_4_2_local_NDFD.csv"
    "/home/gdal/data/grib2_table_4_5.csv"
    "/home/gdal/data/gt_datum.csv"
    "/home/gdal/data/gt_ellips.csv"
    "/home/gdal/data/header.dxf"
    "/home/gdal/data/inspire_cp_BasicPropertyUnit.gfs"
    "/home/gdal/data/inspire_cp_CadastralBoundary.gfs"
    "/home/gdal/data/inspire_cp_CadastralParcel.gfs"
    "/home/gdal/data/inspire_cp_CadastralZoning.gfs"
    "/home/gdal/data/jpfgdgml_AdmArea.gfs"
    "/home/gdal/data/jpfgdgml_AdmBdry.gfs"
    "/home/gdal/data/jpfgdgml_AdmPt.gfs"
    "/home/gdal/data/jpfgdgml_BldA.gfs"
    "/home/gdal/data/jpfgdgml_BldL.gfs"
    "/home/gdal/data/jpfgdgml_Cntr.gfs"
    "/home/gdal/data/jpfgdgml_CommBdry.gfs"
    "/home/gdal/data/jpfgdgml_CommPt.gfs"
    "/home/gdal/data/jpfgdgml_Cstline.gfs"
    "/home/gdal/data/jpfgdgml_ElevPt.gfs"
    "/home/gdal/data/jpfgdgml_GCP.gfs"
    "/home/gdal/data/jpfgdgml_LeveeEdge.gfs"
    "/home/gdal/data/jpfgdgml_RailCL.gfs"
    "/home/gdal/data/jpfgdgml_RdASL.gfs"
    "/home/gdal/data/jpfgdgml_RdArea.gfs"
    "/home/gdal/data/jpfgdgml_RdCompt.gfs"
    "/home/gdal/data/jpfgdgml_RdEdg.gfs"
    "/home/gdal/data/jpfgdgml_RdMgtBdry.gfs"
    "/home/gdal/data/jpfgdgml_RdSgmtA.gfs"
    "/home/gdal/data/jpfgdgml_RvrMgtBdry.gfs"
    "/home/gdal/data/jpfgdgml_SBAPt.gfs"
    "/home/gdal/data/jpfgdgml_SBArea.gfs"
    "/home/gdal/data/jpfgdgml_SBBdry.gfs"
    "/home/gdal/data/jpfgdgml_WA.gfs"
    "/home/gdal/data/jpfgdgml_WL.gfs"
    "/home/gdal/data/jpfgdgml_WStrA.gfs"
    "/home/gdal/data/jpfgdgml_WStrL.gfs"
    "/home/gdal/data/netcdf_config.xsd"
    "/home/gdal/data/nitf_spec.xml"
    "/home/gdal/data/nitf_spec.xsd"
    "/home/gdal/data/ogrvrt.xsd"
    "/home/gdal/data/osmconf.ini"
    "/home/gdal/data/ozi_datum.csv"
    "/home/gdal/data/ozi_ellips.csv"
    "/home/gdal/data/pci_datum.txt"
    "/home/gdal/data/pci_ellips.txt"
    "/home/gdal/data/pdfcomposition.xsd"
    "/home/gdal/data/pds4_template.xml"
    "/home/gdal/data/plscenesconf.json"
    "/home/gdal/data/ruian_vf_ob_v1.gfs"
    "/home/gdal/data/ruian_vf_st_uvoh_v1.gfs"
    "/home/gdal/data/ruian_vf_st_v1.gfs"
    "/home/gdal/data/ruian_vf_v1.gfs"
    "/home/gdal/data/s57agencies.csv"
    "/home/gdal/data/s57attributes.csv"
    "/home/gdal/data/s57expectedinput.csv"
    "/home/gdal/data/s57objectclasses.csv"
    "/home/gdal/data/seed_2d.dgn"
    "/home/gdal/data/seed_3d.dgn"
    "/home/gdal/data/stateplane.csv"
    "/home/gdal/data/template_tiles.mapml"
    "/home/gdal/data/tms_LINZAntarticaMapTileGrid.json"
    "/home/gdal/data/tms_MapML_APSTILE.json"
    "/home/gdal/data/tms_MapML_CBMTILE.json"
    "/home/gdal/data/tms_NZTM2000.json"
    "/home/gdal/data/trailer.dxf"
    "/home/gdal/data/vdv452.xml"
    "/home/gdal/data/vdv452.xsd"
    "/home/gdal/data/vicar.json"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal/GDAL-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal/GDAL-targets.cmake"
         "/home/gdal/build/CMakeFiles/Export/lib/cmake/gdal/GDAL-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal/GDAL-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal/GDAL-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal" TYPE FILE FILES "/home/gdal/build/CMakeFiles/Export/lib/cmake/gdal/GDAL-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal" TYPE FILE FILES "/home/gdal/build/CMakeFiles/Export/lib/cmake/gdal/GDAL-targets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal" TYPE FILE FILES "/home/gdal/build/GDALConfigVersion.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/gdal" TYPE FILE FILES "/home/gdal/build/GDALConfig.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xapplicationsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/home/gdal/build/apps/gdal-config")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibrariesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/gdal/build/gdal.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/autotest/cpp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/autotest/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/fuzzers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/gdal/build/fuzzers/tests/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/gdal/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
