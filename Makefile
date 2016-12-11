PROG = matrix_producer_consumer
default: build
build:
	gcc main.c util.c -o bin/${PROG}
clean:
	rm bin/*