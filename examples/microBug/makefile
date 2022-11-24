WHERE_IS_MKF := ${abspath ${lastword ${MAKEFILE_LIST}}}
WHERE_IS_DIR := ${dir ${WHERE_IS_MKF}}

FLAG_COMMON := -v -O0 -gdwarf-4
CMD_NORM := gcc ${WHERE_IS_DIR}./microBug.c ${FLAG_COMMON}

ASAN_STATIC := -static-libasan
ASAN_DYNAMC := -ldl
CMD_ASAN := ${CMD_NORM} -fsanitize=address

LOC_OUTPUT := ${WHERE_IS_DIR}./build/microBug-

asan-static:
	${CMD_ASAN} ${ASAN_STATIC} -o ${LOC_OUTPUT}asan-static
asan-dynamic:
	${CMD_ASAN} ${ASAN_DYNAMC} -o ${LOC_OUTPUT}asan-dynamic

normal:
	${CMD_NORM} -o ${LOC_OUTPUT}normal

all: normal  asan-static  asan-dynamic

clean:
	rm ${WHERE_IS_DIR}./build/microBug-*

.PHONY: clean  all  normal  asan-static  asan-dynamic