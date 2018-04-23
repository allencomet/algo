CC = gcc
TARGET = exe

#����Դ��Ѱ��ͷ�ļ�·��
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

#gcc����֧��-D�궨�壬�൱��C�е�ȫ��#define
CXXDEFINES = -DDEBUG=1 -D_FILE_OFFSET_BITS=64

#C++���Ա���������
CXXFLAGS = -std=c++0x -g2 -O2 -Werror -Wno-deprecated

#����������
LDFLAGS = -lstdc++ -lm -lpthread -ldl -lrt -lmongoc-1.0 -lbson-1.0

#�г�����������ͷ�ļ�
DEPS = $(shell find ./ -name "*.h")

#�г�����������Դ�ļ�
SRC = $(shell find ./ -name "*.cc")

#$(src:%.cc=%.o)����һ���ַ��滻���������Ὣsrc���е�.cc�ִ��滻��.o��
#ʵ���Ͼ͵����г�������.cc�ļ�Ҫ����Ľ���������������趨���������ǽ�
#���ڸù��̼������.cc��.h�ļ���Makefile�����Զ��������뵽��������
OBJ = $(SRC:%.cc=%.o) 

$(TARGET): $(OBJ)
	$(CC) $(LINKFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJ)

#$@������ǵ�ǰ����Ŀ�꣬$<�������������ϵ���еĵ�һ����õ���������ϵ��ʹ��$^
#����һ��ģʽ���򣬱�ʾ���е�.oĿ�궼����������ͬ����.cc�ļ�����Ȼ����DEPS���г���ͷ�ļ���
%.o: %.cc $(DEPS)
	$(CC) -c $(CXXPATH) $(CXXDEFINES) $(CXXFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -rf $(OBJ) $(TARGET)