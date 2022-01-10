#include "stdafx.h"
#include "XSock.h"


bool GetIpBySock(SOCKET Sock, char* IP)
{
	struct sockaddr sa;
	int len = sizeof(sa);

	if (0 != getpeername(Sock, &sa, &len))
		return false;

	for (size_t i =0; i<6; ++i) {
		IP[i] = sa.sa_data[i + 2];
	}

	return true;
}

XSock::XSock()
{
 
}

XSock::~XSock()
{
 //CleanupLibrary();
}

void XSock::CleanupLibrary(void)
{
	WSACleanup();
}

int XSock::InitLibrary(void)
{
	WSADATA WSD;
	WORD wVersionRequired = MAKEWORD( _SOCKET_MAJOR_VERSION, _SOCKET_MINOR_VERSION );
	ZeroMemory(&WSD,sizeof(WSADATA));
	int nErrorNo = WSAStartup(wVersionRequired, &WSD);
	if ( SOCKET_SUCCESS != nErrorNo )
	{
		//SetLastError( nErrorNo );
		return ( SOCKET_ERROR );
	}
	if ( LOBYTE( WSD.wVersion ) != _SOCKET_MINOR_VERSION ||
		HIBYTE( WSD.wVersion ) != _SOCKET_MAJOR_VERSION )
	{
		WSACleanup( );
		//SetLastError( WSAVERNOTSUPPORTED );
		return (SOCKET_ERROR);
	}
	//成功初始化
	return (SOCKET_SUCCESS);
}

//设置套接字属性
//入口:套接字
//出口:如果正确那么返回0，错误返回-1
int XSock::SetSocketOption(SOCKET hSocket)
{
    int nActivate = 1;
 //允许地址重用
    if (setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *) &nActivate,
		sizeof(nActivate)) == SOCKET_ERROR )
    {
        SetLastError( WSAGetLastError() );
        return (SOCKET_ERROR);//return (-1)
    }
	/*
	bool bDontLinger=true;
    if (setsockopt(hSocket, SOL_SOCKET, SO_DONTLINGER, (const char *) &bDontLinger,
		sizeof(bDontLinger)) == SOCKET_ERROR )
    {
        SetLastError( WSAGetLastError() );
        return (SOCKET_ERROR);//return (-1)
    }
  
	linger Linger={0,0};
    if (setsockopt(hSocket, SOL_SOCKET, SO_LINGER, (const char *) &Linger,
		sizeof(Linger)) == SOCKET_ERROR )
    {
        SetLastError( WSAGetLastError() );
        return (SOCKET_ERROR);//return (-1)
    }*/
	return (SOCKET_SUCCESS);
}

//设置套接字是否为阻塞的
//入口:套接字,是否需要阻塞的
//出口:如果正确那么返回0，错误返回-1
int XSock::BlockSocket(SOCKET hSocket, BOOL bBlock/*FALSE*/)
{
	u_long IoctlLong = (bBlock) ? 0 : 1;
	if (ioctlsocket(hSocket, FIONBIO, &IoctlLong) == SOCKET_ERROR)
	{
		SetLastError( WSAGetLastError() );
		return (SOCKET_ERROR);
	}
	return (SOCKET_SUCCESS);
}

int XSock::Connect(SOCKET hSocket, const char* pIP, int nPort)
{
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr,sizeof(sockAddr));

	sockAddr.sin_family   = AF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr(pIP);
	inet_pton(AF_INET, pIP, &(sockAddr.sin_addr));
	sockAddr.sin_port   = htons((u_short)nPort);

	return connect(hSocket, (struct sockaddr*)&sockAddr, sizeof(SOCKADDR_IN));
}

//关闭套接字
//入口:套接字,是否强行关闭(如果bHardClose==FALSE,那么接收剩余的数据后关闭连接)
void XSock::CloseSocket(SOCKET hSocket, BOOL bHardClose)
{
	//不需要捕获错误
	if (!bHardClose) //优雅关闭 Graceful close
	{
		//不再发送数据，对于TCP套接字，在所有的数据都发送完毕之后，
		//将发送一个 FIN ，通知接收方所有数据已经发送完毕。
		shutdown(hSocket, SD_SEND);

		//接收缓冲区有可能还有未接收的数据，在关闭套接字之前应该先
		//读取残留的数据。
		int  nRecvResult;
		HANDLE hSocketEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		//为残留数据提供的缓冲区
		char szBuffer[256];
		do
		{
			if (hSocketEvent != NULL)
			{
				//注册网络事件
				WSAEventSelect(hSocket,(WSAEVENT) hSocketEvent, FD_READ | FD_CLOSE);
				WSAWaitForMultipleEvents(1,&hSocketEvent, TRUE, _SHUTDOWN_RECV_TIMEOUT, TRUE);
				//清除网络事件
				WSAEventSelect(hSocket, (WSAEVENT) hSocketEvent, 0);
			}
			ZeroMemory(szBuffer,256);
			//接收残留数据
			nRecvResult = RecvData(hSocket, szBuffer, sizeof(szBuffer));
		} while (nRecvResult > 0);

		if (hSocketEvent != NULL)
			CloseHandle(hSocketEvent);
			//不再允许接收和发送
		shutdown(hSocket, SD_BOTH);
	}
	// 关闭套接字
	if (0 != hSocket)
		closesocket(hSocket);
}

// 绑定套接字
//入口:套接字,绑定的地址信息,长度
//出口:正确0，错误-1
int XSock::BindSocket(SOCKET hSocket, struct sockaddr * pSocketAddress,
        int nAddrLen)
{
	if (bind(hSocket, pSocketAddress, nAddrLen) == SOCKET_ERROR)
	{
		SetLastError( WSAGetLastError() );
		return (SOCKET_ERROR);
	}
	return (SOCKET_SUCCESS);
}

// 绑定套接字
//入口:套接字,端口号
//出口:正确0,错误-1
int XSock::BindSocketEx(SOCKET hSocket,int nPort)
{
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr,sizeof(sockAddr));

	sockAddr.sin_family   = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port   = htons((u_short)nPort);

	return BindSocket(hSocket,(SOCKADDR *)&sockAddr, sizeof(sockAddr));
}

//创建具有重叠IO能力的套接字
//入口:协议，协议类型(TCP/U),协议
//出口:返回创建的重叠IO SOCKET
//注意:使用SOCKET()函数创建的套接字默认具有重叠IO能力
SOCKET XSock::CreateSocket(int nAddressFamily /*= AF_INET*/,
           int nType/*= SOCK_STREAM*/,
           int nProtocol/*= 0*/)
{
	SOCKET hSocket = WSASocket(nAddressFamily, nType, nProtocol,
	NULL,0,WSA_FLAG_OVERLAPPED); 
	if ( hSocket == INVALID_SOCKET )
	{
		SetLastError( WSAGetLastError() );
		return (INVALID_SOCKET);
	}

	//设置套接字选项
	if ( SOCKET_ERROR == SetSocketOption(hSocket) ) //设置属性失败
	{
		CloseSocket(hSocket, TRUE);
		return (INVALID_SOCKET);
	}
	return (hSocket);
}

//阻塞ACCEPT，没有响应不返回
//入口:套接字，主机地址，长度
//出口:正确返回端口号，否则返回INVALID_SOCKET
SOCKET XSock::Accept_Block(SOCKET hSocket, struct sockaddr* pSocketAddress,
         int *nAddrLen)
{
	if (INVALID_SOCKET == hSocket)
		return (INVALID_SOCKET);
	SOCKET hAccept = accept(hSocket, pSocketAddress, nAddrLen);

	//如果该端口错误
	if (INVALID_SOCKET == hAccept)
		SetLastError(WSAGetLastError());
	return hAccept;
}

SOCKET XSock::Accept(SOCKET hSocket, struct sockaddr* pSocketAddress, int *nAddrLen,
        DWORD dwTimeout)
{ 
	if (INVALID_SOCKET == hSocket)
		return (SOCKET_ERROR);

	//dwTimeout = 1000*1000;
	SOCKET hAccept;
	bool bNodelay;
	FD_SET fd = {1, hSocket};
	TIMEVAL tv = {dwTimeout, 0};
	unsigned long flag = 1;
	int nBytesSent=0;

	int iError = select(0, &fd, NULL, NULL, &tv);
	if(iError == 0)
	{
		goto CLEAR;//选择发送超时
	}else if (SOCKET_ERROR == iError)
	{
	}else
	{
		hAccept = accept(hSocket, pSocketAddress, nAddrLen);
		//如果该端口错误
		if (INVALID_SOCKET == hAccept)
			SetLastError(WSAGetLastError());  
		else
		{
			setsockopt(hAccept,IPPROTO_TCP,TCP_NODELAY,(char *)&bNodelay,sizeof(bNodelay));
			//ioctlsocket(hAccept, FIONBIO, &flag);
		}
		return hAccept;
	}
	CLEAR:
	SetLastError(WSAGetLastError());//超时
	return(INVALID_SOCKET);
}

//监听套接字
//入口:套接字,接入的等待队列长度
//出口:SOCKET_ERROR/SOCKET_SUCCESS
int XSock::ListenSocket(SOCKET hSocket, int nConnections)
{
	if(listen(hSocket, nConnections) == SOCKET_ERROR)
	{
		SetLastError( WSAGetLastError() );
		return (SOCKET_ERROR);
	}
	return (SOCKET_SUCCESS);
}

//阻塞接收数据，该函数停止是在该端口被关闭(接收长度为0)，或出错误的时候
//入口:套接字,接收缓冲区和长度,超时数
//出口:正确返回接收长度,否则返回SOCKET_ERROR
int XSock::SelectData_Block(SOCKET hSocket,  DWORD dwTimeout)
{
	if (hSocket==INVALID_SOCKET)
		return (SOCKET_ERROR);
	
	//dwTimeout = 1000*1000;
	FD_SET fd = {1, hSocket};												
	TIMEVAL tv = {dwTimeout, 0};											//超时时间
	int nBytesReceived=0;
	int nSelectRet = select(0, &fd, NULL, NULL, &tv);
	//返回大于0的值时,表示与条件相符的SOCKET数
	//返回0表示超时
	//失败时返回SOCKET_ERROR
	if (SOCKET_ERROR == nSelectRet)//套接字错误
		goto CLEAR;
	
	return nSelectRet;

CLEAR:
	SetLastError(WSAGetLastError());//超时
	return(SOCKET_ERROR);
}

//阻塞接收数据，该函数停止是在该端口被关闭(接收长度为0)，或出错误的时候
//入口:套接字,接收缓冲区和长度,超时数
//出口:正确返回接收长度,否则返回SOCKET_ERROR
int XSock::RecvData_Block(SOCKET hSocket, char *pszBuffer, int nBufferSize,
  DWORD dwTimeout)
{
	if (hSocket==INVALID_SOCKET || pszBuffer==NULL)
		return (SOCKET_ERROR);

	//dwTimeout = 1000*1000;
	FD_SET fd = {1, hSocket};
	TIMEVAL tv = {dwTimeout, 0};
	int nBytesReceived=0;
	int nSelectRet = select(0, &fd, NULL, NULL, &tv);
	//返回大于0的值时,表示与条件相符的SOCKET数
	//返回0表示超时
	//失败时返回SOCKET_ERROR
	if (0 == nSelectRet  || //超时
		SOCKET_ERROR == nSelectRet)//套接字错误
		goto CLEAR;

	if((nBytesReceived = recv(hSocket, pszBuffer, nBufferSize, 0)) == SOCKET_ERROR)
		goto CLEAR;
	//成功时返回收到的字节数.
	//如果连接被中断则返回0
	//失败时返回 SOCKET_ERROR
	return nBytesReceived;

	CLEAR:
	SetLastError(WSAGetLastError());//超时
	return(SOCKET_ERROR);
}

//接收所有数据，注意在这个函数调用之前必须确认是否有接收消息到来
//入口:套接字,数据缓冲区,缓冲区大小
//出口:如果正确那么返回接收的字节数量，错误返回错误代码
int XSock::RecvData(SOCKET hSocket, char *pszBuffer, int nBufferSize)
{
	if (hSocket==INVALID_SOCKET || pszBuffer==NULL)
		return (SOCKET_ERROR);

	DWORD  dwRtxBytes = 0,
	dwRtxFlags = 0;
	WSABUF  WSABuff;

	//清空缓冲
	ZeroMemory(&WSABuff,sizeof(WSABUF));

	WSABuff.len = nBufferSize;
	WSABuff.buf = pszBuffer;
	//如果正确就返回本次接收的字节个数，如果错误返回错误号码(负数)
	return ((WSARecv(hSocket, &WSABuff, 1, &dwRtxBytes, &dwRtxFlags,NULL, NULL)
			== SOCKET_SUCCESS) ? (int) dwRtxBytes : -WSAGetLastError());
}

//接收数据(阻塞直至收到数据为止)
//入口:套接字,数据缓冲区,缓冲区大小,超时数
//出口:如果正确那么返回接收的字节数量，错误返回错误代码
int XSock::RecvData_Event(SOCKET hSocket, char *pszBuffer,
          int nBufferSize, DWORD dwTimeout)
{
	if (hSocket==INVALID_SOCKET || pszBuffer==NULL)
		return (SOCKET_ERROR);

	HANDLE hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hReadEvent == NULL)
	{
		SetLastError( (int)GetLastError() );
		return ( SOCKET_ERROR );
	}

	int  nRecvBytes = 0;
	DWORD dwWaitResult ;
	for (;;)
	{
		// 注册FD_READ | FD_CLOSE 事件
		// (因为可能在等待FD_READ事件中，对方关闭套接字，所以要关注FD_CLOSE)
		if( WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, FD_READ | FD_CLOSE)
			== SOCKET_ERROR)
		{
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_ERROR);
		}

		// 等等FD_READ | FD_CLOSE事件的发生
		dwWaitResult = WSAWaitForMultipleEvents(1, &hReadEvent, TRUE,dwTimeout, TRUE);

		if (dwWaitResult != WSA_WAIT_EVENT_0)
		{
			// 清除事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_ERROR);
		}

		//////////////////////////////////////////////////////////////
		/// 注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该
		///   进一步检查网络是否发生错误
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if(WSAEnumNetworkEvents(hSocket,(WSAEVENT)hReadEvent,&NetEvent) == SOCKET_ERROR)
		{
			// 清除事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_ERROR);
		}
		//判断发生了什么事件 FD_READ 或 FD_CLOSE 
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( NetEvent.lNetworkEvents == FD_READ &&
			  NetEvent.iErrorCode[FD_READ_BIT] !=0 ) ) // 发生错误
		{   
			// 清除事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError(WSAGetLastError() );
			return (SOCKET_ERROR);
		}
		////////////////////////////////////////////////////////////////
		// 清除事件
		WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);

		// 接收数据
		if ((nRecvBytes = RecvData(hSocket, pszBuffer, nBufferSize)) >= 0)
			break; // 跳出循环
		//Recv返回的是错误代码的负数，所以需要调转过来
		int nErrorCode = -nRecvBytes;

		if ( nErrorCode != WSAEWOULDBLOCK ) //太多的未完成重叠操作
		{
			CloseHandle(hReadEvent);
			SetLastError( nErrorCode );
			return (SOCKET_ERROR);
		}
		//阻塞住了
		Sleep(_BLOCKED_SNDRCV_SLEEP);
	}
	CloseHandle(hReadEvent);
	return (nRecvBytes);
}

//发送数据，阻塞
//入口:套接字，发送的字串,字串长度，超时值
int XSock::Send_Block(SOCKET hSocket,char const * pszBuffer,
  int nBufferSize, DWORD dwTimeout)
{
	if (hSocket==INVALID_SOCKET || pszBuffer==NULL)
		return (SOCKET_ERROR);

	FD_SET fd = {1, hSocket};
	TIMEVAL tv = {dwTimeout, 0};
	int nBytesSent=0;
	if(select(0, NULL, &fd, NULL, &tv) == 0)
		goto CLEAR;//选择发送超时
	if((nBytesSent = send(hSocket, pszBuffer, nBufferSize, 0)) == SOCKET_ERROR)
		goto CLEAR;//发送出错误
	return nBytesSent;

	CLEAR:
	SetLastError(WSAGetLastError());//超时
	return(SOCKET_ERROR);
}

//发送全部缓冲区中数据，阻塞
//入口:套接字，发送的字串,字串长度，超时值
//出口:正确返回发送的字节数量，错误返回SOCKET_ERROR
int XSock::SendData_Block(SOCKET hSocket,char const * pszBuffer,
  int nBufferSize, DWORD dwTimeout)
{
	if (hSocket==INVALID_SOCKET || pszBuffer==NULL)
		return (SOCKET_ERROR);

	int nBytesSent = 0;
	int nBytesThisTime;
	const char* pszTemp = pszBuffer;
	do {
		nBytesThisTime = Send_Block(hSocket,pszTemp, nBufferSize-nBytesSent, dwTimeout);
		if (nBytesThisTime < 0)
			return(SOCKET_ERROR);
		//如果一次没有发送成功
		nBytesSent += nBytesThisTime;
		//改变当前字符指针
		pszTemp += nBytesThisTime;
	} while(nBytesSent < nBufferSize);
	
	return nBytesSent;
}


void XSock::RecevieData(SOCKET hSocket,const int &RecvCount, int &ReturnCount, char* const pRecvBuff)
{	
	fd_set fread;
	int index = 0;
	int ret = 0;
	
	timeval tval;
	tval.tv_sec = 1;
	tval.tv_usec = 0;
	
	int iTempRecvCount = RecvCount;
	while( iTempRecvCount > 0 )
	{   
		FD_ZERO(&fread);
		FD_SET(hSocket,&fread);
		ret = select(0, &fread, NULL, NULL, &tval);
		if( ret == 0 ){
			ReturnCount = 0;
			continue;
		} else if ( SOCKET_ERROR==ret ) {
			LPVOID lpMsgBuf = NULL;	
			ReturnCount = 0;
			return;
		}
		
		if( ret > 0)
		{
			if( FD_ISSET(hSocket,&fread) )
			{
				ret = recv( hSocket,&pRecvBuff[index],iTempRecvCount,0);
				int ntemp = WSAGetLastError();
				
				if( ret == 0) {
					ReturnCount = 0;
					return;
				}else if( ret == SOCKET_ERROR ) {
					ReturnCount = 0;
					return;
				}
				
				index += ret;
				iTempRecvCount -= ret;
			}
			
		}
	}

	if( index == RecvCount ){
       ReturnCount = RecvCount;
	   return;
	}
	else{  
       ReturnCount = 0;
	   return;
	}
}

void XSock::SendData(SOCKET hSocket,const int &SendCount, int &ReturnCount, char* const pSendBuff)
{
	fd_set fwrite;
	int iTempSendCount = SendCount;
	int index = 0;
	int ret;
	
	timeval tval;
	tval.tv_sec = 1;
	tval.tv_usec = 0;
	
	while( iTempSendCount > 0 )
	{
		FD_ZERO(&fwrite);
		FD_SET(hSocket,&fwrite);
		ret = select(0, NULL, &fwrite, NULL, &tval);
		if ( ret == 0 ) {
			ReturnCount = 0;
			continue;//Alter by TGM			
		} else if ( ret == SOCKET_ERROR ) {
			ReturnCount = 0;
			return;
		}
		
		if( ret > 0) {
			if(FD_ISSET(hSocket,&fwrite)) {
				ret = send( hSocket,&pSendBuff[index],iTempSendCount,0);
				if( ret == 0) {
					ReturnCount = 0;
					return;
				}else if( ret == SOCKET_ERROR ) {
					ReturnCount = 0;
					return;
				}
				
				index += ret;
				iTempSendCount -= ret;
			}
		}
	}
	
	if( index == SendCount ) {
		ReturnCount = SendCount;
		return;
	} else {  
		ReturnCount = 0;
		return;
	}
}
