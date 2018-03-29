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

    //创建SOCKET对象,并绑定端口
    BOOL Create(int port=0, int type = SOCK_STREAM, const char* strAddress = NULL);  

public:    
    operator SOCKET() const;
    //设置SOCKET为阻塞和非阻塞模式(TRUE:非阻塞  FALSE:阻塞, SOCKET系统缺省为阻塞)
    BOOL SetNoBlockMode(BOOL bFlag);
    //关联到一个SOCKET对象
    BOOL Attach(SOCKET hSocket);
    //返回SOCKET对象(原SOCKET对象清空)
    SOCKET Detach();

    //获取与套接口相连的对端地址
    BOOL GetPeerName(std::string& rPeerAddress, int& rPeerPort);
    BOOL GetPeerName(SOCKADDR* pSockAddr, int* pSockAddrLen);

    //获取套接口的本地
    BOOL GetSockName(std::string& rSocketAddress, int& rSocketPort);
    BOOL GetSockName(SOCKADDR* pSockAddr, int* pSockAddrLen);

    //获取与设置SOCKET参数
    BOOL SetSockOpt(int nOptionName, const void* lpOptionValue,int nOptionLen, int nLevel = SOL_SOCKET);
    BOOL GetSockOpt(int nOptionName, void* lpOptionValue,int* lpOptionLen, int nLevel = SOL_SOCKET);

    //获取和设置SOCKET的SO_KEEPALIVE属性
    BOOL GetKeepAlive();
    void SetKeepAlive(BOOL bFlag);
    //设置KEEPALIVE超时(单位:秒)
    void SetKeepAlive(int nKeepIdle, int nKeepInterval, int nKeepCount);

    //设置Linger相关属性
    void SetLinger(BOOL bOpen, int nLingerTime/*单位:秒*/);

    //获取和设置发送与接收缓冲区大小
    int  GetReceiveBufferSize();
    void SetReceiveBufferSize(int nSize);
    int  GetSendBufferSize();
    void SetSendBufferSize(int nSize);

    //获取和设置是否复用地址属性
    BOOL GetReuseAddress();
    void SetReuseAddress(BOOL bFlag);

    //获取和设置是否允许发送广播数据报(UDP有效)
    BOOL GetBroadCast();
    void SetBroadCast(BOOL bFlag);

    //获取和设置TCP_NODELAY属性
    BOOL GetTcpNoDelay();
    void SetTcpNoDelay(BOOL bFlag);

    //获取和设置发送与接收超时时间(单位：毫秒)
    int  GetReceiveTimeout();
    void SetReceiveTimeout(int nTimeout); 
    int  GetSendTimeout();
    void SetSendTimeout(int nTimeout);

public:
    //接收一个SOCKET对象(SOCKET对象需要自己创建)
    BOOL Accept(CSocket* pConnectedSocket,SOCKADDR* pSockAddr = NULL, int* pSockAddrLen = NULL);
    BOOL Accept(CSocket &rConnectedSocket,std::string& rPeerAddress, int& rPeerPort);

    //绑定端口
    BOOL Bind(int port,const char * strAddress = NULL);
    BOOL Bind (const SOCKADDR* pSockAddr, int nSockAddrLen);

    //关闭连接
    virtual void Close(void);

    //连接到服务器
    BOOL Connect(const char *strAddress, int nPort);
    BOOL Connect(const char *strAddress, int nPort, int nTimeout);   //可以指定超时长度(单位:毫秒)
    BOOL Connect(const SOCKADDR* pSockAddr, int nSockAddrLen);

    //开始服务器监听
    BOOL Listen(int backlog = 5);

    //接收收据(返回接收的字节数，接收错误时返回-1)
    virtual int Receive(char* buf, int nBufLen, int flags = 0);

    //UDP接收数据(返回接收的字节数，接收错误时返回-1)
    int ReceiveFrom(char* buf, int nBufLen, SOCKADDR* pSockAddr, int* pSockAddrLen, int nFlags = 0);
    int ReceiveFrom(char* buf, int nBufLen, std::string& rSocketAddress, int& rSocketPort, int nFlags = 0);

    //关闭套接字相应功能
    enum {RECVIVES = 0, SENDS = 1, BOTH = 2};
    BOOL ShutDown(int nHow = SENDS);

    //发送数据(返回发送的字节数，发送错误时返回-1)
    virtual int Send(const char* buf, int nBufLen, int flags = 0);
    
    //UDP发送数据(返回发送的字节数，发送错误时返回-1,地址为空时则广播)
    int SendTo(const char* buf, int nBufLen, int nPort, char *strAddress = NULL, int nFlags = 0);
    int SendTo(const char* buf, int nBufLen, const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags = 0);

    //接收固定长度数据，接收完成或超时才返回(超时单位为毫秒)
    BOOL ReceiveFixedLenData(char* buf, int nBufLen, int nTimeout);
    //接收固定长度数据，接收完成或超时才返回
    BOOL ReceiveFixedLenData(char* buf, int nBufLen);

    //发送固定长度数据，发送完或超时才返回(超时单位为毫秒)
    BOOL SendFixedLenData(const char* buf, int nBufLen, int nTimeout);
    //发送固定长度数据，发送完或出错时才返回
    BOOL SendFixedLenData(const char* buf, int nBufLen);

 
private:
    BOOL Socket(int domain, int type, int protocol);

protected:
    SOCKET m_hSocket;                    //SOCKET描述符

};

}

#endif
