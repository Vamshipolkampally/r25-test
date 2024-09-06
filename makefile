CC = gcc
CFLAGS = -Og -g -Wall
all: build
build:
	$(CC) src/*.c lib/*.c -Iinclude -o main.exe
check:
	@echo "--------------------------------------------"
	@echo "Checking..."
	@echo "Test-1: "
	./main.exe test/sbus_data[1] test/sab_data[1]
	diff test/sab_data[1] test/expected[1]
	@echo "Test-2: "
	./main.exe test/sbus_data[2] test/sab_data[2]
	diff test/sab_data[2] test/expected[2]
	@echo "Test-3: "
	./main.exe test/sbus_data[3] test/sab_data[3]
	diff test/sab_data[3] test/expected[3]
	@echo "**** Success: ***"
	@echo "--------------------------------------------"
clean:
	rm -f main.exe
	rm -f test/sab_data[1]
	rm -f test/sab_data[2]
	rm -f test/sab_data[3]
.PHONY: clean check
