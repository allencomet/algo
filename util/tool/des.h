
#ifndef _YDESH_
#define _YDESH_

namespace toolHelp
{

typedef bool    (*PSubKey)[16][48];
enum    {ENCRYPT,DECRYPT};

class CDes
{	
public:	
    CDes();
	
    bool  Encrypt(char *Out,char *In,int len,const char *KeyStr); //����
	bool  Decrypt(char *Out,char *In,int len,const char *KeyStr); //����
	void SetSubKey(const char* KeyStr); //��������Կ����������Կ

	static char* RandKeyStr(char KeyStr[9]);//���������Կ��
		
private:
     void DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type=ENCRYPT);  //��׼DES��/����

	 void SetSubKey(PSubKey pSubKey, const char Key[8]);  //��������Կ����16Ȧ����Կ
	 void F_func(bool In[32], const bool Ki[48]);  // f ����
	 void S_func(bool Out[32], const bool In[48]);  // S �д���

	 void Transform(bool *Out, bool *In, const char *Table, int len);  // �任
	 void Xor(bool *InA, const bool *InB, int len);  // ���
	 void RotateL(bool *In, int len, int loop);  // ѭ������
	 void ByteToBit(bool *Out, const char *In, int bits);  // �ֽ���ת����λ��
	 void BitToByte(char *Out, const bool *In, int bits);  // λ��ת�����ֽ���

private:
    bool SubKey[2][16][48];  //16Ȧ����Կ
    bool Is3DES;  // 3��DES��־
    char deskey[16]; //��Կ��������
};

}

#endif
