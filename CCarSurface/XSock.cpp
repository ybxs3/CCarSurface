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
	//�ɹ���ʼ��
	return (SOCKET_SUCCESS);
}

//�����׽�������
//���:�׽���
//����:�����ȷ��ô����0�����󷵻�-1
int XSock::SetSocketOption(SOCKET hSocket)
{
    int nActivate = 1;
 //�����ַ����
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

//�����׽����Ƿ�Ϊ������
//���:�׽���,�Ƿ���Ҫ������
//����:�����ȷ��ô����0�����󷵻�-1
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

//�ر��׽���
//���:�׽���,�Ƿ�ǿ�йر�(���bHardClose==FALSE,��ô����ʣ������ݺ�ر�����)
void XSock::CloseSocket(SOCKET hSocket, BOOL bHardClose)
{
	//����Ҫ�������
	if (!bHardClose) //���Źر� Graceful close
	{
		//���ٷ������ݣ�����TCP�׽��֣������е����ݶ��������֮��
		//������һ�� FIN ��֪ͨ���շ����������Ѿ�������ϡ�
		shutdown(hSocket, SD_SEND);

		//���ջ������п��ܻ���δ���յ����ݣ��ڹر��׽���֮ǰӦ����
		//��ȡ���������ݡ�
		int  nRecvResult;
		HANDLE hSocketEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		//Ϊ���������ṩ�Ļ�����
		char szBuffer[256];
		do
		{
			if (hSocketEvent != NULL)
			{
				//ע�������¼�
				WSAEventSelect(hSocket,(WSAEVENT) hSocketEvent, FD_READ | FD_CLOSE);
				WSAWaitForMultipleEvents(1,&hSocketEvent, TRUE, _SHUTDOWN_RECV_TIMEOUT, TRUE);
				//��������¼�
				WSAEventSelect(hSocket, (WSAEVENT) hSocketEvent, 0);
			}
			ZeroMemory(szBuffer,256);
			//���ղ�������
			nRecvResult = RecvData(hSocket, szBuffer, sizeof(szBuffer));
		} while (nRecvResult > 0);

		if (hSocketEvent != NULL)
			CloseHandle(hSocketEvent);
			//����������պͷ���
		shutdown(hSocket, SD_BOTH);
	}
	// �ر��׽���
	if (0 != hSocket)
		closesocket(hSocket);
}

// ���׽���
//���:�׽���,�󶨵ĵ�ַ��Ϣ,����
//����:��ȷ0������-1
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

// ���׽���
//���:�׽���,�˿ں�
//����:��ȷ0,����-1
int XSock::BindSocketEx(SOCKET hSocket,int nPort)
{
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr,sizeof(sockAddr));

	sockAddr.sin_family   = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port   = htons((u_short)nPort);

	return BindSocket(hSocket,(SOCKADDR *)&sockAddr, sizeof(sockAddr));
}

//���������ص�IO�������׽���
//���:Э�飬Э������(TCP/U),Э��
//����:���ش������ص�IO SOCKET
//ע��:ʹ��SOCKET()�����������׽���Ĭ�Ͼ����ص�IO����
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

	//�����׽���ѡ��
	if ( SOCKET_ERROR == SetSocketOption(hSocket) ) //��������ʧ��
	{
		CloseSocket(hSocket, TRUE);
		return (INVALID_SOCKET);
	}
	return (hSocket);
}

//����ACCEPT��û����Ӧ������
//���:�׽��֣�������ַ������
//����:��ȷ���ض˿ںţ����򷵻�INVALID_SOCKET
SOCKET XSock::Accept_Block(SOCKET hSocket, struct sockaddr* pSocketAddress,
         int *nAddrLen)
{
	if (INVALID_SOCKET == hSocket)
		return (INVALID_SOCKET);
	SOCKET hAccept = accept(hSocket, pSocketAddress, nAddrLen);

	//����ö˿ڴ���
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
		goto CLEAR;//ѡ���ͳ�ʱ
	}else if (SOCKET_ERROR == iError)
	{
	}else
	{
		hAccept = accept(hSocket, pSocketAddress, nAddrLen);
		//����ö˿ڴ���
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
	SetLastError(WSAGetLastError());//��ʱ
	return(INVALID_SOCKET);
}

//�����׽���
//���:�׽���,����ĵȴ����г���
//����:SOCKET_ERROR/SOCKET_SUCCESS
int XSock::ListenSocket(SOCKET hSocket, int nConnections)
{
	if(listen(hSocket, nConnections) == SOCKET_ERROR)
	{
		SetLastError( WSAGetLastError() );
		return (SOCKET_ERROR);
	}
	return (SOCKET_SUCCESS);
}

//�����������ݣ��ú���ֹͣ���ڸö˿ڱ��ر�(���ճ���Ϊ0)����������ʱ��
//���:�׽���,���ջ������ͳ���,��ʱ��
//����:��ȷ���ؽ��ճ���,���򷵻�SOCKET_ERROR
int XSock::SelectData_Block(SOCKET hSocket,  DWORD dwTimeout)
{
	if (hSocket==INVALID_SOCKET)
		return (SOCKET_ERROR);
	
	//dwTimeout = 1000*1000;
	FD_SET fd = {1, hSocket};												
	TIMEVAL tv = {dwTimeout, 0};											//��ʱʱ��
	int nBytesReceived=0;
	int nSelectRet = select(0, &fd, NULL, NULL, &tv);
	//���ش���0��ֵʱ,��ʾ�����������SOCKET��
	//����0��ʾ��ʱ
	//ʧ��ʱ����SOCKET_ERROR
	if (SOCKET_ERROR == nSelectRet)//�׽��ִ���
		goto CLEAR;
	
	return nSelectRet;

CLEAR:
	SetLastError(WSAGetLastError());//��ʱ
	return(SOCKET_ERROR);
}

//�����������ݣ��ú���ֹͣ���ڸö˿ڱ��ر�(���ճ���Ϊ0)����������ʱ��
//���:�׽���,���ջ������ͳ���,��ʱ��
//����:��ȷ���ؽ��ճ���,���򷵻�SOCKET_ERROR
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
	//���ش���0��ֵʱ,��ʾ�����������SOCKET��
	//����0��ʾ��ʱ
	//ʧ��ʱ����SOCKET_ERROR
	if (0 == nSelectRet  || //��ʱ
		SOCKET_ERROR == nSelectRet)//�׽��ִ���
		goto CLEAR;

	if((nBytesReceived = recv(hSocket, pszBuffer, nBufferSize, 0)) == SOCKET_ERROR)
		goto CLEAR;
	//�ɹ�ʱ�����յ����ֽ���.
	//������ӱ��ж��򷵻�0
	//ʧ��ʱ���� SOCKET_ERROR
	return nBytesReceived;

	CLEAR:
	SetLastError(WSAGetLastError());//��ʱ
	return(SOCKET_ERROR);
}

//�����������ݣ�ע���������������֮ǰ����ȷ���Ƿ��н�����Ϣ����
//���:�׽���,���ݻ�����,��������С
//����:�����ȷ��ô���ؽ��յ��ֽ����������󷵻ش������
int XSock::RecvData(SOCKET hSocket, char *pszBuffer, int nBufferSize)
{
	if (hSocket==INVALID_SOCKET || pszBuffer==NULL)
		return (SOCKET_ERROR);

	DWORD  dwRtxBytes = 0,
	dwRtxFlags = 0;
	WSABUF  WSABuff;

	//��ջ���
	ZeroMemory(&WSABuff,sizeof(WSABUF));

	WSABuff.len = nBufferSize;
	WSABuff.buf = pszBuffer;
	//�����ȷ�ͷ��ر��ν��յ��ֽڸ�����������󷵻ش������(����)
	return ((WSARecv(hSocket, &WSABuff, 1, &dwRtxBytes, &dwRtxFlags,NULL, NULL)
			== SOCKET_SUCCESS) ? (int) dwRtxBytes : -WSAGetLastError());
}

//��������(����ֱ���յ�����Ϊֹ)
//���:�׽���,���ݻ�����,��������С,��ʱ��
//����:�����ȷ��ô���ؽ��յ��ֽ����������󷵻ش������
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
		// ע��FD_READ | FD_CLOSE �¼�
		// (��Ϊ�����ڵȴ�FD_READ�¼��У��Է��ر��׽��֣�����Ҫ��עFD_CLOSE)
		if( WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, FD_READ | FD_CLOSE)
			== SOCKET_ERROR)
		{
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_ERROR);
		}

		// �ȵ�FD_READ | FD_CLOSE�¼��ķ���
		dwWaitResult = WSAWaitForMultipleEvents(1, &hReadEvent, TRUE,dwTimeout, TRUE);

		if (dwWaitResult != WSA_WAIT_EVENT_0)
		{
			// ����¼�
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_ERROR);
		}

		//////////////////////////////////////////////////////////////
		/// ע�⣺��ʹ dwWaitResult == WSA_WAIT_EVENT0 ��ҲӦ��
		///   ��һ����������Ƿ�������
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if(WSAEnumNetworkEvents(hSocket,(WSAEVENT)hReadEvent,&NetEvent) == SOCKET_ERROR)
		{
			// ����¼�
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_ERROR);
		}
		//�жϷ�����ʲô�¼� FD_READ �� FD_CLOSE 
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( NetEvent.lNetworkEvents == FD_READ &&
			  NetEvent.iErrorCode[FD_READ_BIT] !=0 ) ) // ��������
		{   
			// ����¼�
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError(WSAGetLastError() );
			return (SOCKET_ERROR);
		}
		////////////////////////////////////////////////////////////////
		// ����¼�
		WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);

		// ��������
		if ((nRecvBytes = RecvData(hSocket, pszBuffer, nBufferSize)) >= 0)
			break; // ����ѭ��
		//Recv���ص��Ǵ������ĸ�����������Ҫ��ת����
		int nErrorCode = -nRecvBytes;

		if ( nErrorCode != WSAEWOULDBLOCK ) //̫���δ����ص�����
		{
			CloseHandle(hReadEvent);
			SetLastError( nErrorCode );
			return (SOCKET_ERROR);
		}
		//����ס��
		Sleep(_BLOCKED_SNDRCV_SLEEP);
	}
	CloseHandle(hReadEvent);
	return (nRecvBytes);
}

//�������ݣ�����
//���:�׽��֣����͵��ִ�,�ִ����ȣ���ʱֵ
int XSock::Send_Block(SOCKET hSocket,char const * pszBuffer,
  int nBufferSize, DWORD dwTimeout)
{
	if (hSocket==INVALID_SOCKET || pszBuffer==NULL)
		return (SOCKET_ERROR);

	FD_SET fd = {1, hSocket};
	TIMEVAL tv = {dwTimeout, 0};
	int nBytesSent=0;
	if(select(0, NULL, &fd, NULL, &tv) == 0)
		goto CLEAR;//ѡ���ͳ�ʱ
	if((nBytesSent = send(hSocket, pszBuffer, nBufferSize, 0)) == SOCKET_ERROR)
		goto CLEAR;//���ͳ�����
	return nBytesSent;

	CLEAR:
	SetLastError(WSAGetLastError());//��ʱ
	return(SOCKET_ERROR);
}

//����ȫ�������������ݣ�����
//���:�׽��֣����͵��ִ�,�ִ����ȣ���ʱֵ
//����:��ȷ���ط��͵��ֽ����������󷵻�SOCKET_ERROR
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
		//���һ��û�з��ͳɹ�
		nBytesSent += nBytesThisTime;
		//�ı䵱ǰ�ַ�ָ��
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
