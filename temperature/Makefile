DEBUG := false

SOURCES := $(wildcard source/*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

ifeq ($(DEBUG), true)
	CFLAGS += -Iinclude -fPIC -ggdb
else
	CFLAGS += -Iinclude -fPIC -O2
endif
LDFLAGS += -Llib -ltemp -lpthread

MODULE_NAME := libtemp.so

.PHONY: all test clean install uninstall

all: $(MODULE_NAME)
	gcc $(CFLAGS) -c -o main.o main.c
	gcc -o temperature main.o $(LDFLAGS)

test: $(MODULE_NAME)
	gcc $(CFLAGS) -c -o test/test_ds18b20.o test/test_ds18b20.c
	gcc -o test/test_ds18b20 test/test_ds18b20.o $(LDFLAGS)

clean:
	find . -name "*.o" -o -name "*.so" | xargs rm -f
	rm -rf temperature temperature.dSYM
	rm -rf test/test_ds18b20 test/test_ds18b20.dSYM

install:
	sudo cp lib/libtemp.so /usr/lib
	sudo cp temperature /usr/bin

uninstall:
	sudo rm -f /usr/lib/libtemp.so
	sudo rm -f /usr/bin/temperature

$(MODULE_NAME): $(OBJECTS)
	@mkdir -p lib
	gcc -shared -o lib/$@ $^
	chmod 0644 lib/$@

