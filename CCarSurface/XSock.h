// SockObj.h: interface for the XSock class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(XSOCKET_H)
#define XSOCKET_H
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

const int SOCKET_SUCCESS = 0;					//与SOCKET_ERROR对应的成功标志
const int _SOCKET_MAJOR_VERSION = 2;
const int _SOCKET_MINOR_VERSION = 2;
const int _LISTEN_COUNT = 200;
const DWORD _SHUTDOWN_RECV_TIMEOUT = 4*1000;	// 完美关闭时延迟4秒
const DWORD _RECV_TIMEOUT = 12;				    // 12秒 接收超时
const DWORD _SEND_TIMEOUT = 12;					// 12秒 发送超时
const DWORD _ACCEPT_TIMEOUT = 12;				// 12秒 接受超时
const DWORD _BLOCKED_SNDRCV_SLEEP = 100;		// 100毫秒(发生阻塞时等待[睡眠]时间)
const DWORD _RECV_BUFF_LEN = 8192;


bool GetIpBySock(SOCKET Sock, char* IP);

class XSock 
{
public:
	XSock();
	virtual ~XSock();

public:
	static void CleanupLibrary(void);
	static int InitLibrary(void);

protected:
	int m_nErrorNo;

public:
	// 创建具有重叠IO能力的套接字
	SOCKET CreateSocket(int nAddressFamily= AF_INET,
						int nType= SOCK_STREAM,
						int nProtocol= 0);
	// 设置套接字属性
	int SetSocketOption(SOCKET hSocket);

	// 设置套接字是否为阻塞的
	int BlockSocket(SOCKET hSocket, BOOL bBlock);

	// 绑定套接字
	int BindSocketEx(SOCKET hSocket,int nPort);
	// 基本参数绑定套接字
	int BindSocket(SOCKET hSocket, struct sockaddr * pSocketAddress, int nAddrLen);
	// 连接套接字
	int Connect(SOCKET hSocket, const char* pIP, int nPort);
	// 监听套接字
	int ListenSocket(SOCKET hSocket, int nConnections=_LISTEN_COUNT);
	// 阻塞ACCEPT，没有响应不返回
	SOCKET Accept_Block(SOCKET hSocket, struct sockaddr* pSocketAddress=NULL, int *nAddrLen=NULL);

	// 异步ACCEPT，超时返回
	SOCKET Accept(SOCKET hSocket, struct sockaddr* pSocketAddress=NULL, int *nAddrLen=NULL,
	DWORD dwTimeout=_ACCEPT_TIMEOUT);

	// 
	int SelectData_Block(SOCKET hSocket,  DWORD dwTimeout);
	// int RecvDataFrom_Block( SOCKET hSocket, struct sockaddr* pFrom,  int nAddrlen,
	// char* pszBuffer, int nBufferSize, DWORD dwTimeout=_RECV_TIMEOUT);
	// 阻塞接收数据，该函数停止是在该端口被关闭(接收长度为0)，或出错误的时候
	int RecvData_Block(SOCKET hSocket, char *pszBuffer, int nBufferSize,
	DWORD dwTimeout= _RECV_TIMEOUT);

	int RecvData_Event(SOCKET hSocket, char *pszBuffer, int nBufferSize,
	DWORD dwTimeout= _RECV_TIMEOUT);
	// 接收所有数据，注意在这个函数调用之前必须确认是否有接收消息到来
	int RecvData(SOCKET hSocket, char *pszBuffer, int nBufferSize);

	// 发送全部缓冲区中数据，阻塞
	int SendData_Block(SOCKET hSocket,char const * pszBuffer, int nBufferSize,
	DWORD dwTimeout=_SEND_TIMEOUT);
	//发送数据，阻塞
	int Send_Block(SOCKET hSocket, char const * pszBuffer,  int nBufferSize,
	DWORD dwTimeout=_SEND_TIMEOUT);

	// 发送一次缓冲区数据，不过有可能没有全部发送
	// int Send_Event(SOCKET hSocket, char const * pszBuffer, int nBufferSize,
	// DWORD dwTimeout=_SEND_TIMEOUT);
	// 关闭套接字
	void CloseSocket(SOCKET hSocket, BOOL bHardClose=FALSE);

	inline void SetLastError(int iErrorCode)//设置错误代码
	{
		InterlockedExchange((LPLONG)&m_nErrorNo, iErrorCode);  
	};

	inline int GetLastError()//获取最近一次操作的错误代码
	{
		int iErrorNo;
		InterlockedExchange((LPLONG)&iErrorNo, m_nErrorNo);
		return (iErrorNo);
	};

// add from smdp
	void RecevieData(SOCKET hSocket,const int &RecvCount, int &ReturnCount, char* const pRecvBuff);   //recevie data 
	void SendData(SOCKET hSocket,const int &SendCount, int &ReturnCount, char* const pSendBuff);      //send data 

};


#endif // !defined(AFX_INIFILE_H__B5C0D7F7_8353_4C93_AAA4_38A688CA253C__INCLUDED_)