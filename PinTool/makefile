##### NECESSARY PATHS #####

## Since the pintool is built out of the kit, locate where we are firstly.

WHERE_IS_MKF := $(abspath $(lastword $(MAKEFILE_LIST)))
WHERE_IS_DIR := $(dir $(WHERE_IS_MKF))

## Set output directory

DIR_SRC := $(WHERE_IS_DIR)src
DIR_OUT := $(WHERE_IS_DIR)build
DIR_OUT_64 := $(DIR_OUT)/intel64
DIR_OUT_32 := $(DIR_OUT)/ia32

TNAME_64 := TracerCore64
TNAME_32 := TracerCore32

ifneq (,$(and $(filter $(TNAME_64),$(MAKECMDGOALS)),$(filter $(TNAME_32),$(MAKECMDGOALS))))
  $(error Only one of $(TNAME_64) and $(TNAME_32) can be compiled at one call)
endif

## Use `override` to force them not to be overwritten
## Ref:
## [6.7 The override Directive](https://www.gnu.org/software/make/manual/make.html#Override-Directive)

ifneq (,$(filter $(TNAME_64),$(MAKECMDGOALS)))
  override OBJDIR := $(DIR_OUT_64)/
  override TARGET := intel64
endif
ifneq (,$(filter $(TNAME_32),$(MAKECMDGOALS)))
  override OBJDIR := $(DIR_OUT_32)/
  override TARGET := ia32
endif

## Read `KIT_DIR_TREE` to locate Pin Kit

PINKIT_PTR := $(abspath $(WHERE_IS_DIR)../PinKit/KIT_DIR_TREE)
ifeq (,$(wildcard $(PINKIT_PTR)))
  $(error missing $(PINKIT_PTR))
endif

PIN_ROOT := $(abspath $(WHERE_IS_DIR)../PinKit/$(file < $(PINKIT_PTR)))
ifeq (,$(wildcard $(PIN_ROOT)))
  $(error missing $(PIN_ROOT))
endif

CONFIG_ROOT := $(PIN_ROOT)/source/tools/Config
ifeq (,$(wildcard $(CONFIG_ROOT)))
  $(error missing $(CONFIG_ROOT))
endif


##### NECESSARY INCLUDE #####

## Position of this include cannot be changed.
## Ref: 
## pintool/docs/98650/Pin/doc/html/index.html#ConfigDirectory
include $(CONFIG_ROOT)/makefile.config


##### CORE RULES BELOW #####

## File management

DIR64:
	mkdir -p $(DIR_OUT_64)
DIR32:
	mkdir -p $(DIR_OUT_32)
reset:
	$(shell set -e; rm -rf $(DIR_OUT)/*;)
	@echo "============== RESET =============="
	@echo " Things in build have been flushed "
	@echo "==================================="

## All intermediate targets

$(OBJDIR)checker$(OBJ_SUFFIX): $(DIR_SRC)/checker.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

$(OBJDIR)cli$(OBJ_SUFFIX): $(DIR_SRC)/cli.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

$(OBJDIR)payload$(OBJ_SUFFIX): $(DIR_SRC)/payload.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

$(OBJDIR)TracerCore$(OBJ_SUFFIX): $(DIR_SRC)/TracerCore.cpp
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$@ $<

## Target of the tool itself

$(OBJDIR)TracerCore$(PINTOOL_SUFFIX):   $(OBJDIR)cli$(OBJ_SUFFIX)       \
                                        $(OBJDIR)checker$(OBJ_SUFFIX)   \
                                        $(OBJDIR)payload$(OBJ_SUFFIX)   \
                                        $(OBJDIR)TracerCore$(OBJ_SUFFIX)
	$(LINKER) $(TOOL_LDFLAGS) $(LINK_EXE)$@ $+ $(TOOL_LPATHS) $(TOOL_LIBS)
	@echo "=========================== WELCOME ==========================="
	@echo "Thank you for using Igor's powerful"
	@echo "'########:'########:::::'###:::::'######::'########:'########::"
	@echo "... ##..:: ##.... ##:::'## ##:::'##... ##: ##.....:: ##.... ##:"
	@echo "::: ##:::: ##:::: ##::'##:. ##:: ##:::..:: ##::::::: ##:::: ##:"
	@echo "::: ##:::: ########::'##:::. ##: ##::::::: ######::: ########::"
	@echo "::: ##:::: ##.. ##::: #########: ##::::::: ##...:::: ##.. ##:::"
	@echo "::: ##:::: ##::. ##:: ##.... ##: ##::: ##: ##::::::: ##::. ##::"
	@echo "::: ##:::: ##:::. ##: ##:::: ##:. ######:: ########: ##:::. ##:"
	@echo ":::..:::::..:::::..::..:::::..:::......:::........::..:::::..::"
	@echo "PinTool is located at $(OBJDIR)TracerCore$(PINTOOL_SUFFIX)"
	@echo "==============================================================="

## Final targets

$(TNAME_64): DIR64 | $(OBJDIR)TracerCore$(PINTOOL_SUFFIX)
$(TNAME_32): DIR32 | $(OBJDIR)TracerCore$(PINTOOL_SUFFIX)

## Summary

AVAILABLE_TARGETS := $(TNAME_64) $(TNAME_32) reset DIR64 DIR32

.PHONY: $(AVAILABLE_TARGETS)

## Override targets introduced by makefile.config

all:
	$(error Available targets: [ $(AVAILABLE_TARGETS) ])
dir:
	$(error Available targets: [ $(AVAILABLE_TARGETS) ])