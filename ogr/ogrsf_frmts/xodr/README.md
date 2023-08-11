# How to build the OpenDRIVE driver (XODR)

The following is tested to build on

- Ubuntu 22.04 using GCC
- Windows 10 using GCC 13.1.0 (with MCF threads) + MinGW-w64 11.0.0 (MSVCRT runtime), which is obtainable from [WinLibs](https://winlibs.com/).

Ensure to meet the following driver dependencies:

- PROJ
- GEOS
- Clone the sources of [`libOpenDRIVE`](https://github.com/grepthat/libOpenDRIVE). Then build and install the library, preferably with CMake.

Then, after checking out this GDAL branch create the build directory:

```bash
cd <gdal>
mkdir build
cd build
```

From the build directory configure CMake to activate our XODR driver as plugin:

```bash
cmake .. -DOGR_ENABLE_DRIVER_XODR=TRUE -DOGR_ENABLE_DRIVER_XODR_PLUGIN=TRUE -DOpenDrive_DIR=/path/to/libOpenDRIVE/installdir/cmake/
```

> **_NOTE:_**  The `cmake/` path is usually automatically created when installing `libOpenDRIVE` and contains the necessary configuration files for inclusion into other project builds, such as ours.


Now, build GDAL and install it:

```bash
cmake --build .
cmake --build . --target install
```

Afterwards you will find a new shared library `ogr_XODR` under `/path/to/GDAL/installdir/lib/gdalplugins/` folder.

## Using the driver

Check if XODR driver is found:

```bash
cd <gdal>/build/apps/
./ogrinfo --formats
```

This should print a list of supported OGR formats, including `XODR` in the first row:


```bash
Supported Formats:
  XODR -vector- (rov): OpenDRIVE - Open Dynamic Road Information for Vehicle Environment
  PCIDSK -raster,vector- (rw+v): PCIDSK Database File       
  PDS4 -raster,vector- (rw+vs): NASA Planetary Data System 4
  ...
```
---
> **_NOTE:_** If you are on Linux, depending on your environment you might experience linker errors like: 
```bash
ogrinfo: error while loading shared libraries: libgdal.so.33: cannot open shared object file: No such file or directory

```
In such a case set the following environment variables:

```bash
 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<gdal>/build
 export GDAL_DRIVER_PATH=<gdal>/build/gdalplugins/

```
---
To convert reference lines of an OpenDRIVE file into, e.g., an ESRI Shapefile, use the provided app `ogr2ogr`:

```bash
ogr2ogr -f "ESRI Shapefile" CulDeSac.shp CulDeSac.xodr referenceLine
```

To convert the wohle OpenDRIVE file with all its different layers into a Geopackage:

```bash
ogr2ogr -f "GPKG" CulDeSac.gpkg CulDeSac.xodr
```

> **_NOTE:_**  By default, OpenDRIVE XML files are opened by GDAL always in UTF-8 encoding, see [`cpl_vsil.cpp`](https://github.com/OSGeo/gdal/blob/00b47acb383cacfec3199636a21294e098114778/port/cpl_vsil.cpp#L1837).