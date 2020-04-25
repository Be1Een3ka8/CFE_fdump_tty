# GNU Makefile, tested on Ubuntu. Author Gerallt Franke.
# Date: 25 April 2020 13:29 UTC. 

include config.mk

SHELL=/bin/sh
MAKE=make
CXX=g++
CXX_DEFINES_GNU=-D POSIX -D LINUX
CXX_OPTIMIZATIONS_FLAG=-Os

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
	${CXX} ${SOURCES} -o ${APP_NAME} $(CXXFLAGS) $(CXX_INCLUDES) $(CXX_DEFINES_GNU)
	@echo "Built simple target."

Debug: $(APP_NAME).$(DEBUG_NAME)
	@echo "Built target on Debug."

Release: $(APP_NAME)
	@echo "Built target on Release."

# Debug build chain:
$(APP_NAME).$(DEBUG_NAME): $(OBJ_DEBUG)
	@echo "d2. Linking objects into executable/shared library."
	@$(PWD_SHOW)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_GNU)
	@echo "Link complete."

$(ODIR)/$(DEBUG_NAME)/%.o: $(SDIR)/%.cpp
	@echo "d1. Compile and output objects."
	@$(PWD_SHOW)
	$(shell $(MKDIR_P) obj)\
	$(shell $(MKDIR_P) obj/$(DEBUG_NAME))\
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(DEBUG_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_GNU)

# Release build chain:
$(APP_NAME): $(OBJ_RELEASE)
	@echo "r2. Linking objects into executable/shared library."
	@$(PWD_SHOW)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(RELEASE_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_GNU)
	@echo "Link complete."

$(ODIR)/%.o: $(SDIR)/%.cpp
	@echo "r1. Compile and output objects."
	@$(PWD_SHOW)
	$(shell $(MKDIR_P) obj)\
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
	@echo "CXX_DEFINES    = ${CXX_DEFINES} $(CXX_DEFINES_GNU)"
	@echo "CXX_INCLUDES    = ${CXX_INCLUDES}"
	@echo "CXX_LIBRARIES    = ${CXX_LIBRARIES}"
	@echo "CXX        = ${CXX}"
	@echo "MAKE        = ${MAKE}"
	@echo "SHELL        = ${SHELL}"

help:
	@echo "Valid targets are: 'all', 'simple', 'Debug', 'Release', 'clean', 'cleanDebug', 'cleanRelease', 'options', 'help'"

.PHONY: Debug Release all simple clean cleanDebug cleanRelease options help
