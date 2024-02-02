include config.mk

all: libqk.so libqk.a

CFLAGS+=-Os -fdata-sections -ffunction-sections
LIBDESTPATH=/usr/local/lib
INCDESTPATH=/usr/local/include
OBJ=$(patsubst %.c,%.o,$(wildcard src/*/*.c))

%.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

libqk.so: $(OBJ)
	$(CC) $(CFLAGS) -fPIC -shared $(OBJ) -o $@

libqk.a: $(OBJ)
	$(AR) rcs $@ $^

clean:
	find . -type f \( -name '*.o' -o -name '*.a' -o -name '*.so' \) -delete
	cd test && $(MAKE) $@

install: all
	cp -f libqk.* $(LIBDESTPATH)
	cp -rf include/* $(INCDESTPATH)

uninstall:
	rm -f $(LIBDESTPATH)/libqk.*
	rm -f $(INCDESTPATH)/qk.h
	rm -rf $(INCDESTPATH)/qk

test:
	cd $@ && $(MAKE) $@ && ./$@

.PHONY: clean all test install uninstall
