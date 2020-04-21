# BSD makefile, tested on OpenBSD & NetBSD.

include config.mk

SHELL=/bin/sh
MAKE=make
CXX=c++
CXX_OPTIMIZATIONS_FLAG_CLANG=-Oz
CXX_OPTIMIZATIONS_FLAG_GCC=-Os
CXX_OPTIMIZATIONS_FLAG=

PWD_SHOW=@echo -e `echo 'In: '; pwd`
MKDIR_P=mkdir -p

#all: Release
#	@echo "Target 'all'."

all:
	##########################################################
	##########################################################
	@echo "Entering BSD Makefile."
	@$(PWD_SHOW)
	@${MAKE} -f BSDmakefile Release

# Simple test build:  run: $ make simple
simple:
	${CXX} -o fdump fdump.cpp $(CXX_INCLUDES)
	@echo "Built simple target."

Debug: $(APP_NAME).$(DEBUG_NAME)
	@echo "Built target on Debug."

Release: $(APP_NAME)
	@echo "Built target on Release."

# Debug build chain:
$(APP_NAME).$(DEBUG_NAME): $(OBJ_DEBUG)
	@echo "d2. Link objects into executable/shared library."
	@$(PWD_SHOW)
	$(CXX) -o $@ ${OBJ_DEBUG} $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(RELEASE_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES)
	@echo "Link complete."

# GNU pattern rules don't work in POSIX
$(ODIR)/$(DEBUG_NAME)/fdump.o: $(SOURCES)
	@echo "d1. Compile and output objects."
	@$(PWD_SHOW)
	@$(MKDIR_P) obj
	@$(MKDIR_P) obj/$(DEBUG_NAME)
	$(CXX) -c -o $@ ${SOURCES} $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES)

# Release build chain:
$(APP_NAME): $(OBJ_RELEASE)
	@echo "r2. Link objects into executable/shared library."
	@$(PWD_SHOW)
	$(CXX) -o $@ ${OBJ_RELEASE} $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(RELEASE_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES)
	@echo "Link complete."

# GNU pattern rules don't work in POSIX
$(ODIR)/fdump.o: $(SOURCES)
	@echo "r1. Compile and output objects."
	@$(PWD_SHOW)
	@$(MKDIR_P) obj
	$(CXX) -c -o $@ ${SOURCES} $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES)

# Clean toolchain:
# BSD make always chdir's which is annoying hence ../ 
cleanDebug:
	@$(PWD_SHOW)
	- rm -f ../$(APP_NAME).$(DEBUG_NAME);
	- rm -f ../$(ODIR)/$(DEBUG_NAME)/*.o *~ core $(INCDIR)/*~ ;
	- rm -rf ../$(ODIR)/$(DEBUG_NAME)
	@echo "Debug objects cleaned."

cleanRelease:
	@$(PWD_SHOW)
	- rm -f ../$(APP_NAME);
	- rm -f ../$(ODIR)/*.o *~ core ../$(INCDIR)/*~ ;
	@echo "Release objects cleaned."

clean: cleanRelease cleanDebug
	@$(PWD_SHOW)
	- rm -rf ../$(ODIR)
	@echo "All objects cleaned."

options:
	@echo fdump build options:
	@echo "CXXFLAGS   = ${CXXFLAGS}"
	@echo "CXX_DEFINES    = ${CXX_DEFINES}"
	@echo "CXX_INCLUDES    = ${CXX_INCLUDES}"
	@echo "CXX_LIBRARIES    = ${CXX_LIBRARIES}"
	@echo "CXX        = ${CXX}"
	@echo "MAKE        = ${MAKE}"
	@echo "SHELL        = ${SHELL}"

help:
	@echo "Valid targets are: 'all', 'simple', 'Debug', 'Release', 'clean', 'cleanDebug', 'cleanRelease', 'options', 'help'"

.PHONY: Debug Release all simple clean cleanDebug cleanRelease options help
