
CXXFLAGS=$(shell pkg-config --cflags exiv2) $(shell pkg-config --cflags libjpeg) 
LDFLAGS=$(shell pkg-config --libs exiv2) $(shell pkg-config --libs libjpeg)
DEBUG_FLAGS=-ggdb3 -O0 -DDEBUG
FILES=src/io.cpp src/main.cpp src/tiff.cpp src/crx.cpp src/jpeg.cpp src/ljpeg.cpp src/ppm.cpp
OBJECTS=$(subst src/,build/,$(FILES:.cpp=.o))

default: cora

release: DEBUG_FLAGS =
release: cora

clean:
	rm -r build

build/%.o: src/%.cpp
	g++ $(CXXFLAGS) $(DEBUG_FLAGS) $^ -c -o $@

cora: build $(OBJECTS)
	g++ $(LDFLAGS) $(DEBUG_FLAGS) $(OBJECTS) -o $@

build:
	mkdir build

.PHONY: default release
