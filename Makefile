CC=aarch64-linux-android21-clang
CXX=$(CC)++
LDFLAGS=-static-libstdc++
LIBNAME=bakdroid.hxo
build:
	$(CXX) -fPIC -c *.cpp -I./inih/cpp/
	$(CXX) -fPIC -c ./inih/cpp/*.cpp
	$(CC) -fPIC -c ./inih/*.c
	$(CXX) -o $(LIBNAME) *.o -shared $(LDFLAGS)

clean:
	rm -v *.o $(LIBNAME)
