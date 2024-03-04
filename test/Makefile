include ../config.mk

LIBS+=-l:libqk.a -lm
OBJ+=test.o unit.o test_hmap.o test_ll.o test_buf.o test_buf_str.o test_eval.o
CFLAGS+=-g

all: test

../libqk.a:
	cd .. && $(MAKE) libqk.a

%.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

test: ../libqk.a $(OBJ)
	$(CC) $(CFLAGS) -L .. $(OBJ) $(LIBS) -o $@

clean:
	rm -f *.o test

.PHONY: clean all
