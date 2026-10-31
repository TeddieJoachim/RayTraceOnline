# compiler
EMCC = emcc 

# in order: -O2 enables compiler optimization, 
# -sMODULARIZE=1 wraps the generated javascript in a function that can generate the module
# -sEXPORT_NAME names the function that will load the module
CFLAGS = -O2 -sMODULARIZE=1 -sEXPORT_NAME=createWASMModule 

TARGET = boidRunner.js

all: $(TARGET)

$(TARGET): boids.c 
	$(EMCC) boids.c $(CFLAGS) -o $(TARGET)

serve: all 
	python3 -m http.server 8000 

killServe: all 
	fuser -k 8000/tcp

clean:
	rm -f boidRunner.js boidRunner.wasm 
