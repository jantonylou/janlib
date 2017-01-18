
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

mo_buffer_test:
	gcc -Wall -g -I include/ -DJTL_MO_BUFFER_16BIT=1 mo_buffer.c mo_buffer_test.c && ./a.out
	gcc -Wall -g -I include/ mo_buffer.c mo_buffer_test.c && ./a.out
mo_packet_test:
	gcc -Wall -g -I include/ -DJTL_MO_BUFFER_16BIT=1 -DJTL_MO_BUFFER_PRIV_DATA=4 mo_buffer.c mo_packet.c crc8.c mo_packet_test.c && ./a.out

