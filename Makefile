GXX = g++
ARGS = -O2 -C --no-warnings
SOURCEDIR = source/ssal.cpp
WLIBS = -lmingw32 -static-libstdc++ -lwinmm

all:
	@make build
	@make compile
buildLibrary:
	$(GXX) $(WARGS) $(SOURCEDIR) $(WLIBS) -c
	@$(GXX) -shared ssal.o -lwinmm
buildCMD:
	$(GXX) -O2 $(SOURCEDIR) main.cpp $(WLIBS) -o ssal.exe