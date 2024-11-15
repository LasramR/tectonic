CFLAGS=-std=c++20 -O2 -I.
LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

OUT_NAME = tectonic
BIN_DIR = "./bin"

SRC = $(shell find . -name '*.cpp')
OBJ = $(patsubst ./%.cpp,$(BIN_DIR)/%.o,$(SRC))
VERT = $(shell find . -name '*.vert')
SPV_VERT = $(VERT:%=%.spv)
FRAG = $(shell find . -name '*.frag')
SPV_FRAG = $(FRAG:%=%.spv)

all: $(OUT_NAME) shader

$(OUT_NAME): $(OBJ)
	g++ $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(BIN_DIR)/%.o: %.cpp | $(BIN_DIR)
	@mkdir -p $(dir $@)
	g++ $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR) && rm -f $(OUT_NAME) && rm ./shaders/*.spv

run:
	./$(OUT_NAME)

dev: all run

shader: $(SPV_VERT) $(SPV_FRAG)

%.vert.spv: %.vert
	glslc $< -o $@
%.frag.spv: %.frag
	glslc $< -o $@

.PHONY: all clean run dev