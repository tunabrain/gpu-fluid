CXX = g++
RM = rm -f
WARNINGS = -Wall -Wformat -Wcast-align
CFLAGS = -Isrc/ -O2 $(WARNINGS)

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lfreeglut -lopengl32 -lglu32 -lglew32
    TARGET = fluid.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LDFLAGS = -lGL -lGLU -lglut -lGLEW
        TARGET = fluid
    endif
endif

MATH_OBJS = Mat4.o Vec3.o Vec4.o
RENDER_OBJS = BufferObject.o MatrixStack.o RenderTarget.o Shader.o \
	ShaderObject.o Texture.o VertexBuffer.o
FLUID_OBJS = Debug.o File.o Fluid.o Main.o Util.o lodepng/lodepng.o \
	$(addprefix math/,$(MATH_OBJS)) $(addprefix render/,$(RENDER_OBJS))
OBJECTS = $(addprefix src/,$(FLUID_OBJS))

fluid: $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(TARGET)
	$(RM) -f $(OBJECTS)

.PHONY: clean fluid
