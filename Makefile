CC=g++
CFLAGS=-c -Wall -O2 -fopenmp
LDFLAGS=
LIBFLAGS=-lglut -lGL -lGLU -lfreeimageplus -fopenmp
SOURCES=main.cpp Renderer.cpp FrameSaver.cpp BoundingTree.cpp force.cpp particle.cpp simulator.cpp grid.cpp Matrix.cpp source.cpp vector.cpp point.cpp scene.cpp object.cpp face.cpp normal.cpp kernel.cpp field.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=final
RM = /bin/rm -f

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	$(RM) *.o $(EXECUTABLE)
