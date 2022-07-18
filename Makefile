# Compiling settings
GXX = g++
OS := $(shell uname)
ARCH := $(shell uname -m)

# Building the library
MSOURCE = source/mac
MFLAGS = -O2 -std=c++11 -arch $(ARCH)
MLIBS = -framework Foundation -framework AVFoundation
MOUTPUT = libESAL.a

# Building the app
SOURCE = main.cpp
FLAGS = -O2 -std=c++17 -arch $(ARCH)
LIBS = -lESAL
OUTPUT = esal

all: $(MOUTPUT)

run:
	@clear
	@echo "Running '$(OUTPUT)'"
	@./$(OUTPUT)
	
$(MOUTPUT): $(MSOURCE)/*.mm #$(MSOURCE)/*.cpp
	@echo "Building '$(MOUTPUT)'"
	$(GXX) $(MFLAGS) $^ -c
	$(GXX) -shared $(MFLAGS) $(MLIBS) *.o -o $(MOUTPUT)
	@rm *.o;
	@echo "Finished building '$(MOUTPUT)'"

app:
	@echo "Building '$(OUTPUT)'"
	$(GXX) $(FLAGS) $(SOURCE) $(LIBS) -o $(OUTPUT)
	@echo "Finished building '$(OUTPUT)'"
	@make run
	
clean:
	@rm $(OUTPUT) $(MOUTPUT)
	@echo "Cleaned"

install:
	@make $(MOUTPUT)
	sudo mkdir -p /usr/local/lib/
	sudo mkdir -p /usr/local/include/
	sudo cp $(MOUTPUT) /usr/local/lib/
	sudo cp include/ESAL.hpp /usr/local/include/