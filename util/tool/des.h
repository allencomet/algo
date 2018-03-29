
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
	
    bool  Encrypt(char *Out,char *In,int len,const char *KeyStr); //加密
	bool  Decrypt(char *Out,char *In,int len,const char *KeyStr); //解密
	void SetSubKey(const char* KeyStr); //由输入密钥串设置子密钥

	static char* RandKeyStr(char KeyStr[9]);//产生随机密钥串
		
private:
     void DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type=ENCRYPT);  //标准DES加/解密

	 void SetSubKey(PSubKey pSubKey, const char Key[8]);  //由输入密钥设置16圈子密钥
	 void F_func(bool In[32], const bool Ki[48]);  // f 函数
	 void S_func(bool Out[32], const bool In[48]);  // S 盒代替

	 void Transform(bool *Out, bool *In, const char *Table, int len);  // 变换
	 void Xor(bool *InA, const bool *InB, int len);  // 异或
	 void RotateL(bool *In, int len, int loop);  // 循环左移
	 void ByteToBit(bool *Out, const char *In, int bits);  // 字节组转换成位组
	 void BitToByte(char *Out, const bool *In, int bits);  // 位组转换成字节组

private:
    bool SubKey[2][16][48];  //16圈子密钥
    bool Is3DES;  // 3次DES标志
    char deskey[16]; //密钥串缓冲区
};

}

#endif
