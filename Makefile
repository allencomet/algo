CC = gcc
TARGET = exe

#编译源码寻找头文件路径
CXXPATH = -I. -I/usr/local/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0

#Pass the flag -export-dynamic to the ELF linker, on targets that support it. 
#This instructs the linker to add all symbols, not only used ones, to the dynamic 
#symbol table. This option is needed for some uses of dlopen or to allow obtaining 
#backtraces from within a program.

#Pass option as an option to the linker. If option contains commas, it is split 
#into multiple options at the commas. You can use this syntax to pass an argument 
#to the option. For example, -Wl,-Map,output.map passes -Map output.map to the linker. 
#When using the GNU linker, you can also get the same effect with -Wl,-Map=output.map.

#refer to https://stackoverflow.com/questions/8623884/gcc-debug-symbols-g-flag-vs-linkers-rdynamic-option
LINKFLAGS = -Wl,--no-as-needed -rdynamic

#gcc命令支持-D宏定义，相当于C中的全局#define
CXXDEFINES = -DDEBUG=1 -D_FILE_OFFSET_BITS=64

#C++语言编译器参数
CXXFLAGS = -std=c++0x -g2 -O2 -Werror -Wno-deprecated

#链接器参数
LDFLAGS = -lstdc++ -lm -lpthread -ldl -lrt -lmongoc-1.0 -lbson-1.0

#列出依赖的所有头文件
DEPS = $(shell find ./ -name "*.h")

#列出依赖的所有源文件
SRC = $(shell find ./ -name "*.cc")

#$(src:%.cc=%.o)则是一个字符替换函数，它会将src所有的.cc字串替换成.o，
#实际上就等于列出了所有.cc文件要编译的结果，有了这两个设定，无论我们今
#后在该工程加入多少.cc和.h文件，Makefile都能自动将其纳入到工程中来
OBJ = $(SRC:%.cc=%.o) 

$(TARGET): $(OBJ)
	$(CC) $(LINKFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJ)

#$@代表的是当前语句的目标，$<代表的是依赖关系表中的第一项，引用的是整个关系表使用$^
#这是一个模式规则，表示所有的.o目标都依赖于与它同名的.cc文件（当然还有DEPS中列出的头文件）
%.o: %.cc $(DEPS)
	$(CC) -c $(CXXPATH) $(CXXDEFINES) $(CXXFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -rf $(OBJ) $(TARGET)