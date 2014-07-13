DEBUG := false

SOURCES := $(wildcard source/*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

ifeq ($(DEBUG), true)
	CFLAGS += -Iinclude -fPIC -ggdb
else
	CFLAGS += -Iinclude -fPIC -O2
endif
LDFLAGS += -Llib -lpthread

MODULE_NAME := libtemp.so

.PHONY: all test clean

all: $(MODULE_NAME)
	gcc $(CFLAGS) $(LDFLAGS) -o temperature main.c -ltemp

test: $(MODULE_NAME)
	gcc $(CFLAGS) $(LDFLAGS) -o test/test_ds18b20 test/test_ds18b20.c -ltemp

clean:
	find . -name "*.o" -o -name "*.so" | xargs rm -f
	rm -rf temperature temperature.dSYM
	rm -rf test/test_ds18b20 test/test_ds18b20.dSYM

$(MODULE_NAME): $(OBJECTS)
	@mkdir -p lib
	gcc -shared -o lib/$@ $^

