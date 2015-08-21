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

test_s.out: test.c timer.h
	$(CC) -g -Wall $^ -o $@ -DTIMERUNIT=0

test_ms.out: test.c timer.h
	$(CC) -g -Wall $^ -o $@ -DTIMERUNIT=1

test_ns.out: test.c timer.h
	$(CC) -g -Wall $^ -o $@ -DTIMERUNIT=3

test_mis.out: test.c timer.h
	$(CC) -g -Wall $^ -o $@ -DTIMERUNIT=2

clean:
	$(RM) test_s.out test_ms.out test_ns.out test_mis.out
