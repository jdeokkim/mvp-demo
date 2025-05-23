#
# Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# =============================================================================

.PHONY: all clean rebuild
.SUFFIXES: .c .exe .html .out

# =============================================================================

_COLOR_BEGIN = \033[1;38;5;219m
_COLOR_END = \033[m

# =============================================================================

PROJECT_NAME = mvp-demo
PROJECT_FULL_NAME = jdeokkim/mvp-demo

LOG_PREFIX = ${_COLOR_BEGIN} ~>${_COLOR_END}

# =============================================================================

BINARY_PATH = bin
INCLUDE_PATH = include
LIBRARY_PATH = lib
RESOURCE_PATH = res
SOURCE_PATH = src

OBJECTS = \
	${SOURCE_PATH}/clip.o   \
	${SOURCE_PATH}/game.o   \
	${SOURCE_PATH}/local.o  \
	${SOURCE_PATH}/utils.o  \
	${SOURCE_PATH}/view.o   \
	${SOURCE_PATH}/world.o  \
	${SOURCE_PATH}/main.o   

TARGET_SUFFIX = out

TARGETS = \
	${BINARY_PATH}/${PROJECT_NAME}.${TARGET_SUFFIX}

# =============================================================================

CC = cc
CFLAGS = -D_DEFAULT_SOURCE -g -I${INCLUDE_PATH} -I${INCLUDE_PATH}/raylib \
	-I${INCLUDE_PATH}/raygui -I${RESOURCE_PATH} -O2 -std=gnu99
LDLIBS = -lraylib -ldl -lGL -lglfw -lm -lpthread -lrt -lX11

CFLAGS += -Wno-unused-result

# =============================================================================

all: pre-build build post-build

pre-build:
	@printf "${LOG_PREFIX} CC = ${CC}, MAKE = ${MAKE}\n"

build: ${TARGETS}

.c.o:
	@printf "${LOG_PREFIX} Compiling: $@ (from $<)\n"
	@${CC} -c $< -o $@ ${CFLAGS}

${TARGETS}: ${OBJECTS}
	@mkdir -p ${BINARY_PATH}
	@printf "${LOG_PREFIX} Linking: ${TARGETS}\n"
	@${CC} ${OBJECTS} -o ${TARGETS} ${LDFLAGS} ${LDLIBS} ${WEBFLAGS}

post-build:
	@printf "${LOG_PREFIX} Build complete.\n"

# =============================================================================

rebuild: clean all

# =============================================================================

clean:
	@printf "${LOG_PREFIX} Cleaning up.\n"
	@rm -f ${BINARY_PATH}/*.data ${BINARY_PATH}/*.exe ${BINARY_PATH}/*.html \
		${BINARY_PATH}/*.js ${BINARY_PATH}/*.out ${BINARY_PATH}/*.wasm \
		${SOURCE_PATH}/*.o

# =============================================================================
