CC = gcc
TARGET = exe
CPPPATH = -I. -I/usr/local/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0
LINKFLAGS = -Wl,--no-as-needed -rdynamic
CPPDEFINES = -DDEBUG=1 -D_FILE_OFFSET_BITS=64
CXXFLAGS = -std=c++0x -g2 -O2 -Werror -Wno-deprecated
LDFLAGS = -lstdc++ -lm -lpthread -ldl -lrt -lmongoc-1.0 -lbson-1.0
DEPS = $(shell find ./ -name "*.h")
SRC = $(shell find ./ -name "*.cc")
OBJ = $(SRC:%.cc=%.o) 

$(TARGET): $(OBJ)
	$(CC) $(LINKFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cc $(DEPS)
	$(CC) -c $(CPPPATH) $(CPPDEFINES) $(CXXFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -rf $(OBJ) $(TARGET)