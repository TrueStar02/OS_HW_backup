run:
	@./main.out
debug:
	@gdb main.out
build:
	@gcc -g -o c_func.o -m32 -c c_func.c
	@g++ -g -o cpp_func.o -m32 -c cpp_func.cpp
	@g++ -g -o main.o -m32 -c main.cpp
	@nasm -g -o asm_func.o -f elf32 asm_func.asm
	@g++ -g -o main.out main.o c_func.o cpp_func.o asm_func.o -m32
clean:
	@rm -fr *.bin *.o *.out
