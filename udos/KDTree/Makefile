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

OPFLAGS=-O3

all:
	$(CXX) ${CPPFLAGS} -o main src/main.cpp

clean:
	rm -f main