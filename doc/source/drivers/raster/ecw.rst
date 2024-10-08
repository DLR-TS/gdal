.. _raster.ecw:

================================================================================
ECW -- Enhanced Compressed Wavelets (.ecw)
================================================================================

.. shortname:: ECW

.. build_dependencies:: ECW SDK

GDAL supports reading and writing ECW files using the ERDAS ECW/JP2 SDK
developed by Hexagon Geospatial (formerly Intergraph, ERDAS, ERMapper).
Support is optional and requires linking in the libraries available from
the ECW/JP2 SDK Download page.

Driver capabilities
-------------------

.. supports_createcopy::

.. supports_create::

.. supports_georeferencing::

.. supports_virtualio::

Licensing
---------

The ERDAS ECW/JP2 SDK v5.x is available under multiple license types.
For Desktop usage, decoding any sized ECW/JP2 image is made available
free of charge. To compress, deploy on a Server platform, or decode
unlimited sized files on Mobile platforms a license must be purchased
from Hexagon Geospatial.

History
-------

-  v3.x - Last release, 2006
-  v4.x - Last release, 2012
-  v5.x - Active development, 2013 - current

Creation Options
----------------

The ERDAS ECW/JP2 v4.x and v5.x SDK is only free for image
decompression. To compress images it is necessary to build with the
read/write SDK and to provide an OEM licensing key at runtime which may
be purchased from ERDAS.

For those still using the ECW 3.3 SDK, images less than 500MB may be
compressed for free, while larger images require licensing from ERDAS.
See the licensing agreement and the LARGE_OK option.

Files to be compressed into ECW format must also be at least 128x128.
ECW currently only supports 8 bits per channel for ECW Version 2 files.
ECW Version 3 files supports 16 bits per channel (as Uint16 data type).
Please see Creation options to enable ECW V3 files writing

When writing coordinate system information to ECW files, many less
common coordinate systems are not mapped properly. If you know the ECW
name for the coordinate system you can force it to be set at creation
time with the PROJ and DATUM creation options.

ECW format does not support creation of overviews since the ECW format
is already considered to be optimized for "arbitrary overviews".

.. _creation-options-1:

Creation Options:
-----------------

|about-creation-options|
The following creation options are supported:

-  .. co:: LARGE_OK
      :choices: YES, NO

      *(v3.x SDK only)* Allow compressing files larger
      than 500MB in accordance with EULA terms. Deprecated since v4.x and
      replaced by :co:`ECW_ENCODE_KEY` & :co:`ECW_ENCODE_COMPANY`.

-  .. co:: ECW_ENCODE_KEY

      *(v4.x SDK or higher)* Provide the OEM
      encoding key to unlock encoding capability up to the licensed
      gigapixel limit. The key is approximately 129 hex digits long. The
      Company and Key must match and must be re-generated with each minor
      release of the SDK. It may also be provided globally as a
      configuration option.

-  .. co:: ECW_ENCODE_COMPANY

      *(v4.x SDK or higher)* Provide the name
      of the company in the issued OEM key (see :co:`ECW_ENCODE_KEY`). The
      Company and Key must match and must be re-generated with each minor
      release of the SDK. It may also be provided globally as a
      configuration option.

-  .. co:: TARGET

      Set the target size reduction as a percentage of
      the original. If not provided defaults to 90% for greyscale images,
      and 95% for RGB images.

-  .. co:: PROJ

      Name of the ECW projection string to use. Common
      examples are NUTM11, or GEODETIC.

-  .. co:: DATUM

      Name of the ECW datum string to use. Common examples
      are WGS84 or NAD83.

-  .. co:: UNITS
      :choices: METERS, FEET
      :default: METERS

      Name of the ECW projection units to use : METERS (default) or FEET (us-foot).

-  .. co:: ECW_FORMAT_VERSION
      :choices: 2, 3
      :default: 2

      When building with the ECW
      5.x SDK this option can be set to allow ECW Version 3 files to be
      created. Default, 2 to retain widest compatibility.

Configuration Options
---------------------

|about-config-options|
The ERDAS ECW SDK supports a variety of configuration options to control various features. See the ECW SDK documentation for full details on the meaning
of these options.

-  .. config:: ECW_CACHE_MAXMEM
      :choices: <bytes>

      maximum bytes of RAM used for in-memory
      caching. If not set, up to one quarter of physical RAM will be used
      by the SDK for in-memory caching.

-  .. config:: ECWP_CACHE_LOCATION
      :choices: <path>

      Path to a directory to use for caching
      ECWP results. If unset ECWP caching will not be enabled.

-  .. config:: ECWP_CACHE_SIZE_MB
      :choices: <megabytes>

      The maximum number of
      megabytes of space in the :config:`ECWP_CACHE_LOCATION` to be used for caching
      ECWP results.

-  .. config:: ECWP_BLOCKING_TIME_MS
      :choices: <milliseconds>
      :default: 10000

      time an ecwp:// blocking read will wait before returning.

-  .. config:: ECWP_REFRESH_TIME_MS
      :choices: <milliseconds>
      :default: 10000

      time delay between blocks arriving and the
      next refresh callback. For the purposes of GDAL
      this is the amount of time the driver will wait for more data on an
      ecwp connection for which the final result has not yet been returned.
      If set small then RasterIO() requests will often produce low
      resolution results.

-  .. config:: ECW_TEXTURE_DITHER
      :choices: TRUE, FALSE
      :default: TRUE

      This may be set to FALSE to
      disable dithering when decompressing ECW files.

-  .. config:: ECW_FORCE_FILE_REOPEN
      :choices: TRUE, FALSE
      :default: FALSE

      This may be set to TRUE to
      force open a file handle for each file for each connection made.

-  .. config:: ECW_CACHE_MAXOPEN
      :choices: <integer>

      The maximum number of files to keep
      open for ECW file handle caching. Defaults to unlimited.

-  .. config:: ECW_RESILIENT_DECODING
      :choices: TRUE, FALSE
      :default: TRUE

      Controls whether the reader
      should be forgiving of errors in a file, trying to return as much
      data as is available. If set to FALSE an invalid
      file will result in an error.

The GDAL-specific options:

-  .. config:: ECW_ALWAYS_UPWARD
      :choices: TRUE, FALSE
      :default: TRUE

      If TRUE, the driver sets negative
      Y-resolution and assumes an image always has the "Upward" orientation
      (Y coordinates increase upward). This may be set to FALSE to let the
      driver rely on the actual image orientation, using Y-resolution value
      (sign) of an image, to allow correct processing of rare images with
      "Downward" orientation (Y coordinates increase "Downward" and
      Y-resolution is positive). Defaults to TRUE.

ECW Version 3 Files
~~~~~~~~~~~~~~~~~~~

ECW 5.x SDK introduces a new file format version which,

#. Storage of data statistics, histograms, metadata, RPC information
   within the file header
#. Support for UInt16 data type
#. Ability to update regions within an existing ECW v3 file
#. Introduces other space saving optimizations

Note: This version is not backward compatible and will fail to decode in
v3.x or v4.x ECW/JP2 SDK's. The File VERSION Metadata will advertise
whether the file is ECW v2 or ECW v3.

ECWP
~~~~

In addition to local files, this driver also supports access to
streaming network imagery services using the proprietary "ECWP" protocol
from the ERDAS APOLLO product. Use the full ecwp:// prefixed dataset url
as input. When built with ECW/JP2 SDK v4.1 or newer it is also possible
to take advantage of :ref:`rfc-24`
for asynchronous / progressive streaming access to ECWP services.

Metadata / Georeferencing
~~~~~~~~~~~~~~~~~~~~~~~~~

The PROJ, DATUM and UNITS found in the ECW header are reported in the
ECW metadata domain. They can also be set with the SetMetadataItem()
method, in order to update the header information of an existing ECW
file, opened in update mode, without modifying the imagery.

The geotransform and projection can also be modified with the
SetGeoTransform() and SetProjection() methods. If the projection is set
with SetProjection() and the PROJ, DATUM or UNITS with
SetMetadataItem(), the later values will override the values built from
the projection string.

All those can for example be modified with the -a_ullr, -a_srs or -mo
switches of the :ref:`gdal_edit` utility.

For example:

::

   gdal_edit.py -mo DATUM=WGS84 -mo PROJ=GEODETIC -a_ullr 7 47 8 46 test.ecw

   gdal_edit.py -a_srs EPSG:3068 -a_ullr 20800 22000 24000 19600 test.ecw

File Metadata Keys:
~~~~~~~~~~~~~~~~~~~

-  FILE_METADATA_ACQUISITION_DATE
-  FILE_METADATA_ACQUISITION_SENSOR_NAME
-  FILE_METADATA_ADDRESS
-  FILE_METADATA_AUTHOR
-  FILE_METADATA_CLASSIFICATION
-  FILE_METADATA_COMPANY - should be set to ECW_ENCODE_COMPANY
-  FILE_METADATA_COMPRESSION_SOFTWARE - updated during recompression
-  FILE_METADATA_COPYRIGHT
-  FILE_METADATA_EMAIL
-  FILE_METADATA_TELEPHONE
-  CLOCKWISE_ROTATION_DEG
-  COLORSPACE
-  COMPRESSION_DATE
-  COMPRESSION_RATE_ACTUAL
-  COMPRESSION_RATE_TARGET. This is the percentage of the target
   compressed file size divided by the uncompressed file size. This is
   equal to 100 / (100 - TARGET) where TARGET is the value of the TARGET
   creation option used at file creation, so a COMPRESSION_RATE_TARGET=1
   is equivalent to a TARGET=0 (ie no compression),
   COMPRESSION_RATE_TARGET=5 is equivalent to TARGET=80 (ie dividing
   uncompressed file size by 5), etc...
-  VERSION

Standalone plugin compilation
-----------------------------

.. versionadded:: 3.10

While this driver may be built as part of a whole GDAL build, either in libgdal
itself, or as a plugin, it is also possible to only build this driver as a plugin,
against an already built libgdal.

The version of the GDAL sources used to build the driver must match the version
of the libgdal it is built against.

For example, from a "build_georaster" directory under the root of the GDAL source tree:

::

    cmake -S ../frmts/ecw -DCMAKE_PREFIX_PATH=/path/to/GDAL_installation_prefix -DECW_ROOT=/path/to/ecw_sdk_root
    cmake --build .


Note that such a plugin, when used against a libgdal not aware of it, will be
systematically loaded at GDAL driver initialization time, and will not benefit from
`deferred plugin loading capabilities <rfc-96>`. For that, libgdal itself must be built with the
CMake variable GDAL_REGISTER_DRIVER_ECW_FOR_LATER_PLUGIN=ON set.

See Also
--------

-  Implemented as :source_file:`frmts/ecw/ecwdataset.cpp`.
-  ERDAS ECW/JP2 SDK Read-Only Redistributable available at
   `www.hexagongeospatial.com <https://supportsi.hexagon.com/help/s/article/ERDAS-ECW-JP2-SDK-Read-Only-Redistributable-download>`__
-  Further product information available in the `User
   Guide <https://bynder.hexagon.com/m/5af86a8895f6696f/original/Hexagon_GSP_ERDAS_ECW_JP2_SDK_5_5_User_Guide.pdf>`__
-  Support for non-GDAL specific issues should be directed to the
   `Hexagon Geospatial public
   forum <https://supportsi.hexagon.com/help/s/erdas-apollo>`__
-  Community contributed `patches <https://github.com/rouault/libecwj2-3.3-builds/blob/main/libecwj2-3.3.patch>`__ to apply to ECW SDK 3.3 sources
