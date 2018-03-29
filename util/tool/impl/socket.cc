#include "../socket.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include  <sys/ioctl.h>  
#include  <netinet/in.h>  
#include <netinet/tcp.h> // for TCP_NODELAY
#include  <arpa/inet.h>  
#include  <netdb.h>  


namespace toolHelp
{

CSocket::CSocket()
{ 
    m_hSocket = INVALID_SOCKET;
}

CSocket::~CSocket()
{
    Close();
}

void CSocket::Close()
{
    if(m_hSocket != INVALID_SOCKET)
    {
        close(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
}

BOOL CSocket::Socket(int domain, int type, int protocol)
{
    assert(m_hSocket == INVALID_SOCKET);
    
    m_hSocket = socket(domain,type,protocol);
    if(m_hSocket == INVALID_SOCKET)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CSocket::SetNoBlockMode(BOOL bFlag)
{
    assert(m_hSocket != INVALID_SOCKET);

    int flags;
    
    flags = fcntl(m_hSocket, F_GETFL);
    if (flags < 0)
    {
        return FALSE;
    }
    
    if(bFlag) //设为非阻塞
    {
        flags |= O_NONBLOCK;
        if (fcntl(m_hSocket, F_SETFL, flags) < 0)
        {
            return FALSE;
        }
    }
    else //设为阻塞
    {
        flags &= ~O_NONBLOCK;
        if (fcntl(m_hSocket, F_SETFL, flags) < 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CSocket::Attach(SOCKET hSocket)
{
    assert(m_hSocket == INVALID_SOCKET);
    assert(hSocket != INVALID_SOCKET);

    m_hSocket = hSocket;
    
    return TRUE;
}

CSocket::operator SOCKET() const
{
    return m_hSocket;
}

SOCKET CSocket::Detach()
{
    SOCKET hSocket = INVALID_SOCKET;
    
    hSocket = m_hSocket;
    m_hSocket = INVALID_SOCKET;
    
    return hSocket;
}

BOOL CSocket::GetPeerName(std::string& rPeerAddress, int& rPeerPort)
{
    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    
    socklen_t nSockAddrLen = sizeof(sockAddr);
    int nResult = getpeername(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
    if (nResult != SOCKET_ERROR)
    {
        rPeerPort = ntohs(sockAddr.sin_port);
        rPeerAddress = inet_ntoa(sockAddr.sin_addr);
        return TRUE;
    }
    return FALSE;
}

BOOL CSocket::GetPeerName(SOCKADDR* pSockAddr, int* pSockAddrLen)
{
    return (SOCKET_ERROR != getpeername(m_hSocket,pSockAddr,(socklen_t*)pSockAddrLen));
}

BOOL CSocket::GetSockName(std::string& rSocketAddress, int& rSocketPort)
{
    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    
    socklen_t nSockAddrLen = sizeof(sockAddr);
    int nResult = getsockname(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
    if (nResult != SOCKET_ERROR)
    {
        rSocketPort = ntohs(sockAddr.sin_port);
        rSocketAddress = inet_ntoa(sockAddr.sin_addr);
        return TRUE;
    }
    return FALSE;
}

BOOL CSocket::GetSockName(SOCKADDR* pSockAddr, int* pSockAddrLen)
{
    return (SOCKET_ERROR != getsockname(m_hSocket,pSockAddr,(socklen_t*)pSockAddrLen));
}

BOOL CSocket::SetSockOpt(int nOptionName, const void* lpOptionValue,int nOptionLen, int nLevel)
{
    int nResult = setsockopt(m_hSocket,nLevel,nOptionName,lpOptionValue,nOptionLen);
    return (nResult != SOCKET_ERROR);
}

BOOL CSocket::GetSockOpt(int nOptionName, void* lpOptionValue,int* lpOptionLen, int nLevel)
{
    int nResult = getsockopt(m_hSocket,nLevel,nOptionName,lpOptionValue,(socklen_t*)lpOptionLen);
    return (nResult != SOCKET_ERROR);
}

BOOL CSocket::GetKeepAlive()
{
    int nValue;
    socklen_t nLength = sizeof(nValue);
    int nResult = getsockopt(m_hSocket,SOL_SOCKET,SO_KEEPALIVE,&nValue, &nLength);
    if(nResult != SOCKET_ERROR)
    {
        if(nValue == 1)
        {
            return TRUE;
        }
    }

    return FALSE;
}


void CSocket::SetKeepAlive(BOOL bFlag)
{
    int nValue = 0;
    nValue = (bFlag)? 1 : 0;

    socklen_t nLength = sizeof(nValue);
    setsockopt(m_hSocket,SOL_SOCKET,SO_KEEPALIVE,&nValue,nLength);
}

void CSocket::SetKeepAlive(int nKeepIdle, int nKeepInterval, int nKeepCount)
{    
    //int keepIdle = 60;    //开始首次KeepAlive探测前的TCP空闭时间(单位:秒)
    //int keepInterval = 5; //两次KeepAlive探测间的时间间隔(单位:秒)
    //int keepCount = 3;    //判定断开前的KeepAlive探测次数(单位:秒)

    int nKeepAlive = 1;  //设定KeepAlive
    setsockopt(m_hSocket,SOL_SOCKET,SO_KEEPALIVE,(void*)&nKeepAlive,sizeof(nKeepAlive));    
    setsockopt(m_hSocket,SOL_TCP,TCP_KEEPIDLE,(void *)&nKeepIdle,sizeof(nKeepIdle));
    setsockopt(m_hSocket,SOL_TCP,TCP_KEEPINTVL,(void *)&nKeepInterval,sizeof(nKeepInterval));
    setsockopt(m_hSocket,SOL_TCP,TCP_KEEPCNT,(void *)&nKeepCount,sizeof(nKeepCount));
}

void CSocket::SetLinger(BOOL bOpen, int nLingerTime)
{
    int nValue = 0;
    nValue = (bOpen) ? 1 : 0;
    
    struct linger so_linger;
    so_linger.l_onoff = nValue;
    so_linger.l_linger = nLingerTime;

    setsockopt(m_hSocket,SOL_SOCKET,SO_LINGER,&so_linger,sizeof(so_linger));
}

int  CSocket::GetReceiveBufferSize()
{
    int nValue;
    socklen_t nLength = sizeof(nValue);
    int nResult = getsockopt(m_hSocket,SOL_SOCKET,SO_RCVBUF,&nValue, &nLength);
    if(nResult != SOCKET_ERROR)
    {
       return nValue;
    }
    
    return SOCKET_ERROR;
}

void CSocket::SetReceiveBufferSize(int nSize)
{
    int nValue = nSize;
    
    socklen_t nLength = sizeof(nValue);
    setsockopt(m_hSocket,SOL_SOCKET,SO_RCVBUF,&nValue,nLength);
}

int  CSocket::GetSendBufferSize()
{
    int nValue;
    socklen_t nLength = sizeof(nValue);
    int nResult = getsockopt(m_hSocket,SOL_SOCKET,SO_SNDBUF,&nValue, &nLength);
    if(nResult != SOCKET_ERROR)
    {
        return nValue;
    }
    
    return SOCKET_ERROR;
}

void CSocket::SetSendBufferSize(int nSize)
{
    int nValue = nSize;
    
    socklen_t nLength = sizeof(nValue);
    setsockopt(m_hSocket,SOL_SOCKET,SO_SNDBUF,&nValue,nLength);
}


BOOL  CSocket::GetReuseAddress()
{
    int nValue;
    socklen_t nLength = sizeof(nValue);
    int nResult = getsockopt(m_hSocket,SOL_SOCKET,SO_REUSEADDR,&nValue, &nLength);
    if(nResult != SOCKET_ERROR)
    {
        if(nValue == 1)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}
 
void  CSocket::SetReuseAddress(BOOL bFlag)
{
    int nValue = 0;
    nValue = (bFlag)? 1 : 0;
    
    socklen_t nLength = sizeof(nValue);
    setsockopt(m_hSocket,SOL_SOCKET,SO_REUSEADDR,&nValue,nLength);
}

BOOL CSocket::GetBroadCast()
{
    int nValue;
    socklen_t nLength = sizeof(nValue);
    int nResult = getsockopt(m_hSocket,SOL_SOCKET,SO_BROADCAST,&nValue, &nLength);
    if(nResult != SOCKET_ERROR)
    {
        if(nValue == 1)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}


void CSocket::SetBroadCast(BOOL bFlag)
{
    int nValue = 0;
    nValue = (bFlag)? 1 : 0;
    
    socklen_t nLength = sizeof(nValue);
    setsockopt(m_hSocket,SOL_SOCKET,SO_BROADCAST,&nValue,nLength);
}


BOOL CSocket::GetTcpNoDelay()
{
    int nValue;
    socklen_t nLength = sizeof(nValue);
    int nResult = getsockopt(m_hSocket,IPPROTO_TCP,TCP_NODELAY,&nValue, &nLength);    
    if(nResult != SOCKET_ERROR)
    {
        if(nValue == 1)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}


void CSocket::SetTcpNoDelay(BOOL bFlag)
{
    int nValue = 0;
    nValue = (bFlag)? 1 : 0;
    
    socklen_t nLength = sizeof(nValue);
    setsockopt(m_hSocket,IPPROTO_TCP,TCP_NODELAY,&nValue,nLength);
}

int  CSocket::GetReceiveTimeout()
{
    timeval time;
    memset(&time,0,sizeof(time));
    
    socklen_t nLength = sizeof(time);
    int nResult = getsockopt(m_hSocket,SOL_SOCKET,SO_RCVTIMEO,&time, &nLength);
    if(nResult != SOCKET_ERROR)
    {
        return time.tv_sec * 1000 + time.tv_usec / 1000;
    }
    
    return SOCKET_ERROR;
}

void CSocket::SetReceiveTimeout(int nTimeout)
{
    timeval time;
    time.tv_sec = 0;
    time.tv_usec = nTimeout * 1000;
    
    socklen_t nLength = sizeof(time);
    setsockopt(m_hSocket,SOL_SOCKET,SO_RCVTIMEO,&time,nLength);
}

int  CSocket::GetSendTimeout()
{
    timeval time;
    memset(&time,0,sizeof(time));

    socklen_t nLength = sizeof(time);
    int nResult = getsockopt(m_hSocket,SOL_SOCKET,SO_SNDTIMEO,&time, &nLength);
    if(nResult != SOCKET_ERROR)
    {
        return  time.tv_sec * 1000 + time.tv_usec / 1000;
    }
    
    return SOCKET_ERROR;
}

void CSocket::SetSendTimeout(int nTimeout)
{
    timeval time;

#if 0
    time.tv_sec = 0;
    time.tv_usec = nTimeout * 1000;
#else
	time.tv_sec = 3;
    time.tv_usec = 0;
#endif
    
    socklen_t nLength = sizeof(time);
    setsockopt(m_hSocket,SOL_SOCKET,SO_SNDTIMEO,&time,nLength);
}

BOOL CSocket::Create(int port, int type, const char * strAddress)
{
    if(Socket(AF_INET,type,0))
    {
        if(port > 0)
        {
            if(Bind(port,strAddress))
            {
                return TRUE;
            }
        }
        else
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL CSocket::Bind(int port, const char * strAddress)
{
    SOCKADDR_IN socketAddr;
    
    socketAddr.sin_family = AF_INET;
    if(NULL == strAddress)
    {
        socketAddr.sin_addr.s_addr = htons(INADDR_ANY); 
    }
    else
    {
        socketAddr.sin_addr.s_addr = inet_addr(strAddress);
    }
    socketAddr.sin_port = htons(port);
    
    int nResult = bind(m_hSocket,(SOCKADDR*)&socketAddr,sizeof(SOCKADDR));           

    return (nResult != SOCKET_ERROR);
}

BOOL CSocket::Bind (const SOCKADDR* pSockAddr, int nSockAddrLen)
{
    return (SOCKET_ERROR != bind(m_hSocket,pSockAddr,nSockAddrLen));
}

BOOL CSocket::Accept(CSocket *pConnectedSocket,SOCKADDR* pSockAddr, int* pSockAddrLen)
{
    assert(pConnectedSocket != NULL);
    assert(pConnectedSocket->m_hSocket == INVALID_SOCKET);

    SOCKET hSocket = accept(m_hSocket,pSockAddr,(socklen_t*)pSockAddrLen);
    if(hSocket == INVALID_SOCKET)
    {
        pConnectedSocket->m_hSocket = INVALID_SOCKET;        
    }
    else
    {
        pConnectedSocket->m_hSocket = hSocket;
    }
    
    return (hSocket != INVALID_SOCKET);
}

BOOL CSocket::Accept(CSocket &rConnectedSocket,std::string& rPeerAddress, int& rPeerPort)
{
    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    
    int nSockAddrLen = sizeof(sockAddr);
    if(Accept(&rConnectedSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen))
    {
        rPeerPort = ntohs(sockAddr.sin_port);
        rPeerAddress = inet_ntoa(sockAddr.sin_addr);
        return TRUE;
    }
    
    return FALSE;
}

BOOL CSocket::Listen(int backlog)
{
    return (listen(m_hSocket,backlog) != SOCKET_ERROR);
}

BOOL CSocket::Connect(const char *strAddress, int nPort)
{
    assert(NULL != strAddress);

    SOCKADDR_IN sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(strAddress);

    if (sockAddr.sin_addr.s_addr == INADDR_NONE)
    {
        hostent* lphost = NULL;
        lphost = gethostbyname(strAddress);
        if (lphost != NULL)
        {
            sockAddr.sin_addr.s_addr = ((in_addr*)lphost->h_addr)->s_addr;
        }
        else
        {
            return FALSE;
        }
    }

    sockAddr.sin_port = htons(nPort);    
    int nResult = connect(m_hSocket,(const struct sockaddr *)&sockAddr,sizeof(sockAddr));

    return (nResult != SOCKET_ERROR);
}

BOOL CSocket::Connect(const char *strAddress, int nPort, int nTimeout)
{
    assert(NULL != strAddress);
    
    SOCKADDR_IN sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(strAddress);
    
    if (sockAddr.sin_addr.s_addr == INADDR_NONE)
    {
        hostent* lphost = NULL;
        lphost = gethostbyname(strAddress);
        if (lphost != NULL)
        {
            sockAddr.sin_addr.s_addr = ((in_addr*)lphost->h_addr)->s_addr;
        }
        else
        {
            return FALSE;
        }
    }
    
    sockAddr.sin_port = htons(nPort);    

    //获取当前设置的超时数据
    timeval time;
    memset(&time,0,sizeof(time));    
    socklen_t nLength = sizeof(time);
    getsockopt(m_hSocket,SOL_SOCKET,SO_SNDTIMEO,&time, &nLength);

    SetSendTimeout(nTimeout);
    int nResult = connect(m_hSocket,(const struct sockaddr *)&sockAddr,sizeof(sockAddr));
    //还原超时设置
    setsockopt(m_hSocket,SOL_SOCKET,SO_SNDTIMEO,&time,nLength);
    
    return (nResult != SOCKET_ERROR);
}

BOOL CSocket::Connect(const SOCKADDR* pSockAddr, int nSockAddrLen)
{
    return (SOCKET_ERROR != connect(m_hSocket,pSockAddr,nSockAddrLen));
}

int CSocket::Send(const char* buf, int nBufLen, int flags)
{
    return send(m_hSocket, buf, nBufLen, flags);
}

int CSocket::Receive(char* buf, int nBufLen, int flags)
{
    return recv(m_hSocket, buf, nBufLen, flags);
}

int CSocket::SendTo(const char* buf, int nBufLen, const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags)
{
    return sendto(m_hSocket, buf, nBufLen, nFlags, pSockAddr, nSockAddrLen);
}

int CSocket::SendTo(const char* buf, int nBufLen, int nPort, char *strAddress, int nFlags)
{
    SOCKADDR_IN sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    
    sockAddr.sin_family = AF_INET;

    if(NULL == strAddress)
    {
        sockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    }
    else
    {
        sockAddr.sin_addr.s_addr = inet_addr(strAddress);        
        if (sockAddr.sin_addr.s_addr == INADDR_NONE)
        {
            hostent* lphost = NULL;
            lphost = gethostbyname(strAddress);
            if (lphost != NULL)
            {
                sockAddr.sin_addr.s_addr = ((in_addr*)lphost->h_addr)->s_addr;
            }
            else
            {
                return SOCKET_ERROR;
            }
        }
    }
    
    sockAddr.sin_port = htons(nPort); 
    
    return SendTo(buf, nBufLen, (SOCKADDR*)&sockAddr, sizeof(sockAddr), nFlags);
}

int CSocket::ReceiveFrom(char* buf, int nBufLen, SOCKADDR* pSockAddr, int* pSockAddrLen, int nFlags)
{
    return recvfrom(m_hSocket, buf, nBufLen, nFlags, pSockAddr, (socklen_t*)pSockAddrLen);
}

int CSocket::ReceiveFrom(char* buf, int nBufLen, std::string& rSocketAddress, int& rSocketPort, int nFlags)
{
    SOCKADDR_IN sockAddr;
    
    memset(&sockAddr, 0, sizeof(sockAddr));
    
    int nSockAddrLen = sizeof(sockAddr);
    int nResult = ReceiveFrom(buf, nBufLen, (SOCKADDR*)&sockAddr, &nSockAddrLen, nFlags);
    if(nResult != SOCKET_ERROR)
    {
        rSocketPort = ntohs(sockAddr.sin_port);
        rSocketAddress = inet_ntoa(sockAddr.sin_addr);
    }
    return nResult;
}

BOOL CSocket::ShutDown(int nHow)
{
    return (shutdown(m_hSocket,nHow) != SOCKET_ERROR);
}

BOOL CSocket::ReceiveFixedLenData(char* buf, int nBufLen, int nTimeout)
{
    if(m_hSocket == INVALID_SOCKET || buf == NULL)
    {
        return FALSE;
    }

    int nRecvTime = 0;
    int nCount = 0;
    int nRemainLength = nBufLen;
    char *pBuffer = (char*)buf; 

    timeval tv_begin;
    timeval tv_end;
    
    //开始时间
    gettimeofday(&tv_begin, NULL);

    while(nRemainLength > 0)
    {
        gettimeofday(&tv_end, NULL);
        nRecvTime = (1000000 * tv_end.tv_sec + tv_end.tv_usec - 1000000 * tv_begin.tv_sec - tv_begin.tv_usec) / 1000;
        
        if(nRecvTime > nTimeout)  //超时，接收失败
        {
            return FALSE;
        }
        
        nCount = recv(m_hSocket,pBuffer,nRemainLength,0);
        if(nCount < 0)  //有错误发生
        {
            if(errno == EINTR) //中断发生，需要继续接收
            {
                continue;
            }
            else if(errno == EWOULDBLOCK || errno == EAGAIN ) //SOCKET非阻塞时，表示未有数据到达，需要等待一会
            {
                usleep(1000); //等待1毫秒
                continue;
            }
            else  //其它错误，接收失败
            {
                return FALSE;
            }
        }
        else if(nCount == 0) //对方已经关闭
        {
            return FALSE;
        }

        pBuffer += nCount;
        nRemainLength -= nCount;
    }

    return TRUE;
}
    
BOOL CSocket::ReceiveFixedLenData(char* buf, int nBufLen)
{
    if(m_hSocket == INVALID_SOCKET || buf == NULL)
    {
        return FALSE;
    }
    
    int nCount = 0;
    int nRemainLength = nBufLen;
    char *pBuffer = (char*)buf; 
    
    while(nRemainLength > 0)
    {
        nCount = recv(m_hSocket,pBuffer,nRemainLength,0);
        if(nCount < 0)  //有错误发生
        {
            if(errno == EINTR) //中断发生，需要继续接收
            {
                continue;
            }
            else if(errno == EWOULDBLOCK || errno == EAGAIN ) //SOCKET非阻塞时，表示未有数据到达，需要等待一会
            {
                usleep(1000); //等待1毫秒
                continue;
            }
            else  //其它错误，接收失败
            {
                return FALSE;
            }
        }
        else if(nCount == 0) //对方已经关闭
        {
            return FALSE;
        }
        
        pBuffer += nCount;
        nRemainLength -= nCount;
    }
    
    return TRUE;
}

BOOL CSocket::SendFixedLenData(const char* buf, int nBufLen, int nTimeout)
{
    if(m_hSocket == INVALID_SOCKET || buf == NULL)
    {
        return FALSE;
    }

    int nSendTime = 0;
    int nRemainLength = nBufLen; 
    int nCount = 0;
    char *pBuffer = (char*)buf; 

    timeval tv_begin;
    timeval tv_end;

    //开始时间
    gettimeofday(&tv_begin, NULL);
    
    while(nRemainLength > 0)  //还有数据未发完
    {
       gettimeofday(&tv_end, NULL);
       nSendTime = (1000000 * tv_end.tv_sec + tv_end.tv_usec - 1000000 * tv_begin.tv_sec - tv_begin.tv_usec) / 1000;

       if(nSendTime > nTimeout)  //超时，发送失败
       {
           return FALSE;
       }

       nCount = send(m_hSocket,pBuffer,nRemainLength,0);
       if(nCount < 0) //有错误发生
       {
           if(errno == EINTR) //中断发生，需要继续发送
           {
               continue;
           }
           else if(errno == EWOULDBLOCK || errno == EAGAIN ) //SOCKET非阻塞时，表示发送缓冲区满，需要等待一会
           {
               usleep(1000); //等待1毫秒
               continue;
           }
           else  //其它错误，发送失败
           {
               return FALSE;
           }
       }
       else if(nCount == 0) //对方已经关闭
       {
           return FALSE;
       }


       pBuffer += nCount;
       nRemainLength -= nCount;
    }

    return TRUE;
}

BOOL CSocket::SendFixedLenData(const char* buf, int nBufLen)
{
    if(m_hSocket == INVALID_SOCKET || buf == NULL)
    {
        return FALSE;
    }
    
    int nRemainLength = nBufLen; 
    int nCount = 0;
    char *pBuffer = (char*)buf; 
        
    while(nRemainLength > 0)  //还有数据未发完
    {
        nCount = send(m_hSocket,pBuffer,nRemainLength,0);
        if(nCount < 0) //有错误发生
        {
            if(errno == EINTR) //中断发生，需要继续发送
            {
                continue;
            }
            else if(errno == EWOULDBLOCK || errno == EAGAIN ) //SOCKET非阻塞时，表示发送缓冲区满，需要等待一会
            {
                usleep(1000); //等待1毫秒
                continue;
            }
            else  //其它错误，发送失败
            {
                return FALSE;
            }
        }
        else if(nCount == 0) //对方已经关闭
        {
            return FALSE;
        }

        
        pBuffer += nCount;
        nRemainLength -= nCount;
    }
    
    return TRUE;
}

}
