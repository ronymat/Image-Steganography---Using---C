STE:= $(patsubst %.c, %.o, $(wildcard *.c))
Steg.exe: $(STE)
		gcc -o $@ $^
clean:
		rm *.o *.exe
