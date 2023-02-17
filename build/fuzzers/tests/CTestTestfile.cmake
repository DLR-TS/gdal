# CMake generated Testfile for 
# Source directory: /home/gdal/fuzzers/tests
# Build directory: /home/gdal/build/fuzzers/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[test-fuzzer-gdal]=] "/home/gdal/build/fuzzers/tests/test_gdal_fuzzer")
set_tests_properties([=[test-fuzzer-gdal]=] PROPERTIES  ENVIRONMENT "LD_LIBRARY_PATH=/home/gdal/build:" _BACKTRACE_TRIPLES "/home/gdal/fuzzers/tests/CMakeLists.txt;22;add_test;/home/gdal/fuzzers/tests/CMakeLists.txt;26;fuzzer_test_target;/home/gdal/fuzzers/tests/CMakeLists.txt;0;")
add_test([=[test-fuzzer-ogr]=] "/home/gdal/build/fuzzers/tests/test_ogr_fuzzer")
set_tests_properties([=[test-fuzzer-ogr]=] PROPERTIES  ENVIRONMENT "LD_LIBRARY_PATH=/home/gdal/build:" _BACKTRACE_TRIPLES "/home/gdal/fuzzers/tests/CMakeLists.txt;22;add_test;/home/gdal/fuzzers/tests/CMakeLists.txt;27;fuzzer_test_target;/home/gdal/fuzzers/tests/CMakeLists.txt;0;")
add_test([=[test-fuzzer-osr_set_from_user_input]=] "/home/gdal/build/fuzzers/tests/test_osr_set_from_user_input_fuzzer")
set_tests_properties([=[test-fuzzer-osr_set_from_user_input]=] PROPERTIES  ENVIRONMENT "LD_LIBRARY_PATH=/home/gdal/build:" _BACKTRACE_TRIPLES "/home/gdal/fuzzers/tests/CMakeLists.txt;22;add_test;/home/gdal/fuzzers/tests/CMakeLists.txt;28;fuzzer_test_target;/home/gdal/fuzzers/tests/CMakeLists.txt;0;")
add_test([=[test-fuzzer-wkb_import]=] "/home/gdal/build/fuzzers/tests/test_wkb_import_fuzzer")
set_tests_properties([=[test-fuzzer-wkb_import]=] PROPERTIES  ENVIRONMENT "LD_LIBRARY_PATH=/home/gdal/build:" _BACKTRACE_TRIPLES "/home/gdal/fuzzers/tests/CMakeLists.txt;22;add_test;/home/gdal/fuzzers/tests/CMakeLists.txt;29;fuzzer_test_target;/home/gdal/fuzzers/tests/CMakeLists.txt;0;")
add_test([=[test-fuzzer-wkt_import]=] "/home/gdal/build/fuzzers/tests/test_wkt_import_fuzzer")
set_tests_properties([=[test-fuzzer-wkt_import]=] PROPERTIES  ENVIRONMENT "LD_LIBRARY_PATH=/home/gdal/build:" _BACKTRACE_TRIPLES "/home/gdal/fuzzers/tests/CMakeLists.txt;22;add_test;/home/gdal/fuzzers/tests/CMakeLists.txt;30;fuzzer_test_target;/home/gdal/fuzzers/tests/CMakeLists.txt;0;")
