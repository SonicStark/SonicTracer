##### The tool is built out of the kit, so locate necessary paths #####
WHERE_IS_MKF := $(abspath $(lastword $(MAKEFILE_LIST)))
WHERE_IS_DIR := $(dir $(WHERE_IS_MKF))

PINKIT_PTR := $(WHERE_IS_DIR)../PinKit/KIT_DIR_TREE
ifeq (,$(wildcard $(PINKIT_PTR)))
$(error missing $(PINKIT_PTR))
endif

DIR_SRC := $(WHERE_IS_DIR)./src
DIR_OUT := $(WHERE_IS_DIR)./build
DIR_OUT_64 := $(DIR_OUT)/intel64
DIR_OUT_32 := $(DIR_OUT)/ia32

TNAME_64 := TracerCore64
TNAME_32 := TracerCore32

ifneq (,$(and $(filter $(TNAME_64),$(MAKECMDGOALS)),$(filter $(TNAME_32),$(MAKECMDGOALS))))
$(error Only one of $(TNAME_64) and $(TNAME_32) can be compiled at one call)
endif

ifneq (,$(filter $(TNAME_64),$(MAKECMDGOALS)))
override OBJDIR := $(DIR_OUT_64)/
override TARGET := intel64
endif
ifneq (,$(filter $(TNAME_32),$(MAKECMDGOALS)))
override OBJDIR := $(DIR_OUT_32)/
override TARGET := ia32
endif

PIN_ROOT := $(WHERE_IS_DIR)../PinKit/$(file < $(PINKIT_PTR))
ifeq (,$(wildcard $(PIN_ROOT)))
$(error missing $(PIN_ROOT))
endif

CONFIG_ROOT := $(PIN_ROOT)/source/tools/Config
ifeq (,$(wildcard $(CONFIG_ROOT)))
$(error missing $(CONFIG_ROOT))
endif


##### Include necessary files #####
# Position of this include cannot be changed.
# Ref: 
# pintool/docs/98650/Pin/doc/html/index.html#ConfigDirectory
include $(CONFIG_ROOT)/makefile.config


################ DEFINE CUSTOM RULES BELOW ################

## file management

DIR64:
	mkdir -p $(DIR_OUT_64)
DIR32:
	mkdir -p $(DIR_OUT_32)
reset:
	$(shell set -e; rm -rf $(DIR_OUT)/*;)

## all intermediate targets

$(OBJDIR)checker$(OBJ_SUFFIX): $(DIR_SRC)/checker.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

$(OBJDIR)cli$(OBJ_SUFFIX): $(DIR_SRC)/cli.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

$(OBJDIR)payload$(OBJ_SUFFIX): $(DIR_SRC)/payload.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

$(OBJDIR)TracerCore$(OBJ_SUFFIX): $(DIR_SRC)/TracerCore.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

## build the tool itself

$(OBJDIR)TracerCore$(PINTOOL_SUFFIX): $(OBJDIR)%$(OBJ_SUFFIX)
	$(LINKER) $(TOOL_LDFLAGS_NOOPT) $(LINK_EXE)$@ $(TOOL_LPATHS) $(TOOL_LIBS)

## entrance
### Ref: [6.11 Target-specific Variable Values](https://www.gnu.org/software/make/manual/make.html#Target_002dspecific)

$(TNAME_64): DIR64 | $(OBJDIR)TracerCore$(PINTOOL_SUFFIX)
$(TNAME_32): DIR32 | $(OBJDIR)TracerCore$(PINTOOL_SUFFIX)

.PHONY: DIR64  DIR32  reset  $(TNAME_64)  $(TNAME_32)


################ DEFINE CUSTOM RULES ABOVE ################


##### Include necessary files #####
# Position of this include cannot be changed.
# Ref: 
# pintool/docs/98650/Pin/doc/html/index.html#ConfigDirectory
include $(TOOLS_ROOT)/Config/makefile.default.rules