#ifndef __DESIGN_VIRTUAL_TABLE_H
#define __DESIGN_VIRTUAL_TABLE_H

//==================================================================================
//==================================================================================
//==============				�������Ӷ�������ʯ�̳н���			  ==============
//==================================================================================
//==================================================================================



//==================================================================================
//==================================================================================
//==============				��������ͨ�̳е����ڴ�ֲ�			  ==============
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

���DerivedA��DerivedB��������̳з�ʽ����ôDerivedAB�̳�֮��ͻ����������ָ�룬
�ֱ�ָ������ֱ�Ӹ�������
*/
class DerivedAB :public DerivedA, public DerivedB {
public:

private:
	int _e;
};




//==================================================================================
//==================================================================================
//==============				��������̳е����ڴ�ֲ�			  ==============
//==================================================================================
//==================================================================================

/*
1>  class DerivedVA	size(20):
1>  	+---
1>   0	| {vbptr}					//��ǰ������ָ��
1>   4	| _c
1>  	+---
1>  	+--- (virtual base Base)	//���������ָ��
1>   8	| {vfptr}
1>  12	| _a
1>  16	| _b
1>  	+---
1>
1>  DerivedVA::$vbtable@:		//��ǰ������
1>   0	| 0
1>   1	| 8 (DerivedVAd(DerivedVA+0)Base)
1>
1>  DerivedVA::$vftable@:		//���������ָ��
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

//ͬ��
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
1>   0	| | {vbptr}							//ָ����DerivedVA�����ָ��
1>   4	| | _c
1>  	| +---
1>   8	| +--- (base class DerivedVB)
1>   8	| | {vbptr}							//ָ����DerivedVB�����ָ��
1>  12	| | _d
1>  	| +---
1>  16	| _e
1>  	+---
1>  	+--- (virtual base Base)			//ָ�����������ָ��
1>  20	| {vfptr}
1>  24	| _a
1>  28	| _b
1>  	+---
1>
1>  DerivedVAB::$vbtable@DerivedVA@:		//ָ����DerivedVA�����
1>   0	| 0
1>   1	| 20 (DerivedVABd(DerivedVA+0)Base)
1>
1>  DerivedVAB::$vbtable@DerivedVB@:		//ָ����DerivedVB�����
1>   0	| 0
1>   1	| 12 (DerivedVABd(DerivedVB+0)Base)
1>
1>  DerivedVAB::$vftable@:					//ָ���������
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
1. ÿ���඼����ָ������

2. ���������̳У���ô���ཫ�������ָ��̳���������ָ���������������ڶ�����ʱ�����ж��ٸ��麯��������������ͻ��ж��١�
���ؼ̳�ʱ�����ܴ��ڶ���Ļ����������ָ�룻

3. �������̳У���ô�������������ָ�룬һ��ָ���Լ��������һ��ָ����������ؼ̳�ʱ������������ָ������ֻ��һ�ݡ�
*/
/************************************************************************/

#endif