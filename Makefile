all:
	gcc server.c -o server -lsqlite3 -lcurl
	gcc client.c -o client -lsqlite3 
	./server
