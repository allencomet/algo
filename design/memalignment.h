#ifndef __DESIGN_MEM_ALIGNMENT_H
#define __DESIGN_MEM_ALIGNMENT_H

#pragma pack(push,4)

typedef union Un4 {
	char ch1;
	int k[5];
	char ch2;
}Un4;
//sizeof(Un4) == 5*4

typedef struct Xx4 {
	char ch1;
	Un4 un;
	int n;
	char ch2;
}Xx4;
//sizeof(Xx4) == 4+20+4+4

typedef struct Yy4 {
	char ch1;
	int k[5];
	char str[7];
	int n;
}Yy4;
//sizeof(Yy4) == 4+5*4+8+4

#pragma pack(pop)


#pragma pack(push,2)

typedef union Un2 {
	char ch1;
	int k[5];
	char ch2;
}Un2;
//sizeof(Un2) == 5*(2+2)

typedef struct Xx2 {
	char ch1;
	Un4 un;
	int n;
	char ch2;
}Xx2;
//sizeof(Xx2) == 2+20+(2+2)+2

#pragma pack(pop)

#pragma pack(push,1)

typedef union Un1 {
	char ch1;
	int k[5];
	char ch2;
}Un1;
//sizeof(Un1) == 5*4

typedef struct Xx1 {
	char ch1;
	Un4 un;
	int n;
	char ch2;
}Xx1;
//sizeof(Xx1) == 1+20+4+1

#pragma pack(pop)

#endif