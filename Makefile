# build target
bin/SSPR : src/SSPR.c
	gcc -o bin/SSPR src/SSPR.c

# build object files
# ...

# phony targets
.PHONY : run clean
run :
	./bin/SSPR
clean :
	rm bin/*
