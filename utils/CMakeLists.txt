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

# Add subfolder of every utility
add_subdirectory("iquery")
#Building shim is temporarily disabled because it does not work with C++14. See #4745.
#add_subdirectory("shim")
add_subdirectory("scidblib")
# TODO: fix the indexmapper clang build issues to remove this caveat
if(NOT CLANGBUILD)
  # indexmapper had odd errors with the clang build, the only binary to have such errors, so I omitted
  # it because it's not part of the core product
  add_subdirectory("indexmapper")
endif(NOT CLANGBUILD)

set(CONFIGURE_PREPARE_DB_METADATA_LOCAL "data/meta.sql")

set(CONFIGURE_PREPARE_DB_METADATA "${CMAKE_INSTALL_PREFIX}/share/scidb/meta.sql")

set(CONFIGURE_SCIDBPY_CONFIG "${CMAKE_INSTALL_PREFIX}/etc/config.ini")

configure_file(log1.properties "${GENERAL_OUTPUT_DIRECTORY}/log1.properties" COPYONLY)
if(CMAKE_BUILD_TYPE STREQUAL "Valgrind")
  set(CONFIGURE_SCIDB_PY_VALGRIND "use_valgrind = True")
else(CMAKE_BUILD_TYPE STREQUAL "Valgrind")
  set(CONFIGURE_SCIDB_PY_VALGRIND "use_valgrind = False")
endif(CMAKE_BUILD_TYPE STREQUAL "Valgrind")
configure_file(prelude.txt.in "${GENERAL_OUTPUT_DIRECTORY}/packaging_only/prelude.txt")
configure_file(scidb_cores "${GENERAL_OUTPUT_DIRECTORY}/packaging_only/scidb_cores")
configure_file(loadpipe.py "${GENERAL_OUTPUT_DIRECTORY}/loadpipe.py")
configure_file(pg_seq_reset.py "${GENERAL_OUTPUT_DIRECTORY}/pg_seq_reset.py")
configure_file(scidb_backup.py "${GENERAL_OUTPUT_DIRECTORY}/scidb_backup.py")
configure_file(calculate_chunk_length.py "${GENERAL_OUTPUT_DIRECTORY}/calculate_chunk_length.py")
configure_file(system_report.py "${GENERAL_OUTPUT_DIRECTORY}/system_report.py")
configure_file(scidb_config.py "${GENERAL_OUTPUT_DIRECTORY}/scidb_config.py")
configure_file(disable.py "${GENERAL_OUTPUT_DIRECTORY}/disable.py")
configure_file(spaam.py "${GENERAL_OUTPUT_DIRECTORY}/spaam.py")
configure_file(scidbctl.py "${GENERAL_OUTPUT_DIRECTORY}/scidbctl.py")
configure_file(rbactool.py "${GENERAL_OUTPUT_DIRECTORY}/rbactool.py")
configure_file(arrays.py "${GENERAL_OUTPUT_DIRECTORY}/arrays.py")

add_scidb_executable(benchGen benchGen.cc)
set_target_properties(benchGen PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${GENERAL_OUTPUT_DIRECTORY}
  )
