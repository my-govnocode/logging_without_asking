all:
	gcc -ldl -shared -fPIC src/queue.c src/io_operations.c src/mem_operations.c src/logger.c -o liblogger.so
	gcc example/test.c -o test.out
	gcc -o logd.out src/queue.c src/listener.c src/logd.c -lpthread -lrt
	rm -f *.o