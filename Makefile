all:
	g++ server.cpp -lSDL2 -lSDL2_net -Iinclude -o bin
	rm checksum
	md5sum bin >> checksum
clean:
	rm bin
	rm checksum
rebuild:
	rm bin
	rm checksum
	g++ server.cpp -lSDL2 -lSDL2_net -Iinclude -o bin
	md5sum bin >> checksum
struct:
	tree -C
