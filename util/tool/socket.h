#ifndef __SOCKET_H__
#define __SOCKET_H__


#include "../base/base.h"
#include "../base/data_types.h"

//using namespace std;

namespace toolHelp
{

class CSocket
{
public:
    enum Type {TCP = SOCK_STREAM, UDP = SOCK_DGRAM};

private:
    CSocket(const CSocket& rSrc);    
    void operator=(const CSocket& rSrc); 

public:
    CSocket();
    virtual ~CSocket();

    //����SOCKET����,���󶨶˿�
    BOOL Create(int port=0, int type = SOCK_STREAM, const char* strAddress = NULL);  

public:    
    operator SOCKET() const;
    //����SOCKETΪ�����ͷ�����ģʽ(TRUE:������  FALSE:����, SOCKETϵͳȱʡΪ����)
    BOOL SetNoBlockMode(BOOL bFlag);
    //������һ��SOCKET����
    BOOL Attach(SOCKET hSocket);
    //����SOCKET����(ԭSOCKET�������)
    SOCKET Detach();

    //��ȡ���׽ӿ������ĶԶ˵�ַ
    BOOL GetPeerName(std::string& rPeerAddress, int& rPeerPort);
    BOOL GetPeerName(SOCKADDR* pSockAddr, int* pSockAddrLen);

    //��ȡ�׽ӿڵı���
    BOOL GetSockName(std::string& rSocketAddress, int& rSocketPort);
    BOOL GetSockName(SOCKADDR* pSockAddr, int* pSockAddrLen);

    //��ȡ������SOCKET����
    BOOL SetSockOpt(int nOptionName, const void* lpOptionValue,int nOptionLen, int nLevel = SOL_SOCKET);
    BOOL GetSockOpt(int nOptionName, void* lpOptionValue,int* lpOptionLen, int nLevel = SOL_SOCKET);

    //��ȡ������SOCKET��SO_KEEPALIVE����
    BOOL GetKeepAlive();
    void SetKeepAlive(BOOL bFlag);
    //����KEEPALIVE��ʱ(��λ:��)
    void SetKeepAlive(int nKeepIdle, int nKeepInterval, int nKeepCount);

    //����Linger�������
    void SetLinger(BOOL bOpen, int nLingerTime/*��λ:��*/);

    //��ȡ�����÷�������ջ�������С
    int  GetReceiveBufferSize();
    void SetReceiveBufferSize(int nSize);
    int  GetSendBufferSize();
    void SetSendBufferSize(int nSize);

    //��ȡ�������Ƿ��õ�ַ����
    BOOL GetReuseAddress();
    void SetReuseAddress(BOOL bFlag);

    //��ȡ�������Ƿ������͹㲥���ݱ�(UDP��Ч)
    BOOL GetBroadCast();
    void SetBroadCast(BOOL bFlag);

    //��ȡ������TCP_NODELAY����
    BOOL GetTcpNoDelay();
    void SetTcpNoDelay(BOOL bFlag);

    //��ȡ�����÷�������ճ�ʱʱ��(��λ������)
    int  GetReceiveTimeout();
    void SetReceiveTimeout(int nTimeout); 
    int  GetSendTimeout();
    void SetSendTimeout(int nTimeout);

public:
    //����һ��SOCKET����(SOCKET������Ҫ�Լ�����)
    BOOL Accept(CSocket* pConnectedSocket,SOCKADDR* pSockAddr = NULL, int* pSockAddrLen = NULL);
    BOOL Accept(CSocket &rConnectedSocket,std::string& rPeerAddress, int& rPeerPort);

    //�󶨶˿�
    BOOL Bind(int port,const char * strAddress = NULL);
    BOOL Bind (const SOCKADDR* pSockAddr, int nSockAddrLen);

    //�ر�����
    virtual void Close(void);

    //���ӵ�������
    BOOL Connect(const char *strAddress, int nPort);
    BOOL Connect(const char *strAddress, int nPort, int nTimeout);   //����ָ����ʱ����(��λ:����)
    BOOL Connect(const SOCKADDR* pSockAddr, int nSockAddrLen);

    //��ʼ����������
    BOOL Listen(int backlog = 5);

    //�����վ�(���ؽ��յ��ֽ��������մ���ʱ����-1)
    virtual int Receive(char* buf, int nBufLen, int flags = 0);

    //UDP��������(���ؽ��յ��ֽ��������մ���ʱ����-1)
    int ReceiveFrom(char* buf, int nBufLen, SOCKADDR* pSockAddr, int* pSockAddrLen, int nFlags = 0);
    int ReceiveFrom(char* buf, int nBufLen, std::string& rSocketAddress, int& rSocketPort, int nFlags = 0);

    //�ر��׽�����Ӧ����
    enum {RECVIVES = 0, SENDS = 1, BOTH = 2};
    BOOL ShutDown(int nHow = SENDS);

    //��������(���ط��͵��ֽ��������ʹ���ʱ����-1)
    virtual int Send(const char* buf, int nBufLen, int flags = 0);
    
    //UDP��������(���ط��͵��ֽ��������ʹ���ʱ����-1,��ַΪ��ʱ��㲥)
    int SendTo(const char* buf, int nBufLen, int nPort, char *strAddress = NULL, int nFlags = 0);
    int SendTo(const char* buf, int nBufLen, const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags = 0);

    //���չ̶��������ݣ�������ɻ�ʱ�ŷ���(��ʱ��λΪ����)
    BOOL ReceiveFixedLenData(char* buf, int nBufLen, int nTimeout);
    //���չ̶��������ݣ�������ɻ�ʱ�ŷ���
    BOOL ReceiveFixedLenData(char* buf, int nBufLen);

    //���͹̶��������ݣ��������ʱ�ŷ���(��ʱ��λΪ����)
    BOOL SendFixedLenData(const char* buf, int nBufLen, int nTimeout);
    //���͹̶��������ݣ�����������ʱ�ŷ���
    BOOL SendFixedLenData(const char* buf, int nBufLen);

 
private:
    BOOL Socket(int domain, int type, int protocol);

protected:
    SOCKET m_hSocket;                    //SOCKET������

};

}

#endif
