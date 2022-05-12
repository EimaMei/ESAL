GXX = g++
ARGS = -O2 -C --no-warnings
SOURCEDIR = source/ssal.cpp
WLIBS = -lwinmm

all:
	@make build
	@make compile
build:
	$(GXX) $(WARGS) $(SOURCEDIR) $(WLIBS) -c
	@$(GXX) -shared ssal.o -lwinmm -static -o libSSAL.a
	rm ssal.o
compile:
	g++ -O2 main.cpp libSSAL.a