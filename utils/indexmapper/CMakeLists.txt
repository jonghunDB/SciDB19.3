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

set (indexmapper_src
    indexmapper.cpp
)

add_executable(indexmapper ${indexmapper_src})
set_target_properties(indexmapper PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${GENERAL_OUTPUT_DIRECTORY}
  )
target_link_libraries(indexmapper
  PUBLIC storage_lib
         rocksdb
         ${LIBRT_LIBRARIES}
         Threads::Threads
         ZLIB::ZLIB
         BZip2::BZip2
  )

