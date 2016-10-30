
LIBNAME=libjan.a
LIB=${ARCH}/${LIBNAME}

C_FILES=mo_buffer.c fdops.c time_rtems.c termios.c
C_O_FILES=$(C_FILES:%.c=${ARCH}/%.o)

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(RTEMS_SHARE)/make/lib.cfg

CFLAGS   += -Iinclude

all:	${ARCH} $(SRCS) $(LIB)

$(LIB): ${OBJS}
	$(make-library)

