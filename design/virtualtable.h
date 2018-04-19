#ifndef __DESIGN_VIRTUAL_TABLE_H
#define __DESIGN_VIRTUAL_TABLE_H

//==================================================================================
//==================================================================================
//==============				以下例子都是以钻石继承讲解			  ==============
//==================================================================================
//==================================================================================



//==================================================================================
//==================================================================================
//==============				以下是普通继承的类内存分布			  ==============
//==================================================================================
//==================================================================================

class Base {
public:
	void commonFunc();
	virtual void VirtualcommonFunc();
private:
	int _a;
	int _b;
};

class DerivedA :public Base {
public:
	virtual void VirtualFuncA();
private:
	int _c;
};

class DerivedB :public Base {
public:
	virtual void VirtualFuncB();
private:
	int _d;
};

/*
1>  class DerivedAB	size(36):
1>  	+---
1>   0	| +--- (base class DerivedA)
1>   0	| | +--- (base class Base)
1>   0	| | | {vfptr}
1>   4	| | | _a
1>   8	| | | _b
1>  	| | +---
1>  12	| | _c
1>  	| +---
1>  16	| +--- (base class DerivedB)
1>  16	| | +--- (base class Base)
1>  16	| | | {vfptr}
1>  20	| | | _a
1>  24	| | | _b
1>  	| | +---
1>  28	| | _d
1>  	| +---
1>  32	| _e
1>  	+---
1>
1>  DerivedAB::$vftable@DerivedA@:
1>  	| &DerivedAB_meta
1>  	|  0
1>   0	| &Base::VirtualcommonFunc
1>   1	| &DerivedA::VirtualFuncA
1>
1>  DerivedAB::$vftable@DerivedB@:
1>  	| -16
1>   0	| &Base::VirtualcommonFunc
1>   1	| &DerivedB::VirtualFuncB

如果DerivedA和DerivedB不采用虚继承方式，那么DerivedAB继承之后就会有两个虚表指针，
分别指向两个直接父类的虚表
*/
class DerivedAB :public DerivedA, public DerivedB {
public:

private:
	int _e;
};




//==================================================================================
//==================================================================================
//==============				以下是虚继承的类内存分布			  ==============
//==================================================================================
//==================================================================================

/*
1>  class DerivedVA	size(20):
1>  	+---
1>   0	| {vbptr}					//当前类的虚表指针
1>   4	| _c
1>  	+---
1>  	+--- (virtual base Base)	//虚基类的虚表指针
1>   8	| {vfptr}
1>  12	| _a
1>  16	| _b
1>  	+---
1>
1>  DerivedVA::$vbtable@:		//当前类的虚表
1>   0	| 0
1>   1	| 8 (DerivedVAd(DerivedVA+0)Base)
1>
1>  DerivedVA::$vftable@:		//虚基类的虚表指针
1>  	| -8
1>   0	| &DerivedVA::VirtualcommonFunc
1>
1>  DerivedVA::VirtualcommonFunc this adjustor: 8
1>  vbi:	   class  offset o.vbptr  o.vbte fVtorDisp
1>              Base       8       0       4 0
*/
class DerivedVA :virtual public Base {
public:
	virtual void VirtualcommonFunc();
private:
	int _c;
};

//同上
class DerivedVB :virtual public Base {
public:
	virtual void VirtualcommonFunc();
private:
	int _d;
};


/*
1>  class DerivedVAB	size(32):
1>  	+---
1>   0	| +--- (base class DerivedVA)
1>   0	| | {vbptr}							//指向父类DerivedVA的虚表指针
1>   4	| | _c
1>  	| +---
1>   8	| +--- (base class DerivedVB)
1>   8	| | {vbptr}							//指向父类DerivedVB的虚表指针
1>  12	| | _d
1>  	| +---
1>  16	| _e
1>  	+---
1>  	+--- (virtual base Base)			//指向虚基类的虚表指针
1>  20	| {vfptr}
1>  24	| _a
1>  28	| _b
1>  	+---
1>
1>  DerivedVAB::$vbtable@DerivedVA@:		//指向父类DerivedVA的虚表
1>   0	| 0
1>   1	| 20 (DerivedVABd(DerivedVA+0)Base)
1>
1>  DerivedVAB::$vbtable@DerivedVB@:		//指向父类DerivedVB的虚表
1>   0	| 0
1>   1	| 12 (DerivedVABd(DerivedVB+0)Base)
1>
1>  DerivedVAB::$vftable@:					//指虚基类的虚表
1>  	| -20
1>   0	| &DerivedVAB::VirtualcommonFunc
1>
1>  DerivedVAB::VirtualcommonFunc this adjustor: 20
1>  vbi:	   class  offset o.vbptr  o.vbte fVtorDisp
*/
class DerivedVAB :public DerivedVA, public DerivedVB {
public:
	virtual void VirtualcommonFunc();
private:
	int _e;
};


/************************************************************************/
/* 
1. 每个类都有虚指针和虚表；

2. 如果不是虚继承，那么子类将父类的虚指针继承下来，并指向自身的虚表（发生在对象构造时）。有多少个虚函数，虚表里面的项就会有多少。
多重继承时，可能存在多个的基类虚表与虚指针；

3. 如果是虚继承，那么子类会有两份虚指针，一份指向自己的虚表，另一份指向虚基表，多重继承时虚基表与虚基表指针有且只有一份。
*/
/************************************************************************/

#endif