LOC_MKF := ${abspath ${lastword ${MAKEFILE_LIST}}}
LOC_DIR := ${dir ${LOC_MKF}}
LOC_OUT := ${LOC_DIR}build/

FLAG_COMMON := -O0
FLAG_DBG    := -gdwarf-4
FLAG_X86    := -m32

C_CAL := -c -o ${LOC_OUT}calls.o    ${LOC_DIR}calls.c
C_BUG := -c -o ${LOC_OUT}microBug.o ${LOC_DIR}microBug.cpp
L_ALL := ${LOC_OUT}calls.o ${LOC_OUT}microBug.o -o ${LOC_OUT}

HOST_ARCH := $(shell  uname -m)
ifneq ($(HOST_ARCH),x86_64)
  ifneq ($(HOST_ARCH),amd64)
    $(warning Not running on intel64 so suffix with -32 equals to -64)
  endif
endif

CC  := gcc
CXX := g++

ASAN_STC_F := -fsanitize=address -static-libasan

ASAN_DYN_CF := -fsanitize=address
ASAN_DYN_LF := -fsanitize=address -ldl

all:
	$(MAKE) san0-dbg0-32     
	$(MAKE) san0-dbg1-32     
	$(MAKE) san0-dbg0-64     
	$(MAKE) san0-dbg1-64     
	$(MAKE) san1-dyn0-dbg0-32
	$(MAKE) san1-dyn0-dbg0-64
	$(MAKE) san1-dyn0-dbg1-32
	$(MAKE) san1-dyn0-dbg1-64
	$(MAKE) san1-dyn1-dbg0-32
	$(MAKE) san1-dyn1-dbg0-64
	$(MAKE) san1-dyn1-dbg1-32
	$(MAKE) san1-dyn1-dbg1-64
	$(MAKE) remove-object
	@echo "===== All done ====="

#########################
san0-dbg0-32: remove-object
	$(eval XF := ${FLAG_COMMON} ${FLAG_X86})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@

san0-dbg1-32: remove-object
	$(eval XF := ${FLAG_COMMON} ${FLAG_X86} ${FLAG_DBG})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@

san0-dbg0-64: remove-object
	$(eval XF := ${FLAG_COMMON})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@

san0-dbg1-64: remove-object
	$(eval XF := ${FLAG_COMMON} ${FLAG_DBG})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@
#########################

#########################
san1-dyn0-dbg0-32: remove-object
	$(eval XF := ${FLAG_COMMON} ${ASAN_STC_F} ${FLAG_X86})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@

san1-dyn0-dbg0-64: remove-object
	$(eval XF := ${FLAG_COMMON} ${ASAN_STC_F})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@

san1-dyn0-dbg1-32: remove-object
	$(eval XF := ${FLAG_COMMON} ${ASAN_STC_F} ${FLAG_DBG} ${FLAG_X86})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@

san1-dyn0-dbg1-64: remove-object
	$(eval XF := ${FLAG_COMMON} ${ASAN_STC_F} ${FLAG_DBG})
	${CC}  ${XF} ${C_CAL}
	${CXX} ${XF} ${C_BUG}
	${CXX} ${XF} ${L_ALL}bug-$@

san1-dyn1-dbg0-32: remove-object
	$(eval CF := ${FLAG_COMMON} ${ASAN_DYN_CF} ${FLAG_X86})
	$(eval LF := ${FLAG_COMMON} ${ASAN_DYN_LF} ${FLAG_X86})
	${CC}  ${CF} ${C_CAL}
	${CXX} ${CF} ${C_BUG}
	${CXX} ${LF} ${L_ALL}bug-$@

san1-dyn1-dbg0-64: remove-object
	$(eval CF := ${FLAG_COMMON} ${ASAN_DYN_CF})
	$(eval LF := ${FLAG_COMMON} ${ASAN_DYN_LF})
	${CC}  ${CF} ${C_CAL}
	${CXX} ${CF} ${C_BUG}
	${CXX} ${LF} ${L_ALL}bug-$@

san1-dyn1-dbg1-32: remove-object
	$(eval CF := ${FLAG_COMMON} ${ASAN_DYN_CF} ${FLAG_DBG} ${FLAG_X86})
	$(eval LF := ${FLAG_COMMON} ${ASAN_DYN_LF} ${FLAG_DBG} ${FLAG_X86})
	${CC}  ${CF} ${C_CAL}
	${CXX} ${CF} ${C_BUG}
	${CXX} ${LF} ${L_ALL}bug-$@

san1-dyn1-dbg1-64: remove-object
	$(eval CF := ${FLAG_COMMON} ${ASAN_DYN_CF} ${FLAG_DBG})
	$(eval LF := ${FLAG_COMMON} ${ASAN_DYN_LF} ${FLAG_DBG})
	${CC}  ${CF} ${C_CAL}
	${CXX} ${CF} ${C_BUG}
	${CXX} ${LF} ${L_ALL}bug-$@
#########################

remove-object:
	rm -f ${LOC_DIR}build/*.o

clean:
	rm -f ${LOC_DIR}build/bug-*

.PHONY: clean  all  remove-object