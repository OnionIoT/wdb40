
# find the os
UNAME_S := $(shell uname -s)

# main compiler
CXX := g++
# CXX := clang --analyze # and comment out the linker last line for sanity

# define the directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin
LIBDIR := lib

# define common variables
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -iname "*.$(SRCEXT)" )
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CXXFLAGS := -g # -Wall

INC := $(shell find $(INCDIR) -maxdepth 1 -type d -exec echo -I {}  \;)
#LIB := $(LIB) -L$(LIBDIR) -loniondebug -lonioni2c

ifeq ($(UNAME_S),Darwin)
	# only add this when compiling on OS X
	INC += -I $(LIBDIR)
endif

# define specific binaries to create
APP0 := wdb40
TARGET := $(BINDIR)/$(APP0)


all: info $(TARGET)

$(TARGET): $(OBJECTS) 
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(LIB)

# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@echo "building $@"
	$(CXX) $< $(CXXFLAGS) $(INC) -c -o $@ $(LIB)

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(BINDIR) $(LIB0_TARGET)

info:
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIB: $(LIB)"
	@echo "INC: $(INC)"
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"


# Tests
tester:
	$(CXX) $(CXXFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# Spikes
#ticket:
#  $(CXX) $(CXXFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
