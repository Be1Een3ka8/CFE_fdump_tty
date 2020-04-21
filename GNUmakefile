# GNU Makefile, tested on Ubuntu.

include config.mk

SHELL=/bin/sh
MAKE=make
CXX=g++
CXX_OPTIMIZATIONS_FLAG=-Os
CXX_DEFINES_GNU=-DLINUX

PWD_SHOW=@echo -e `echo 'In: '; pwd`
MKDIR_P=mkdir -p

# GNU string replacement:
OBJ_DEBUG=$(patsubst %,$(ODIR)/%,$(DEBUG_NAME)/$(_OBJ))
OBJ_RELEASE=$(patsubst %,$(ODIR)/%,$(_OBJ))

all:
	##########################################################
	##########################################################
	@echo "Entering GNU Makefile."
	@$(PWD_SHOW)
	@${MAKE} -f GNUmakefile Release

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
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_GNU)
	@echo "Link complete."

$(ODIR)/$(DEBUG_NAME)/%.o: $(SDIR)/%.cpp
	@echo "d1. Compile and output objects."
	@$(PWD_SHOW)
	$(shell mkdir -p obj)\
	$(shell mkdir -p obj/$(DEBUG_NAME))\
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(DEBUG_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_GNU)

# Release build chain:
$(APP_NAME): $(OBJ_RELEASE)
	@echo "r2. Link objects into executable/shared library."
	@$(PWD_SHOW)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(RELEASE_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_GNU)
	@echo "Link complete."

$(ODIR)/%.o: $(SDIR)/%.cpp
	@echo "r1. Compile and output objects."
	@$(PWD_SHOW)
	$(shell mkdir -p obj)\
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_GNU)

# Clean toolchain:
cleanDebug:
	@$(PWD_SHOW)
	- rm -f $(APP_NAME).$(DEBUG_NAME);
	- rm -f $(ODIR)/$(DEBUG_NAME)/*.o *~ core $(INCDIR)/*~ ;
	- rm -rf $(ODIR)/$(DEBUG_NAME)
	@echo "Debug objects cleaned."

cleanRelease:
	@$(PWD_SHOW)
	- rm -f $(APP_NAME);
	- rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ ;
	@echo "Release objects cleaned."

clean: cleanRelease cleanDebug
	@$(PWD_SHOW)
	- rm -rf $(ODIR)
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
