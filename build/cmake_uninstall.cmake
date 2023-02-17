# install_manifest.txt is created in the top build tree, not the project one
if (NOT EXISTS "/home/gdal/build/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"/home/gdal/build/install_manifest.txt\"")
endif()

set(uninstall_file_list "/home/gdal/build/install_manifest.txt")
if(EXISTS "/home/gdal/build/install_manifest_extra.txt")
   list(APPEND uninstall_file_list "/home/gdal/build/install_manifest_extra.txt")
endif()
if(EXISTS "/home/gdal/build/swig/python/record.txt")
   list(APPEND uninstall_file_list "/home/gdal/build/swig/python/record.txt")
endif()

foreach (manifest_file IN ITEMS ${uninstall_file_list})
    file(READ "${manifest_file}" files)
    string(REGEX REPLACE "\n$" "" files "${files}")
    string(REGEX REPLACE "\n" ";" files "${files}")
    list(REVERSE files)
    foreach (file ${files})
        message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
        if (IS_DIRECTORY "$ENV{DESTDIR}${file}")
            # Only remove csharp related directories, which are the only ones
            # to get in the install_manifest, to avoid doing too dangerous
            # removals.
            if("${file}" MATCHES "csharp")
                execute_process(
                  COMMAND /usr/bin/cmake -E remove_directory "$ENV{DESTDIR}${file}"
                  OUTPUT_VARIABLE rm_out
                  RESULT_VARIABLE rm_retval
               )
               if(NOT ${rm_retval} EQUAL 0)
                   message(STATUS "Problem when removing directory \"$ENV{DESTDIR}${file}\"")
               endif()
            else()
               message(STATUS "Keeping directory \"$ENV{DESTDIR}${file}\".")
            endif()
        elseif (EXISTS "$ENV{DESTDIR}${file}")
            execute_process(
                COMMAND /usr/bin/cmake -E remove "$ENV{DESTDIR}${file}"
                OUTPUT_VARIABLE rm_out
                RESULT_VARIABLE rm_retval
            )
            if(NOT ${rm_retval} EQUAL 0)
                message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
            endif (NOT ${rm_retval} EQUAL 0)
        else ()
            message(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
        endif ()
    endforeach(file)
endforeach()
