BUILDDIR ?= build
CFG      ?= debug
NAME     ?= vhagar
SRCDIR   ?= src

all:

-include config/$(CFG).cfg

# Be verbose about the build.
ifneq ($(OS),Windows_NT)
	Q ?= @
endif

BINDIR := $(BUILDDIR)/$(CFG)
BIN    := $(BINDIR)/$(NAME)
SRC    := $(sort $(wildcard $(SRCDIR)/*.cpp))
OBJ    := $(SRC:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)
DEP    := $(OBJ:%.o=%.d)

# Try to locate llvm-config, a tool that produces command line flags for the
# build process.
ifneq ("$(wildcard llvm/install/bin/llvm-config)","")
	LLVM_CONFIG  ?= llvm/install/bin/llvm-config
else
	LLVM_CONFIG  ?= llvm-config
endif

# Obtain the LLVM build flags if we found llvm-config.
ifeq (,$(wildcard $(LLVM_CONFIG)))
	LLVM_CFLAGS  :=
	LLVM_LDFLAGS :=
else
	LLVM_CFLAGS  := $(shell $(LLVM_CONFIG) --cppflags)
	LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --libs --system-libs)
endif

ifeq ($(OS),Windows_NT)
    RM = del /Q
    MKDIR = mkdir
    RMDIR = RMDIR /S /Q
    EXT = .exe
else
    RM = rm -fr
    MKDIR = mkdir -p
    RMDIR = rm -rf
    EXT =
endif

CFLAGS   := $(LLVM_CFLAGS) $(CFLAGS)
CXXFLAGS += $(CFLAGS) -std=c++17
LDFLAGS  += $(LLVM_LDFLAGS)

# use the mold linker if available
ifneq ("$(shell command -v mold)","")
	LDFLAGS += -fuse-ld=mold
endif

# does not work on windows
ifneq ($(OS),Windows_NT)
	DUMMY := $(shell $(MKDIR) $(sort $(dir $(OBJ))))
endif

.PHONY: all clean format

all: $(BIN) format

-include $(DEP)

clean:
	@echo "===> CLEAN"
	$(Q)$(RMDIR) "$(BINDIR)"
	$(Q)$(RM) *.ll ./*.o *.s a.out

$(BIN): $(OBJ)
	@echo "===> LD $@"
	$(Q)$(CXX) -o $(BIN)$(EXT) $(OBJ) $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "===> CXX $<"
	$(Q)$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

format:
	clang-format -i $(SRCDIR)/*.cpp $(SRCDIR)/*.h

llc: main.ll
	./llvm/build/bin/llc $< --filetype=asm --asm-verbose --x86-asm-syntax=intel
