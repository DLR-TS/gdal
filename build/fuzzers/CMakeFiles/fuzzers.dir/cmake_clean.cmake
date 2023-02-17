file(REMOVE_RECURSE
  "CMakeFiles/fuzzers"
  "alg_fuzzer_seed_corpus.zip"
  "cad_fuzzer_seed_corpus.zip"
  "gdal_filesystem_fuzzer_seed_corpus.zip"
  "gdal_fuzzer_seed_corpus.zip"
  "gdal_translate_fuzzer_seed_corpus.zip"
  "gdal_vector_translate_fuzzer_seed_corpus.zip"
  "get_jpeg2000_structure_fuzzer_seed_corpus.zip"
  "gml_geom_import_fuzzer_seed_corpus.zip"
  "gtiff_mmap_seed_corpus.zip"
  "ogr_filesystem_fuzzer_seed_corpus.zip"
  "ogr_fuzzer_seed_corpus.zip"
  "osr_set_from_user_input_fuzzer_seed_corpus.zip"
  "spatialite_geom_import_fuzzer_seed_corpus.zip"
  "wkb_import_fuzzer_seed_corpus.zip"
  "wkt_import_fuzzer_seed_corpus.zip"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/fuzzers.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
