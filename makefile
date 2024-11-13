CFLAGS=-std=c++20 -O2 -I.
LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

OUT_NAME = tectonic
BIN_DIR = "./bin"

SRC = $(shell find . -name '*.cpp')
OBJ = $(patsubst ./%.cpp,$(BIN_DIR)/%.o,$(SRC))

all: $(OUT_NAME)

$(OUT_NAME): $(OBJ)
	g++ $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(BIN_DIR)/%.o: %.cpp | $(BIN_DIR)
	@mkdir -p $(dir $@)
	g++ $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR) && rm -f $(OUT_NAME)

run:
	./$(OUT_NAME)

dev: all run

.PHONY: all clean run dev