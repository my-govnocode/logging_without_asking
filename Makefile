all:
	gcc -ldl -shared -fPIC src/queue.c src/io_operations.c src/mem_operations.c src/logger.c -o bin/liblogger.so
	gcc example/test.c -o bin/test.out
	gcc -o bin/logd.out src/queue.c src/listener.c src/logd.c -lpthread -lrt
	rm -f *.o