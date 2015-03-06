LIBRARIES=libm libcad
ifeq "$(wildcard ../libcad)" ""
include /usr/share/libcad/Makefile
else
CFLAGS ?= -g
CFLAGS += -I ../libcad/include
LDFLAGS += -L ../libcad/target
include ../libcad/Makefile
endif
