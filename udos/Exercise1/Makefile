ifeq ($(SCIDB),) 
  X := $(shell which scidb 2>/dev/null)
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
      -I"$(SCIDB)/include" -I"/home/jh/scidbsource19.3/include" -I"/home/jh/scidbsource19.3/src" -I./extern

LIBS = -shared -Wl,-soname,libExercise1.so -ldl -L. \
       -L"$(SCIDB_THIRDPARTY_PREFIX)/3rdparty/boost/lib" -L"$(SCIDB)/lib" \
       -Wl,-rpath,$(SCIDB)/lib:$(RPATH)

SRCS = Exercise1.cpp \
       LogicalExercise1.cpp \
       PhysicalExercise1.cpp 


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

all: libExercise1.so

clean:
	rm -rf *.so *.o

libExercise1.so: $(SRCS)
	@if test ! -d "$(SCIDB)"; then echo  "Error. Try:\n\nmake SCIDB=<PATH TO SCIDB INSTALL PATH>"; exit 1; fi
	$(CXX) $(CCFLAGS) $(INC) -o Exercise1.o -c Exercise1.cpp
	$(CXX) $(CCFLAGS) $(INC) -o LogicalExercise1.o -c LogicalExercise1.cpp
	$(CXX) $(CCFLAGS) $(INC) -o PhysicalExercise1.o -c PhysicalExercise1.cpp
	$(CXX) $(CCFLAGS) $(INC) -o libExercise1.so plugin.cpp Exercise1.o LogicalExercise1.o PhysicalExercise1.o $(LIBS)
	cp libExercise1.so /home/otter/Desktop/scidb_plugins
	#scp libExercise1.so scidb@master1.korea.ac.kr:~
	#scp libExercise1.so scidb@master2.korea.ac.kr:~
