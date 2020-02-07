CFLAGS = -std=c99 -Wall -Wextra -Wpedantic
LDFLAGS = -lm -lusb-1.0

DEPS = 3rdparty/argtable3.h razer.h list_devices.h brightness.h effect.h color.h
OBJS = 3rdparty/argtable3.o razer.o list_devices.o brightness.o effect.o color.o rzctl.o

rzctl: ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

%.o: %.c ${DEPS}
	${CC} -c -o $@ $< ${CFLAGS}

clean:
	rm -f rzctl *.o *.d

.PHONY: clean
