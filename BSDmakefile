# BSD makefile, tested on FreeBSD, OpenBSD, & NetBSD. Author Gerallt Franke.
# Date: 25 April 2020 13:29 UTC. 

include config.mk

SHELL=/bin/sh
MAKE=make
CXX=c++
CXX_DEFINES_BSD=-D POSIX -D BSD
CXX_OPTIMIZATIONS_FLAG_CLANG=-Oz
CXX_OPTIMIZATIONS_FLAG_GCC=-Os
CXX_OPTIMIZATIONS_FLAG=

PWD_SHOW=@echo -e `echo 'In: '; pwd`
MKDIR_P=mkdir -p

ENTRY_DIR != if [ "${DO_CHDIR}" = "Backward" ]; then echo '../'; else echo ''; fi
ENTRY_DIR != if [ "${DO_CHDIR}" = "None" ]; then echo ''; else echo '../'; fi

all:
	##########################################################
	##########################################################
	@echo "Entering BSD Makefile."
	@$(PWD_SHOW)
	@${MAKE} -f BSDmakefile Release

# Simple test build:  run: $ make simple
simple:
	${CXX} ${SOURCES} -o ${APP_NAME} $(CXXFLAGS) $(CXX_INCLUDES) $(CXX_DEFINES_BSD)
	@echo "Built simple target."

Debug: $(APP_NAME).$(DEBUG_NAME)
	@echo "Built target on Debug."

Release: $(APP_NAME)
	@echo "Built target on Release."

# Debug build chain:
$(APP_NAME).$(DEBUG_NAME): $(OBJ_DEBUG)
	@echo "d2. Linking objects into executable/shared library."
	@$(PWD_SHOW)
	$(CXX) -o $@ ${OBJ_DEBUG} $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(RELEASE_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_BSD)
	@echo "Link complete."

# GNU pattern rules don't work in POSIX
# Also ${.IMPSRC} is the same as $< which is a GNUmake idiom.
$(ODIR)/$(DEBUG_NAME)/fdump.o: $(SOURCES)
	@echo "d1. Compile and output objects."
	@$(PWD_SHOW)
	@$(MKDIR_P) obj
	@$(MKDIR_P) obj/$(DEBUG_NAME)
	$(CXX) -c fdump.cpp -o $@ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_BSD)

$(ODIR)/$(DEBUG_NAME)/uart_nix.o: $(SOURCES)
	@echo "d1. Compile and output objects."
	@$(PWD_SHOW)
	@$(MKDIR_P) obj
	@$(MKDIR_P) obj/$(DEBUG_NAME)
	$(CXX) -c uart_nix.cpp -o $@ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_BSD)

# Release build chain:
$(APP_NAME): ${OBJ_RELEASE}
	@echo "r2. Linking objects into executable/shared library."
	@$(PWD_SHOW)
	$(CXX) -o $@ ${OBJ_RELEASE} $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(RELEASE_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_BSD)
	@echo "Link complete."

# GNU pattern rules don't work in POSIX
# Also ${.IMPSRC} is the same as $< which is a GNUmake idiom.
$(ODIR)/fdump.o: $(SOURCES)
	@echo "r1. Compile and output objects."
	@$(PWD_SHOW)
	@$(MKDIR_P) obj
	$(CXX) -c fdump.cpp -o $@ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_BSD)

$(ODIR)/uart_nix.o: $(SOURCES)
	@echo "r1. Compile and output objects."
	@$(PWD_SHOW)
	@$(MKDIR_P) obj
	$(CXX) -c uart_nix.cpp -o $@ $(CXXFLAGS) $(CXX_OPTIMIZATIONS_FLAG) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) $(CXX_DEFINES_BSD)

# Clean toolchain:
# BSD make always chdir's which is annoying hence use of ${ENTRY_DIR} or ../
cleanDebug:
	@$(PWD_SHOW)
	- rm -f ${ENTRY_DIR}$(APP_NAME).$(DEBUG_NAME);
	- rm -f ${ENTRY_DIR}$(ODIR)/$(DEBUG_NAME)/*.o *~ core ${ENTRY_DIR}$(INCDIR)/*~ ;
	- rm -rf ${ENTRY_DIR}$(ODIR)/$(DEBUG_NAME)
	@echo "Debug objects cleaned."

cleanRelease:
	@$(PWD_SHOW)
	- rm -f ${ENTRY_DIR}$(APP_NAME);
	- rm -f ${ENTRY_DIR}$(ODIR)/*.o *~ core ../$(INCDIR)/*~ ;
	@echo "Release objects cleaned."

clean: cleanRelease cleanDebug
	@$(PWD_SHOW)
	- rm -rf ${ENTRY_DIR}$(ODIR)
	@echo "All objects cleaned."

options:
	@echo fdump build options:
	@echo "CXXFLAGS   = ${CXXFLAGS}"
	@echo "CXX_DEFINES    = ${CXX_DEFINES} $(CXX_DEFINES_BSD)"
	@echo "CXX_INCLUDES    = ${CXX_INCLUDES}"
	@echo "CXX_LIBRARIES    = ${CXX_LIBRARIES}"
	@echo "CXX        = ${CXX}"
	@echo "MAKE        = ${MAKE}"
	@echo "SHELL        = ${SHELL}"

help:
	@echo "Valid targets are: 'all', 'simple', 'Debug', 'Release', 'clean', 'cleanDebug', 'cleanRelease', 'options', 'help'"

.PHONY: Debug Release all simple clean cleanDebug cleanRelease options help

