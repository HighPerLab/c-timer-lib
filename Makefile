CC := gcc
CFLAGS := -g -Wall -std=gnu99

.PHONY: all run

all: test_s.out test_ms.out test_ns.out test_mis.out

run: test_s.out test_ms.out test_ns.out test_mis.out
	@echo "## Seconds test"
	./test_s.out
	@echo "## Milli-seconds test"
	./test_ms.out
	@echo "## Micro-seconds test"
	./test_mis.out
	@echo "## Nano-seconds test"
	./test_ns.out

test_s.out: test.c timer.o
	$(CC) $(CFLAGS) $^ -o $@ -DTIMERUNIT=0

test_ms.out: test.c timer.o
	$(CC) $(CFLAGS) $^ -o $@ -DTIMERUNIT=1

test_ns.out: test.c timer.o
	$(CC) $(CFLAGS) $^ -o $@ -DTIMERUNIT=3

test_mis.out: test.c timer.o
	$(CC) $(CFLAGS) $^ -o $@ -DTIMERUNIT=2

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) *.o test_s.out test_ms.out test_ns.out test_mis.out
