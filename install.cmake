########################################
# BEGIN_COPYRIGHT
#
# Copyright (C) 2008-2019 SciDB, Inc.
# All Rights Reserved.
#
# SciDB is free software: you can redistribute it and/or modify
# it under the terms of the AFFERO GNU General Public License as published by
# the Free Software Foundation.
#
# SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
# INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
# NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
# the AFFERO GNU General Public License for the complete license terms.
#
# You should have received a copy of the AFFERO GNU General Public License
# along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
#
# END_COPYRIGHT
########################################

# C O M P O N E N T S
#scidb-client package
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/libscidbclient${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib COMPONENT scidb-client)

#scidb-utils package
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/arrays.py" DESTINATION bin COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/iquery" DESTINATION bin COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/gen_matrix" DESTINATION bin COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/benchGen" DESTINATION bin COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/loadpipe.py" DESTINATION bin COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/calculate_chunk_length.py" DESTINATION bin COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/spaam.py" DESTINATION bin COMPONENT scidb-utils)
if(NOT CLANGBUILD)
  # indexmapper had odd errors with the clang build, the only binary to have such errors, so I omitted
  # it because it's not part of the core product
  install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/indexmapper" DESTINATION bin COMPONENT scidb-utils)
endif(NOT CLANGBUILD)

install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/PSF_license.txt" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/__init__.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidb_math.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidb_progress.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidb_schema.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidb_afl.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/statistics.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/util.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/counter.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidb_psf.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidb_control.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/pgpass_updater.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/iquery_client.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/psql_client.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/ssh_runner.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidb_rbac.py" DESTINATION bin/scidblib COMPONENT scidb-utils)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidblib/scidbctl_common.py" DESTINATION bin/scidblib COMPONENT scidb-utils)


#scidb-jdbc package
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/jdbc/scidb4j.jar" DESTINATION jdbc COMPONENT scidb-jdbc)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/jdbc/example.jar" DESTINATION jdbc COMPONENT scidb-jdbc)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/jdbc/jdbctest.jar" DESTINATION jdbc COMPONENT scidb-jdbc)

#scidb-dev-tools package
install(PROGRAMS "${CMAKE_BINARY_DIR}/tests/dfa/dfa_tests" DESTINATION bin COMPONENT scidb-dev-tools)
install(PROGRAMS "${CMAKE_BINARY_DIR}/tests/unit/unit_tests" DESTINATION bin COMPONENT scidb-dev-tools)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidbtestharness" DESTINATION bin COMPONENT scidb-dev-tools)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/arg_separator" DESTINATION bin COMPONENT scidb-dev-tools)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidbtestprep.py" DESTINATION bin COMPONENT scidb-dev-tools)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/mu_admin.py" DESTINATION bin COMPONENT scidb-dev-tools)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/box_of_points.py" DESTINATION bin COMPONENT scidb-dev-tools)
install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/daemon.py" DESTINATION etc COMPONENT scidb-dev-tools)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/mu_config.ini" DESTINATION etc COMPONENT scidb-dev-tools)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/log4j.properties" DESTINATION etc COMPONENT scidb-dev-tools)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/libdmalloc${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib COMPONENT libdmalloc)

#scidb-shim package
#Building shim is temporarily disabled because it does not work with C++14. See #4745.
#install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/shim" DESTINATION bin COMPONENT scidb-shim)
#install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/shimsvc" DESTINATION bin COMPONENT scidb-shim)

#
# P L U G I N S
#
#scidb-plugins package
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libpoint${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libmatch${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libbestmatch${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/librational${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libcomplex${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libra_decl${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libmore_math${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libmisc${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libtile_integration${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libfits${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libmpi_test${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)

foreach(LIB dense_linear_algebra linear_algebra)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/lib${LIB}${CMAKE_SHARED_LIBRARY_SUFFIX}"
            DESTINATION lib/scidb/plugins
            COMPONENT scidb-plugins
            RENAME "lib${LIB}-scidb${CMAKE_SHARED_LIBRARY_SUFFIX}")
    install(CODE "execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/update_alternatives.sh ${CMAKE_INSTALL_PREFIX} lib/scidb/plugins ${LIB} ${CMAKE_SHARED_LIBRARY_SUFFIX} ${SCIDB_VERSION_MAJOR} ${SCIDB_VERSION_MINOR} scidb)")
endforeach()

install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libupgrade_chunk_index${CMAKE_SHARED_LIBRARY_SUFFIX}"
        DESTINATION lib/scidb/plugins
        COMPONENT scidb-plugins)

install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/libexample_udos${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
if (TARGET mpi_slave_scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/plugins/mpi_slave_scidb" DESTINATION lib/scidb/plugins COMPONENT scidb-plugins)
endif ()

#
# S C R I P T S
#
# scripts -- package is plugins because these are extensions of the linear_algebra plugin
foreach(SCRIPT bellman_ford_example.sh pagerank_example.sh)
    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/${SCRIPT}" DESTINATION bin COMPONENT scidb-plugins)
endforeach()

#scidb package
if (NOT WITHOUT_SERVER)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidb" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidbctl.py" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidb_config.py" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/disable.py" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/pg_seq_reset.py" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/system_report.py" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/scidb_backup.py" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/rbactool.py" DESTINATION bin COMPONENT scidb)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/packaging_only/scidb_cores" DESTINATION bin COMPONENT scidb)

    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/data/meta.sql" DESTINATION share/scidb COMPONENT scidb)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/log4cxx.properties" DESTINATION share/scidb COMPONENT scidb)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/packaging_only/prelude.txt"       DESTINATION lib/scidb/modules COMPONENT scidb)
endif()

#scidb-dev package
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include COMPONENT scidb-dev PATTERN ".svn" EXCLUDE)

# M K L
# override platform blas,lapack when MKL available
if(MKL_BLAS_FOUND)
    message(STATUS "install.cmake: copying library redirection links from ${LIB_LINKS_DIRECTORY} to lib")
    install(DIRECTORY "${LIB_LINKS_DIRECTORY}/" DESTINATION lib COMPONENT scidb)
else()
    message(SEND_ERROR "Missing libblas, liblapack redirects. Unsupported configuration.")
endif()

# D E B U G   P A C K A G E S
if ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    #scidb-client-dbg package
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/libscidbclient${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-client-dbg)

    #scidb-utils-dbg package
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/iquery${DEBUG_SYMBOLS_EXTENSION}" DESTINATION bin/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-utils-dbg)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/gen_matrix${DEBUG_SYMBOLS_EXTENSION}" DESTINATION bin/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-utils-dbg)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/benchGen${DEBUG_SYMBOLS_EXTENSION}" DESTINATION bin/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-utils-dbg)

    #scidb-dev-tools-dbg package
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/scidbtestharness${DEBUG_SYMBOLS_EXTENSION}" DESTINATION bin/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-dev-tools-dbg)
    install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/arg_separator${DEBUG_SYMBOLS_EXTENSION}" DESTINATION bin/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-dev-tools-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/libdmalloc${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT libdmalloc-dbg)

    #scidb-dbg package
    if (NOT WITHOUT_SERVER)
        install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/${DEBUG_SYMBOLS_DIRECTORY}/scidb${DEBUG_SYMBOLS_EXTENSION}" DESTINATION bin/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-dbg)
    endif()

    #scidb-plugins-dbg package
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libpoint${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libmatch${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libbestmatch${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/librational${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libcomplex${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libra_decl${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libmore_math${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libmisc${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libtile_integration${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libfits${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)
    install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/libmpi_test${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg)

    foreach(LIB dense_linear_algebra linear_algebra)
        install(FILES "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/lib${LIB}${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_DIRECTORY} COMPONENT scidb-plugins-dbg RENAME "lib${LIB}-scidb${CMAKE_SHARED_LIBRARY_SUFFIX}${DEBUG_SYMBOLS_EXTENSION}")
    endforeach()

    if (TARGET mpi_slave_scidb)
        install(PROGRAMS "${GENERAL_OUTPUT_DIRECTORY}/plugins/${DEBUG_SYMBOLS_DIRECTORY}/mpi_slave_scidb${DEBUG_SYMBOLS_EXTENSION}" DESTINATION lib/scidb/plugins/${DEBUG_SYMBOLS_EXTENSION} COMPONENT scidb-plugins-dbg)
    endif ()

endif ()
# E N D   C O M P O N E N T S

# S O U R C E   P A C K A G E
set(SRC_PACKAGE_FILE_NAME
    "scidb-${SCIDB_VERSION_MAJOR}.${SCIDB_VERSION_MINOR}.${SCIDB_VERSION_PATCH}.${SCIDB_REVISION}")

add_custom_target(src_package
    COMMAND rm -rf /tmp/${SRC_PACKAGE_FILE_NAME}
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/${SRC_PACKAGE_FILE_NAME}.tgz
    #
    COMMAND rm -rf /tmp/${SRC_PACKAGE_FILE_NAME}.tar
    COMMAND git archive --prefix=${SRC_PACKAGE_FILE_NAME}/ --output=/tmp/${SRC_PACKAGE_FILE_NAME}.tar HEAD
    COMMAND tar --directory=/tmp -xf /tmp/${SRC_PACKAGE_FILE_NAME}.tar
    COMMAND rm -rf /tmp/${SRC_PACKAGE_FILE_NAME}.tar
    # Put revision into revision file in the tarball
    COMMAND git rev-list --abbrev-commit -1 HEAD > /tmp/${SRC_PACKAGE_FILE_NAME}/revision
    COMMAND ${CMAKE_BINARY_DIR}/../utils/licensing.pl /tmp/${SRC_PACKAGE_FILE_NAME} ${CMAKE_BINARY_DIR}/../utils/scidb.lic
    COMMAND tar  --directory=/tmp/${SRC_PACKAGE_FILE_NAME} -czf ${CMAKE_BINARY_DIR}/${SRC_PACKAGE_FILE_NAME}.tgz .
    COMMAND rm -rf /tmp/${SRC_PACKAGE_FILE_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
