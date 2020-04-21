# config.mk: Common configuration between different makefiles and their platforms.

IDIR=./
ODIR=./obj
SDIR=./

APP_NAME=fdump
DEBUG_DIR=./
RELEASE_DIR=./
DEBUG_NAME=d

RELEASE_FLAGS=-s
DEBUG_FLAGS=-g


#CXXFLAGS=-std=c++17 -Wall -pedantic $(SIZE_OPTIMIZATIONS_FLAG)
CXXFLAGS=-std=c++17
CXX_DEFINES=
CXX_INCLUDES=-I$(IDIR)
CXX_LIBRARIES= 
LIBS=

_OBJ=fdump.o

# GNU string replacement:
#OBJ_DEBUG=$(patsubst %,$(ODIR)/%,$(DEBUG_NAME)/$(_OBJ))
#OBJ_RELEASE=$(patsubst %,$(ODIR)/%,$(_OBJ))

# FreeBSD string replacement:
#SOURCES!= ls *.cpp # FreeBSD only. If sources end up nested, better to use recursive find instead of ls.
#OBJECTS = ${SOURCES:.cpp=.o} # This form of substitution only works in FreeBSD makefiles not GNU.
#OBJ_DEBUG = ${SOURCES:.cpp=d.*.o}
#OBJ_RELEASE = ${SOURCES:.cpp=.o}

# *nix generic string replacement:
SOURCES=$(ls *.cpp)
OBJECTS=$(echo ${SOURCES} | sed 's/.cpp/.o/g')
OBJ_DEBUG=
OBJ_RELEASE=

# Can grab headers: c++ -MM fdump.cpp

# remove trailing '.' and escape all '/' with '\/'
#__ODIR=$(echo $ODIR | sed 's/^.//g' | sed 's/[\/]/\\\//g')

# prepend new ./obj path to object filenames:
#__EXPR=$(echo 's/.*.o/\\'"${__ODIR}"'\/&/g')
#OBJ_DEBUG=$(echo ${OBJECTS} | sed ${__EXPR})
#OBJ_RELEASE=$(echo ${OBJECTS} | sed ${__EXPR})

# Fallback:
SOURCES=fdump.cpp
OBJ_DEBUG=$(ODIR)/$(DEBUG_NAME)/fdump.o
OBJ_RELEASE=$(ODIR)/fdump.o
