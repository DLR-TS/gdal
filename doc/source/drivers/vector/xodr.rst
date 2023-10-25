.. _vector.xodr:

XODR -- OpenDRIVE Road Description Format
========================================

.. shortname:: XODR

.. build_dependencies:: libOpenDRIVE

This driver provides read access to road network elements of an OpenDRIVE dataset.

`OpenDRIVE <https://www.asam.net/standards/detail/opendrive/>`_ is an open industry format for lane-detailed description of road networks, commonly used in applications of the automotive and transportation systems domains. It bundles polynomial, continuous road geometry modelling with all necessary topological links and semantic information from traffic-regulating infrastructure (signs and traffic lights).

Specification version
---------------------

The currently supported OpenDRIVE version is 1.4 and basically depends on what is provided by `libOpenDRIVE <https://github.com/pageldev/libOpenDRIVE/>`_. 

Supported OpenDRIVE elements
+++++++++++++++++++++++++++++

The XODR driver exposes OpenDRIVE's different road elements as separate layers by converting geometric elements into 3-dimensional OGR geometry types. The following _`layer types` are currently implemented:

* *ReferenceLine*: Road reference line (``<planView>``) as :cpp:class:`OGRLineString`.
* *LaneBorder*: Outer road lane border as :cpp:class:`OGRLineString`.
* *Lane*: Polygonal surface (TIN) of the lane mesh as :cpp:class:`OGRTriangulatedSurface`.
* *RoadMark*: Polygonal surface (TIN) of the road mark mesh as :cpp:class:`OGRTriangulatedSurface`.
* *RoadObject*: Polygonal surface (TIN) of the road object mesh as :cpp:class:`OGRTriangulatedSurface`.
* *RoadSignal*: Polygonal surface (TIN) of the road signal mesh as :cpp:class:`OGRTriangulatedSurface`.

Spatial reference
-----------------

By definition, OpenDRIVE geometries are always referenced in a Cartesian coordinate system which defaults to having its origin at ``(0, 0, 0)``. If real-world coordinates are used, the OpenDRIVE header provides a PROJ.4 string with the definition of a projected spatial reference system:

::

  <header ...>
    <geoReference><![CDATA[+proj=tmerc +lat_0=0 +lon_0=9 +k=0.9996 +x_0=500000 +y_0=0 +datum=WGS84 +units=m +no_defs]]></geoReference>
  </header>

The XODR driver uses this PROJ definition as spatial reference for creation of all OGR geometry layers. 

Open options
----------------------

The following open options can be specified
(typically with the ``-oo name=value`` parameters of :program:`ogrinfo` or :program:`ogr2ogr`):

-  .. oo:: EPS
      :choices: <float>
      :default: 1.0

      Epsilon value for linear approximation of continuous OpenDRIVE geometries. A smaller value results in a finer sampling. This parameter is internally forwarded to libOpenDRIVE.

-  .. oo:: DISSOLVE_TIN
      :choices: YES, NO
      :default: NO

      Whether to dissolve triangulated surfaces. All TIN layers of geometry type :cpp:class:`OGRTriangulatedSurface` can be simplified to single, simple :cpp:class:`OGRPolygon` geometries by setting this option to YES. This performs a :cpp:func:`UnaryUnion` which dissolves boundaries of all touching triangle patches.

Examples
--------

- Translate OpenDRIVE road *ReferenceLine* elements (``<planView>``) to :ref:`Shapefile <vector.shapefile>`. The desired :ref:`layer type <layer types>` which is to be extracted from the dataset is specified as the last parameter of the function call. 

  ::

    ogr2ogr -f "ESRI Shapefile" CulDeSac.shp CulDeSac.xodr ReferenceLine

- Convert the whole OpenDRIVE dataset with all its different layers into a :ref:`GeoPackage <vector.gpkg>`:

  ::

    ogr2ogr -f "GPKG" CulDeSac.gpkg CulDeSac.xodr

- Convert the whole OpenDRIVE dataset with custom parameters :oo:`EPS` and :oo:`DISSOLVE_TIN` into a :ref:`GeoPackage <vector.gpkg>`:

  ::

    ogr2ogr -f "GPKG" CulDeSac.gpkg CulDeSac.xodr -oo EPS=0.9 -oo DISSOLVE_TIN=YES

Limitations
-----------

By default, OpenDRIVE XML files are opened by GDAL always in UTF-8 encoding, see :cpp:func:`VSIFOpenExL`.

Building Notes
--------------

Building of the driver as plugin is tested to work on

* Ubuntu 22.04 using GCC
* Windows 10 using GCC 13.1.0 (with MCF threads) + MinGW-w64 11.0.0 (MSVCRT runtime), which is obtainable from `WinLibs <https://winlibs.com/>`_.

Ensure to meet the following driver dependencies:

* PROJ
* GEOS
* Clone the sources of `libOpenDRIVE <https://github.com/grepthat/libOpenDRIVE>`_. Then build and install the library, preferably with CMake.

Then, after checking out GDAL sources with this driver extension, create the build directory:

  ::

    cd <gdal>
    mkdir build
    cd build

From the build directory configure CMake to activate our XODR driver as plugin:

  ::

    cmake .. -DOGR_ENABLE_DRIVER_XODR=TRUE -DOGR_ENABLE_DRIVER_XODR_PLUGIN=TRUE -DOpenDrive_DIR=/path/to/libOpenDRIVE/installdir/cmake/

.. note:: The :file:`cmake/` path is usually automatically created when installing libOpenDRIVE and contains the necessary configuration files for inclusion into other project builds, such as this.

Now, build GDAL and install it:

  ::

    cmake --build .
    cmake --build . --target install

Afterwards you will find a new shared library file :file:`{path/to/GDAL/installdir}/lib/gdalplugins/ogr_XODR`.

Verifying a successful build
++++++++++++++++++++++++++++

Check if XODR driver is found:

  ::
    
    cd <gdal>/build/apps/
    ./ogrinfo --formats

This should print a list of supported OGR formats, including `XODR` in the first row:

  ::

    Supported Formats:
      XODR -vector- (rov): OpenDRIVE - Open Dynamic Road Information for Vehicle Environment
      PCIDSK -raster,vector- (rw+v): PCIDSK Database File       
      PDS4 -raster,vector- (rw+vs): NASA Planetary Data System 4
      ...

If you are on Linux, depending on your environment, you might experience linker errors like: 

  ::

    ogrinfo: error while loading shared libraries: libgdal.so.33: cannot open shared object file: No such file or directory

In such a case set the following environment variables:

  ::

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<gdal>/build/apps
    export GDAL_DRIVER_PATH=<gdal>/build/gdalplugins/
    