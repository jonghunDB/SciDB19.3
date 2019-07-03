ifeq ($(SCIDB),) 
  X := $(shell which scikDdb 2>/dev/null)
  ifneq ($(X),)
    X := $(shell dirname ${X})
    SCIDB := $(shell dirname ${X})
  endif
  $(info SciDB installed at $(SCIDB))
endif

# A development environment will have SCIDB_VER defined, and SCIDB
# will not be in the same place... but the 3rd party directory *will*
# be, so build it using SCIDB_VER .
ifeq ($(SCIDB_VER),)
  SCIDB_3RDPARTY = $(SCIDB)
else
  SCIDB_3RDPARTY = /opt/scidb/$(SCIDB_VER)
endif

# A better way to set the 3rdparty prefix path that does not assume an
# absolute path...
ifeq ($(SCIDB_THIRDPARTY_PREFIX),)
  SCIDB_THIRDPARTY_PREFIX := $(SCIDB_3RDPARTY)
endif

INSTALL_DIR = $(SCIDB)/lib/scidb/plugins

# Include the OPTIMIZED flags for non-debug use
OPTIMIZED=-O3 -DNDEBUG -ggdb3 -g
DEBUG=-g -ggdb3
CCFLAGS = -pedantic -W -Wextra -Wall -Wno-variadic-macros -Wno-strict-aliasing \
         -Wno-long-long -Wno-unused-parameter -Wno-unused -fPIC $(OPTIMIZED) -fno-omit-frame-pointer


INC = -I. -DPROJECT_ROOT="\"$(SCIDB)\"" -I"$(SCIDB_THIRDPARTY_PREFIX)/3rdparty/boost/include/" \
      -I"$(SCIDB)/include" -I"/home/jh/scidbsource19.3/include" -I"/home/jh/scidbsource19.3/src"  -I./extern

LIBS = -shared -Wl,-soname,libUnitTestMemArray.so -ldl -L. \
       -L"$(SCIDB_THIRDPARTY_PREFIX)/3rdparty/boost/lib" -L"$(SCIDB)/lib" \
       -Wl,-rpath,$(SCIDB)/lib:$(RPATH)

SRCS = UnitTestMemArrayLogical.cpp \
       UnitTestMemArrayPhysical.cpp


# Compiler settings for SciDB version >= 15.7
ifneq ("$(wildcard /usr/bin/g++-4.9)","")
 CC := "/usr/bin/gcc-4.9"
 CXX := "/usr/bin/g++-4.9"
 CCFLAGS+=-std=c++14 -DCPP11

else
 ifneq ("$(wildcard /opt/rh/devtoolset-3/root/usr/bin/gcc)","")
  CC := "/opt/rh/devtoolset-3/root/usr/bin/gcc"
  CXX := "/opt/rh/devtoolset-3/root/usr/bin/g++"
  CCFLAGS+=-std=c++14 -DCPP11
 endif
endif

all: libUnitTestMemArray.so cpandrestartscidb

clean:
	rm -rf *.so *.o

libUnitTestMemArray.so: $(SRCS)
	@if test ! -d "$(SCIDB)"; then echo  "Error. Try:\n\nmake SCIDB=<PATH TO SCIDB INSTALL PATH>"; exit 1; fi
	$(CXX) $(CCFLAGS) $(INC) -o UnitTestMemArrayLogical.o -c UnitTestMemArrayLogical.cpp
	$(CXX) $(CCFLAGS) $(INC) -o UnitTestMemArrayPhysical.o -c UnitTestMemArrayPhysical.cpp
	$(CXX) $(CCFLAGS) $(INC) -o libUnitTestMemArray.so plugin.cpp UnitTestMemArrayLogical.o UnitTestMemArrayPhysical.o $(LIBS)
	@echo "Now copy libequi_join.so to $(INSTALL_DIR) on all your SciDB nodes, and restart SciDB."

cpandrestartscidb:
	echo "cp libUnitTestMemArray file"
	sudo cp libUnitTestMemArray.so /opt/scidb/19.3/lib/scidb/plugins
	scidbctl.py stop
	sleep 10
	scidbctl.py start
	iquery -aq "load_library('UnitTestMemArray')"

