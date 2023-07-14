# How to build XODR driver

Build GDAL with CMake using the following CMake options:

- `OGR_ENABLE_DRIVER_XODR:BOOL = TRUE`
- `OGR_ENABLE_DRIVER_XODR_PLUGIN:BOOL = TRUE`

Ensure to meet the dependencies:

- libOpenDRIVE*
- PROJ
- GEOS
- Optional: SQLite3 with RTree extension if you want to export into GeoPackage

After building and installing, you will find a new shared library `ogr_XODR` under `gdalplugins/` folder.

### **To build correct libopendrive version:**
```bash

$ git clone -b dlr https://gitlab.dlr.de ts-spatial-data-processing transport-feature-manipulation/libopendrive.git 

cd libopendrive && mkdir build && cd build
cmake ..
make && make install

```