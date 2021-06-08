TARGET=Benchmark.exe

all:
	make -C ../C-tests/C-fast_double clean all
	gcc -shared -fPIC -o fft.so ../C-tests/C-fast_double/fft.o
	mcs -optimize -unsafe -r:System.Numerics.dll Fft.cs Benchmark.cs Params.cs Native.cs -out:$(TARGET)

clean:
	make -C ../C-fast-all-complex-float-types-opt-4x clean
	rm -f $(TARGET)

PROG=$(TARGET)
test:
	./$(PROG)
	@(./$(PROG);./$(PROG);./$(PROG);./$(PROG);./$(PROG);  ./$(PROG);./$(PROG);./$(PROG);./$(PROG);./$(PROG) )|../avgcalc.py
