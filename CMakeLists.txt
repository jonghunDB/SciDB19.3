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


# protobuf_generate_python was added in 3.4.3
cmake_minimum_required(VERSION 3.4.3)

# CMake 3.4 changed the way symbols are exported in executables. We still need the
# -rdynamic on the executables since plugins loaded by dlopen will need the symbols in the
# executable. See https://cmake.org/cmake/help/latest/policy/CMP0065.html
set(CMAKE_ENABLE_EXPORTS True)


message(STATUS "****************** BEGIN CMakeLists.txt ******************")
# Set this to YES to enable LLVM and clang build
set(CLANGBUILD NO)

# This should point to wherever your LLVM and clang binaries live
set(CLANG_COMPILER_LOCATION "/path/to/binaries")

if(CLANGBUILD)
  # Assumes that clang is in a specific place for now, will update as
  # we continue to on-board clang support in the codebase.
  set(CMAKE_C_COMPILER "${CLANG_COMPILER_LOCATION}/bin/clang")
  set(MPICH_CC "${CLANG_COMPILER_LOCATION}/bin/clang")
  set(CMAKE_CXX_COMPILER "${CLANG_COMPILER_LOCATION}/bin/clang++")
  set(MPICH_CXX "${CLANG_COMPILER_LOCATION}/bin/clang++")
  set(CMAKE_Fortran_COMPILER "/opt/rh/devtoolset-3/root/usr/bin/gfortran")
  set(MPICH_FC "/opt/rh/devtoolset-3/root/usr/bin/gfortran")
else(CLANGBUILD)
  if(EXISTS "/usr/bin/g++-4.9")
    set(CMAKE_C_COMPILER "/usr/bin/gcc-4.9")
    set(MPICH_CC "/usr/bin/gcc-4.9")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.9")
    set(MPICH_CXX "/usr/bin/g++-4.9")
    set(CMAKE_Fortran_COMPILER "/usr/bin/gfortran-4.9")
    set(MPICH_FC "/usr/bin/gfortran-4.9")
  elseif(EXISTS "/opt/rh/devtoolset-3/root/usr/bin/g++")
    set(CMAKE_C_COMPILER "/opt/rh/devtoolset-3/root/usr/bin/gcc")
    set(MPICH_CC "/opt/rh/devtoolset-3/root/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/opt/rh/devtoolset-3/root/usr/bin/g++")
    set(MPICH_CXX "/opt/rh/devtoolset-3/root/usr/bin/g++")
    set(CMAKE_Fortran_COMPILER "/opt/rh/devtoolset-3/root/usr/bin/gfortran")
    set(MPICH_FC "/opt/rh/devtoolset-3/root/usr/bin/gfortran")
  else()
    message(FATAL_ERROR "SCIDBTRUNK/CMakeLists.txt: cannot find g++ 4.9")
  endif()
endif(CLANGBUILD)

project(SciDB C CXX Fortran)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_MESSAGE LAZY)

#
# configure DISTRO_NAME_VERSION
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(LinuxDistroVersion)
message(STATUS "DISTRO_NAME_VER is " ${DISTRO_NAME_VER})

if ("${DISTRO_NAME}" STREQUAL "Ubuntu")
  # Ubuntu/Debian makes uses a suffix to mark the
  # differing implementation of MPI.
  # e.g.
  # /usr/bin/mpicxx.mpich, /usr/bin/mpicxx.openmpi"
  #
  set(MPI_CHECK_PATH_ADDITION "")
  set(MPI_CHECK_SUFFIX ".FLAVOR")
elseif ("${DISTRO_NAME}" STREQUAL "CentOS")
  # CentOS uses a different path location for the
  # differing implementations of MPI:
  # e.g.
  # /usr/lib64/mpich/bin/mpicxx, /usr/lib64/openmpi/bin/mpicxx,
  set(MPI_CHECK_PATH_ADDITION "lib64/FLAVOR/")
  set(MPI_CHECK_SUFFIX "")
else()
  message(WARNING  "Unsupported OS -- ${LSB_RELEASE_ID_SHORT}")
endif()
message(STATUS "  ----------- Looking for /usr/${MPI_CHECK_PATH_ADDITION}bin/XXXX${MPI_CHECK_SUFFIX}")

# Add mpich2scidb as a flavor
foreach (flavor "mpich2scidb" "mpich" "openmpi")
  if (NOT "${MPI_CXX_FOUND}")
    string(REGEX REPLACE "FLAVOR" ${flavor} MPI_CHECK_PATH "/usr/${MPI_CHECK_PATH_ADDITION}bin")
    string(REGEX REPLACE "FLAVOR" ${flavor} MPI_SUFFIX "${MPI_CHECK_SUFFIX}")
    set(MPI_CXX_COMPILER "${MPI_CHECK_PATH}/mpicxx${MPI_SUFFIX}")
    set(MPI_Fortran_COMPILER "${MPI_CHECK_PATH}/mpif77${MPI_SUFFIX}")
    set(MPI_C_COMPILER "${MPI_CHECK_PATH}/mpicc${MPI_SUFFIX}")
    message(STATUS "        ------- MPI_CXX_COMPILER = ${MPI_CXX_COMPILER}")
    message(STATUS "        ------- MPI_Fortran_COMPILER = ${MPI_Fortran_COMPILER}")
    message(STATUS "        ------- MPI_C_COMPILER = ${MPI_C_COMPILER}")

    find_package(MPI)
    if("${MPI_CXX_FOUND}")
      set(MPI_FLAVOR "${flavor}")
      execute_process(
        COMMAND readlink -f ${MPI_CXX_COMPILER}
        OUTPUT_VARIABLE MPI_CXX_COMPILER_FULL_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      message(STATUS " ------- MPI_CXX_COMPILER_FULL_PATH = ${MPI_CXX_COMPILER_FULL_PATH}")
      string(REGEX REPLACE "/bin/[^/]*$" "" MPI_INST_DIR "${MPI_CXX_COMPILER_FULL_PATH}")
      message(STATUS " ------- MPI_INST_DIR = ${MPI_INST_DIR} ---")
      message(STATUS " ------- MPI_FLAVOR = ${MPI_FLAVOR} ---")
    endif()
  endif()
endforeach()

message(STATUS "MPI_CXX_FOUND = ${MPI_CXX_FOUND}")
message(STATUS "MPI_CXX_COMPILER = ${MPI_CXX_COMPILER}")
message(STATUS "MPI_CXX_COMPILE_FLAGS = ${MPI_CXX_COMPILE_FLAGS}")
message(STATUS "MPI_CXX_INCLUDE_PATH = ${MPI_CXX_INCLUDE_PATH}")
message(STATUS "MPI_CXX_LINK_FLAGS = ${MPI_CXX_LINK_FLAGS}")
message(STATUS "MPI_CXX_LIBRARIES = ${MPI_CXX_LIBRARIES}")

message(STATUS "MPI_C_FOUND = ${MPI_C_FOUND}")
message(STATUS "MPI_C_COMPILER = ${MPI_C_COMPILER}")
message(STATUS "MPI_C_COMPILE_FLAGS = ${MPI_C_COMPILE_FLAGS}")
message(STATUS "MPI_C_INCLUDE_PATH = ${MPI_C_INCLUDE_PATH}")
message(STATUS "MPI_C_LINK_FLAGS = ${MPI_C_LINK_FLAGS}")
message(STATUS "MPI_C_LIBRARIES = ${MPI_C_LIBRARIES}")

message(STATUS "MPI_Fortran_FOUND = ${MPI_Fortran_FOUND}")
message(STATUS "MPI_Fortran_COMPILER = ${MPI_Fortran_COMPILER}")
message(STATUS "MPI_Fortran_COMPILE_FLAGS = ${MPI_Fortran_COMPILE_FLAGS}")
message(STATUS "MPI_Fortran_INCLUDE_PATH = ${MPI_Fortran_INCLUDE_PATH}")
message(STATUS "MPI_Fortran_LINK_FLAGS = ${MPI_Fortran_LINK_FLAGS}")
message(STATUS "MPI_Fortran_LIBRARIES = ${MPI_Fortran_LIBRARIES}")

file(READ "${CMAKE_SOURCE_DIR}/version" _SCIDB_VERSION)
STRING(REGEX MATCH "^([0-9]*)\\.([0-9]*)\\.([0-9]*).*$" __SCIDB_VERSION "${_SCIDB_VERSION}")
if(NOT __SCIDB_VERSION)
    message(FATAL_ERROR "Can not parse 'version' file")
endif()
set(SCIDB_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(SCIDB_VERSION_MINOR "${CMAKE_MATCH_2}")
set(SCIDB_VERSION_PATCH "${CMAKE_MATCH_3}")

set(SCIDB_VERSION "${SCIDB_VERSION_MAJOR}.${SCIDB_VERSION_MINOR}.${SCIDB_VERSION_PATCH}")
set(SCIDB_SHORT_VERSION "${SCIDB_VERSION_MAJOR}.${SCIDB_VERSION_MINOR}")

set(PACKAGE_NAME "scidb-${SCIDB_SHORT_VERSION}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/opt/scidb/${SCIDB_SHORT_VERSION}" CACHE STRING "Install path" FORCE)
  message(STATUS "CMAKE_INSTALL_PREFIX Initialized to default setting to: " ${CMAKE_INSTALL_PREFIX})
else(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(STATUS "CMAKE_INSTALL_PREFIX not Initialized to default keeping: " ${CMAKE_INSTALL_PREFIX})
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Assert) # Default value
endif(NOT CMAKE_BUILD_TYPE)

find_package(Git)
if(GIT_EXECUTABLE AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # Get revision from git working copy
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-list --abbrev=8  --abbrev-commit -1 HEAD
        OUTPUT_VARIABLE SCIDB_REVISION
        RESULT_VARIABLE GIT_EXEC_RES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    string(COMPARE NOTEQUAL ${GIT_EXEC_RES} 0 GIT_EXEC_RES)
    if(${GIT_EXEC_RES})
        MESSAGE(FATAL_ERROR "Can not invoke GIT ${GIT_EXECUTABLE}. Check binary!")
    endif()

    message(STATUS "Reading revision from git: ${SCIDB_REVISION}")

    file(WRITE "${CMAKE_BINARY_DIR}/revision" ${SCIDB_REVISION})
endif()
if(NOT SCIDB_REVISION AND EXISTS "${CMAKE_SOURCE_DIR}/revision")
    # Get version from plain source tarball/directory
    file(READ "${CMAKE_SOURCE_DIR}/revision" SCIDB_REVISION)
    string(STRIP ${SCIDB_REVISION} SCIDB_REVISION)
    message(STATUS "Reading revision from file: ${SCIDB_REVISION}")
endif()
if(NOT SCIDB_REVISION)
    message(FATAL_ERROR "Can not fetch working copy version and can't find revision file.")
endif()

set(SCIDB_VERSION "${SCIDB_VERSION}.${SCIDB_REVISION}")

message(STATUS "Build type (use -DCMAKE_BUILD_TYPE=[Assert]/RelWithDebInfo/Debug/CC/Profile/Release/Valgrind/DebugNoAssert): ${CMAKE_BUILD_TYPE}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Assert) # Default value for end user who want to install scidb from sources
endif(NOT CMAKE_BUILD_TYPE)

#
# configure compile and link flags
#
include(CMakeBuildFlags.cmake)

set(GENERAL_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(GENERAL_TEST_DIRECTORY "${CMAKE_BINARY_DIR}/tests/basic")
set(GENERAL_HARNESSTEST_DIRECTORY "${CMAKE_BINARY_DIR}/tests/harness")
set(LIB_LINKS_DIRECTORY "${CMAKE_BINARY_DIR}/softlinks")

if(CLANGBUILD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCLANGBUILD")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCLANGBUILD")
endif(CLANGBUILD)

################################################################
# Setting DT_RUNPATH in the elf header:
#
# A. review of library resolution steps
#
# From "man ld.so":
##The shared libraries needed by the program are searched for in the following order:
##
## 1. (ELF only) Using the directories specified in the DT_RPATH dynamic section attribute of the binary if
## present and DT_RUNPATH attribute does not exist. Use of DT_RPATH is deprecated.
##
## 2. Using the environment variable LD_LIBRARY_PATH. Except if the executable is a set-user-ID/set-group-ID
## binary, in which case it is ignored.
##
## 3. (ELF only) Using the directories specified in the DT_RUNPATH dynamic section attribute of the binary if
## present.
##
## 4. From the cache file /etc/ld.so.cache which contains a compiled list of candidate libraries previously
## found in the augmented library path. If, however, the binary was linked with the -z nodeflib linker
## option, libraries in the default library paths are skipped.
##
## 5. In the default path /lib, and then /usr/lib. If the binary was linked with the -z nodeflib linker
## option, this step is skipped.
#
# NOTE:
#  a. We set DT_RUNPATH so (1) does not apply.
#  b. If the user sets LD_LIBRARY_PATH, then (2) applies, and it is the user's repsonsibility.
#  c. Resolution is normally via step (3).
#  d. Only if that fails, (5) and (6) do apply [we do not use -z nodeflib at this time.]
#
# B. $ORIGIN
#
# From "man ld.so":
##RPATH TOKEN EXPANSION
## The runtime linker provides a number of tokens that can be used in an rpath specification (DT_RPATH or DT_RUNPATH).
##
## $ORIGIN
## ld.so understands the string $ORIGIN (or equivalently ${ORIGIN}) in an rpath specification to mean the directory
## containing the application executable. Thus, an application located in somedir/app could be compiled with
## gcc -Wl,-rpath,'$ORIGIN/../lib' so that it finds an associated shared library in somedir/lib no matter where
## somedir is located in the directory hierarchy.
#
# NOTE:
#   We use $ORIGIN to find the locations
#      ../scidb/<version>/lib and
#      ../scidb/<version>/lib/scidb/plugins
#   from
#        either location above or
#      ../scidb/<version>/bin.
#
# by setting it to "$ORIGIN/../lib:$ORIGIN/../.."
#
#   And we use $ORIGIN/../plugins to find plugins from plugins
#
# NOTE:
#  this also allows resolution to ../scidb/<version>.  This will be fixed at a later point.
#
# C. CMAKE macros and the linker
#
# It should be noted that there is no command to the linker to set RUNPATH directly.
# Instead we set RPATH and pass --enable-new-dtags to the linker.
# The linker then sets DT_RUNPATH rather than DT_RPATH.
#
#  Setting CMAKE_SHARED_LINKER_FLAGS to "-Wl,--enable-new-dtags"
#    means CMAKE macros that set RPATH for shared objects results in RUNPATH being set.
#  Setting CMAKE_EXE_LINKER_FLAGS to "-Wl,--enable-new-dtags"
#    means CMAKE macros that set RPATH for programs results in RUNPATH being set.
#  Setting CMAKE_SKIP_BUILD_RPATH to FALSE
#    assures us that CMAKE sets RPATH at build time.
#  Setting CMAKE_BUILD_WITH_INSTALL_RPATH to TRUE tells CMAKE the object is built with the install time RPATH
#    (which for our purpose is the same as build time) so it will not re-instrument at install time.
#  Setting CMAKE_INSTALL_RPATH_USE_LINK_PATH to TRUE tells CMAKE to add any linkages that were used at build time
#    as absolute paths. This applies to boost objects in /opt/scidb/<version>/3rdparty/boost/lib.
#
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../plugins:$ORIGIN/../lib:$ORIGIN/../../")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags")

find_package(PkgConfig)
#
# CENTOS check
#
if(${DISTRO_NAME_VER} MATCHES "RedHat-6")
 message(FATAL_ERROR "We do not support RedHat build. Use CentOS instead")
endif()

#
# LIBRT config
#
find_package(LibRT REQUIRED)

#
# CITYHASH CONFIG
#
set(CITYHASH_ROOT /opt/scidb/${SCIDB_VERSION_MAJOR}.${SCIDB_VERSION_MINOR}/3rdparty/cityhash)
set(CityHash_NO_SYSTEM_PATHS TRUE)
set(CityHash_NO_CITYHASH_CMAKE TRUE)

find_package(CityHash REQUIRED)

if(NOT CityHash_FOUND)
 message(FATAL_ERROR "Could not find any version of cityhash")
endif(NOT CityHash_FOUND)


#
# BOOST CONFIG
#

set(BOOST_ROOT /opt/scidb/${SCIDB_VERSION_MAJOR}.${SCIDB_VERSION_MINOR}/3rdparty/boost)
set(BOOST_INCLUDEDIR /opt/scidb/19.3/3rdparty/boost/include)
set(Boost_NO_SYSTEM_PATHS TRUE)
set(Boost_NO_BOOST_CMAKE TRUE)

#set(BOOST_ROOT /opt/scidb/${SCIDB_VERSION_MAJOR}.${SCIDB_VERSION_MINOR}/3rdparty/boost)
##set(BOOST_ROOT /opt/scidb/16.9/3rdparty/boost_1_54_0)
#set(BOOST_INCLUDEDIR /opt/scidb/19.3/3rdparty/boost/include)
#set(Boost_NO_SYSTEM_PATHS TRUE)
#set(Boost_NO_BOOST_CMAKE TRUE)

if(CLANGBUILD)
  # Boost 1.66 has a slightly different include layout than 1.54.  On Dave's dev machine,
  # I moved /opt/scidb/18.2/3rdparty/boost to boost.gcc, then expanded and built the
  # latest boost at boost_1_66.  I then created a new boost directory with symlinks
  # to make it match the layout with 1.54 for the couple of exceptions in our system.
  # Obviously, this is not a long-term solution as we should package boost appropriately
  # and let CMake find it wherever it's installed.  TBD at this point.
  # There seems to be little friction with upgrading to 1.66 for the production
  # environment (which would solve this for both clang and gcc builds), so look into
  # that under SDB-6122.
  find_package(Boost 1.66 EXACT REQUIRED
    COMPONENTS date_time
               filesystem
               program_options
               regex
               serialization
               system
               thread
    )
else(CLANGBUILD)
  find_package(Boost 1.54 EXACT REQUIRED
    COMPONENTS date_time
               filesystem
               program_options
               regex
               serialization
               system
               thread
    )
endif(CLANGBUILD)

find_package(Protobuf REQUIRED)
find_package(Log4CXX REQUIRED)
find_package(Doxygen)
find_package(LibPQXX REQUIRED)
find_package(LibPQ REQUIRED)
find_package(OpenSSL 0.9.8 REQUIRED)

#
# configure CPPUNIT
#
pkg_check_modules(CPPUNIT cppunit)

find_package(FLEX 2.5.35 REQUIRED)
find_package(BISON 2.4 REQUIRED)

find_package(SED REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Threads REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(EditLine REQUIRED)

#
# configure BLAS & LAPACK
#
find_package(MKL_BLAS)
if(MKL_BLAS_FOUND)
    # emulate what find_package(BLAS) and find_package(LAPACK)
    # would do if they supported MKL BLAS and LAPACK
    set(BLAS_LIBRARIES ${MKL_BLAS_LIBRARIES})
    set(LAPACK_LIBRARIES ${MKL_BLAS_LIBRARIES})
    set(BLAS_FOUND "TRUE")
    set(LAPACK_FOUND "TRUE")
else()
    message(SEND_ERROR "Can't configure scidb for MKL BLAS LAPACK. Not supported at this time.")
    message(FATAL_ERROR "To use platform libs at your own risk, try commenting out this fatal message.")
    find_package(BLAS REQUIRED)
    find_package(LAPACK REQUIRED)
endif()

#
# Function Check
#
include(CheckFunctionExists)
check_function_exists(malloc_stats HAVE_MALLOC_STATS)

#
# INCLUDE DIRECTORIES
#
include_directories("${CMAKE_SOURCE_DIR}/udos")
include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/utils")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/utils")
include_directories(${LOG4CXX_INCLUDE_DIRS})
include_directories(${LIBPQXX_INCLUDE_DIRS})
include_directories(${LIBPQ_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/extern")

include_directories("${CMAKE_SOURCE_DIR}/include/kdTree")

link_directories(${LIBPQXX_LIBRARY_DIRS})
link_directories(${LIBPQ_LIBRARY_DIRS})

# static link requires adding a few more libraries
set (LINK_APR_LIBS "")

#
# DEBUG symbols
# TODO: should probably factor to a xxxxx.cmake file in scidb
function(strip_symbols targ)
  # NOTE:
  #      Symbols are ONLY stripped in RelWithDebInfo Builds
  #
  if("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(DEBUG_SYMBOLS_DIR ".debug")
    set(DEBUG_SYMBOLS_EXT "${DEBUG_SYMBOLS_DIR}")
    add_custom_command(TARGET ${targ}
      POST_BUILD
      COMMAND mkdir -p $<TARGET_FILE_DIR:${targ}>/${DEBUG_SYMBOLS_DIR}
      COMMAND ${CMAKE_OBJCOPY}
              --only-keep-debug $<TARGET_FILE:${targ}>
              $<TARGET_FILE_DIR:${targ}>/${DEBUG_SYMBOLS_DIR}/$<TARGET_FILE_NAME:${targ}>${DEBUG_SYMBOLS_EXT}
      COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:${targ}>
      COMMAND ${CMAKE_OBJCOPY}
              --add-gnu-debuglink=${DEBUG_SYMBOLS_DIR}/$<TARGET_FILE_NAME:${targ}>${DEBUG_SYMBOLS_EXT}
              $<TARGET_FILE:${targ}>
      COMMAND chmod -x $<TARGET_FILE_DIR:${targ}>/${DEBUG_SYMBOLS_DIR}/$<TARGET_FILE_NAME:${targ}>${DEBUG_SYMBOLS_EXT}
      WORKING_DIRECTORY $<TARGET_FILE_DIR:${targ}>)
  endif()
endfunction()

function(add_scidb_executable targ)
  add_executable(${targ} ${ARGN})
  strip_symbols(${targ})
endfunction()

function(add_scidb_library targ)
  add_library(${targ} ${ARGN})
  strip_symbols(${targ})
endfunction()

#
# SUBDIRECTORIES
#
add_subdirectory("udos")
add_subdirectory("extern")
add_subdirectory("src")
add_subdirectory("utils")
if (NOT WITHOUT_SERVER)
    add_subdirectory("tests")
    add_subdirectory("examples")
endif()

#
# TESTING: CTest, Dart, and Cdash
#
ENABLE_TESTING()
INCLUDE(CTest)

if (CDASH_CONFIG)
  ADD_TEST(ScidbInstall "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/scidb/install.sh ${CDASH_CONFIG})
  ADD_TEST(ScidbTest "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/scidb/harness.sh ${CDASH_CONFIG} "prepare_run")
  ADD_TEST(ScidbResult "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/scidb/harness.sh ${CDASH_CONFIG} "collect_cleanup")
  ADD_TEST(P4Install "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/p4/install.sh ${CDASH_CONFIG})
  ADD_TEST(P4Test "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/scidb/harness.sh ${CDASH_CONFIG} "prepare_run")
  ADD_TEST(P4Result "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/scidb/harness.sh ${CDASH_CONFIG} "collect_cleanup")
  ADD_TEST(NamespaceInstall "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/p4/install.sh ${CDASH_CONFIG})
  ADD_TEST(NamespaceTest "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/scidb/harness.sh ${CDASH_CONFIG} "prepare_run")
  ADD_TEST(NamespaceResult "/bin/bash" ${CMAKE_BINARY_DIR}/cdash2/scidb/harness.sh ${CDASH_CONFIG} "collect_cleanup")
  #
  # TEST INSTALL TIMEOUT
  #
  if (NOT INSTALL_TIMEOUT)
    set(INSTALL_TIMEOUT 3600) # 1 hour.
  endif(NOT INSTALL_TIMEOUT)
  #
  # TEST TIMEOUT
  #
  if (NOT HARNESS_TEST_TIMEOUT)
    set(HARNESS_TEST_TIMEOUT 12600) # 3.5 hours - for regular build/tests.
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Valgrind")
      set(HARNESS_TEST_TIMEOUT 86400) # 24 hours - for Valgrind build/tests.
    endif("${CMAKE_BUILD_TYPE}" STREQUAL "Valgrind")
  endif(NOT HARNESS_TEST_TIMEOUT)
  #
  # TEST RESULTS COLLECTION TIMEOUT
  #
  if (NOT RESULTS_COLLECTION_TIMEOUT)
    set(RESULTS_COLLECTION_TIMEOUT 3600) # 1 hour.
  endif(NOT RESULTS_COLLECTION_TIMEOUT)

  set_tests_properties (ScidbInstall PROPERTIES TIMEOUT ${INSTALL_TIMEOUT})
  set_tests_properties (P4Install PROPERTIES TIMEOUT ${INSTALL_TIMEOUT})
  set_tests_properties (NamespaceInstall PROPERTIES TIMEOUT ${INSTALL_TIMEOUT})
  set_tests_properties (ScidbTest PROPERTIES TIMEOUT ${HARNESS_TEST_TIMEOUT})
  set_tests_properties (P4Test PROPERTIES TIMEOUT ${HARNESS_TEST_TIMEOUT})
  set_tests_properties (NamespaceTest PROPERTIES TIMEOUT ${HARNESS_TEST_TIMEOUT})
  set_tests_properties (ScidbResult PROPERTIES TIMEOUT ${RESULTS_COLLECTION_TIMEOUT})
  set_tests_properties (P4Result PROPERTIES TIMEOUT ${RESULTS_COLLECTION_TIMEOUT})
  set_tests_properties (NamespaceResult PROPERTIES TIMEOUT ${RESULTS_COLLECTION_TIMEOUT})
endif()
#
# Custom uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
#
# all outputs must go to CMAKE_BINARY_DIR to support out-of-tree build and test
#
# TODO: these should be done at make time, not cmake time, so a developer need
#       not do a full rebuild when only changing a test file.
#       (HINT add CMakeLists.txt files, with targets with ADD_CUSTOM_COMMAND()s intead of execute_process()
#
# COPY TESTS
#
message(STATUS "copying ${CMAKE_SOURCE_DIR}/tests to ${CMAKE_BINARY_DIR}/tests")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests
                                                           ${CMAKE_BINARY_DIR}/tests)

set(ignoreMe "${RUN_PY}")
if (DEFINED RUN_PY)
  # Remove the actual test cases and expected results from the build area:
  # they will be accessed directly from the source tree.
  message(STATUS "removing ${CMAKE_SOURCE_DIR}/tests/harness/testcases/t from ${CMAKE_BINARY_DIR}/tests/harness/testcases/t")
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests/harness/testcases/t)
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests/harness/testcases/data)
endif (DEFINED RUN_PY)

#
# COPY VERSION
#
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/version
                                                 ${CMAKE_BINARY_DIR}/version)

#
# INSTALL
#
include(install.cmake)

message(STATUS "****************** END CMakeLists.txt ******************")
