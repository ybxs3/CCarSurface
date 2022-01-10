// SockObj.h: interface for the XSock class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(XSOCKET_H)
#define XSOCKET_H
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

const int SOCKET_SUCCESS = 0;					//��SOCKET_ERROR��Ӧ�ĳɹ���־
const int _SOCKET_MAJOR_VERSION = 2;
const int _SOCKET_MINOR_VERSION = 2;
const int _LISTEN_COUNT = 200;
const DWORD _SHUTDOWN_RECV_TIMEOUT = 4*1000;	// �����ر�ʱ�ӳ�4��
const DWORD _RECV_TIMEOUT = 12;				    // 12�� ���ճ�ʱ
const DWORD _SEND_TIMEOUT = 12;					// 12�� ���ͳ�ʱ
const DWORD _ACCEPT_TIMEOUT = 12;				// 12�� ���ܳ�ʱ
const DWORD _BLOCKED_SNDRCV_SLEEP = 100;		// 100����(��������ʱ�ȴ�[˯��]ʱ��)
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
	// ���������ص�IO�������׽���
	SOCKET CreateSocket(int nAddressFamily= AF_INET,
						int nType= SOCK_STREAM,
						int nProtocol= 0);
	// �����׽�������
	int SetSocketOption(SOCKET hSocket);

	// �����׽����Ƿ�Ϊ������
	int BlockSocket(SOCKET hSocket, BOOL bBlock);

	// ���׽���
	int BindSocketEx(SOCKET hSocket,int nPort);
	// �����������׽���
	int BindSocket(SOCKET hSocket, struct sockaddr * pSocketAddress, int nAddrLen);
	// �����׽���
	int Connect(SOCKET hSocket, const char* pIP, int nPort);
	// �����׽���
	int ListenSocket(SOCKET hSocket, int nConnections=_LISTEN_COUNT);
	// ����ACCEPT��û����Ӧ������
	SOCKET Accept_Block(SOCKET hSocket, struct sockaddr* pSocketAddress=NULL, int *nAddrLen=NULL);

	// �첽ACCEPT����ʱ����
	SOCKET Accept(SOCKET hSocket, struct sockaddr* pSocketAddress=NULL, int *nAddrLen=NULL,
	DWORD dwTimeout=_ACCEPT_TIMEOUT);

	// 
	int SelectData_Block(SOCKET hSocket,  DWORD dwTimeout);
	// int RecvDataFrom_Block( SOCKET hSocket, struct sockaddr* pFrom,  int nAddrlen,
	// char* pszBuffer, int nBufferSize, DWORD dwTimeout=_RECV_TIMEOUT);
	// �����������ݣ��ú���ֹͣ���ڸö˿ڱ��ر�(���ճ���Ϊ0)����������ʱ��
	int RecvData_Block(SOCKET hSocket, char *pszBuffer, int nBufferSize,
	DWORD dwTimeout= _RECV_TIMEOUT);

	int RecvData_Event(SOCKET hSocket, char *pszBuffer, int nBufferSize,
	DWORD dwTimeout= _RECV_TIMEOUT);
	// �����������ݣ�ע���������������֮ǰ����ȷ���Ƿ��н�����Ϣ����
	int RecvData(SOCKET hSocket, char *pszBuffer, int nBufferSize);

	// ����ȫ�������������ݣ�����
	int SendData_Block(SOCKET hSocket,char const * pszBuffer, int nBufferSize,
	DWORD dwTimeout=_SEND_TIMEOUT);
	//�������ݣ�����
	int Send_Block(SOCKET hSocket, char const * pszBuffer,  int nBufferSize,
	DWORD dwTimeout=_SEND_TIMEOUT);

	// ����һ�λ��������ݣ������п���û��ȫ������
	// int Send_Event(SOCKET hSocket, char const * pszBuffer, int nBufferSize,
	// DWORD dwTimeout=_SEND_TIMEOUT);
	// �ر��׽���
	void CloseSocket(SOCKET hSocket, BOOL bHardClose=FALSE);

	inline void SetLastError(int iErrorCode)//���ô������
	{
		InterlockedExchange((LPLONG)&m_nErrorNo, iErrorCode);  
	};

	inline int GetLastError()//��ȡ���һ�β����Ĵ������
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