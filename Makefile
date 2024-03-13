include config.mk

all: libqk.so libqk.a

CFLAGS+=-O2 -fdata-sections -ffunction-sections -fPIC
LIBDESTPATH=/usr/local/lib
INCDESTPATH=/usr/local/include
OBJ=$(patsubst %.c,%.o,$(wildcard src/*/*.c))
LIBS=-lm

%.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

libqk.so: $(OBJ)
	$(CC) $(CFLAGS) -shared $(OBJ) $(LIBS) -o $@

libqk.a: $(OBJ)
	$(AR) rcs $@ $^

clean:
	rm -f $(OBJ) *.a *.so
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

debug: CFLAGS+=-g
debug: all

.PHONY: clean all test install uninstall debug
