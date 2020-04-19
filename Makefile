IDIR=./
ODIR=./obj
SDIR=./

APP_NAME=fdump
DEBUG_DIR=./
RELEASE_DIR=./
DEBUG_NAME=d

RELEASE_FLAGS=-s
DEBUG_FLAGS=-g
CXX=g++
CXXFLAGS=-std=c++17 -Wall -pedantic
#CXXFLAGS=
CXX_DEFINES=-DLINUX
CXX_INCLUDES=-I$(IDIR)
CXX_LIBRARIES= 
LIBS=

_OBJ=fdump.o

OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
OBJ_DEBUG=$(patsubst %,$(ODIR)/%,$(DEBUG_NAME)/$(_OBJ))
OBJ_RELEASE=$(patsubst %,$(ODIR)/%,$(_OBJ))

all: Release
	# Target 'all'.

Debug: $(APP_NAME).$(DEBUG_NAME)
	# Built target on Debug.

Release: $(APP_NAME)
	# Built target on Release.

# Debug build chain:
$(APP_NAME).$(DEBUG_NAME): $(OBJ_DEBUG)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES)
	# d2.

$(ODIR)/$(DEBUG_NAME)/%.o: $(SDIR)/%.cpp
	$(shell mkdir -p obj)\
	$(shell mkdir -p obj/$(DEBUG_NAME))\
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(DEBUG_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES) 
	# d1.

# Release build chain:
$(APP_NAME): $(OBJ_RELEASE)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(RELEASE_FLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES)
	# r2.

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(shell mkdir -p obj)\
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LIBS) $(CXX_INCLUDES) $(CXX_LIBRARIES) $(CXX_DEFINES)
	# r1.

.PHONY: Debug Release all clean cleanDebug cleanRelease

cleanDebug:
	rm -f $(APP_NAME).$(DEBUG_NAME);
	rm -f $(ODIR)/$(DEBUG_NAME)/*.o *~ core $(INCDIR)/*~ ;
	rm -rf $(ODIR)/$(DEBUG_NAME)
	# Debug objects cleaned.

cleanRelease:
	rm -f $(APP_NAME);
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ ;
	# Release objects cleaned.

clean: cleanRelease cleanDebug
	rm -rf $(ODIR)
	# All objects cleaned.