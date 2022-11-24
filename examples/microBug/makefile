WHERE_IS_MKF := ${abspath ${lastword ${MAKEFILE_LIST}}}
WHERE_IS_DIR := ${dir ${WHERE_IS_MKF}}

FLAG_COMMON := -v -O0 -gdwarf-4
CMD_NORM := gcc ${WHERE_IS_DIR}./microBug.c ${FLAG_COMMON}

ASAN_STATIC := -static-libasan
ASAN_DYNAMC := -ldl
CMD_ASAN := ${CMD_NORM} -fsanitize=address

LOC_OUTPUT := ${WHERE_IS_DIR}./build/microBug-

all: normal  asan-static  asan-dynamic  normal-32  asan-static-32  asan-dynamic-32

asan-static:
	${CMD_ASAN} ${ASAN_STATIC} -o ${LOC_OUTPUT}asan-static
asan-static-32:
	${CMD_ASAN} ${ASAN_STATIC} -m32 -o ${LOC_OUTPUT}asan-static-32

asan-dynamic:
	${CMD_ASAN} ${ASAN_DYNAMC} -o ${LOC_OUTPUT}asan-dynamic
asan-dynamic-32:
	${CMD_ASAN} ${ASAN_DYNAMC} -m32 -o ${LOC_OUTPUT}asan-dynamic-32

normal:
	${CMD_NORM} -o ${LOC_OUTPUT}normal
normal-32:
	${CMD_NORM} -m32 -o ${LOC_OUTPUT}normal-32

clean:
	rm ${WHERE_IS_DIR}./build/microBug-*

.PHONY: clean  all  normal  asan-static  asan-dynamic  normal-32  asan-static-32  asan-dynamic-32