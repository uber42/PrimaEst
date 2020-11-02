/**
 * @file SocketWrapper.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

static WSADATA g_wsaData = { 0 };

#pragma pack(push)

/**
* Структура заголовок сообщения
*/
typedef struct _SSocketMessageHeader
{
	/** Размер сообщения */
	LONG64	dwSize;

	/** Хэш сообщения */
	DWORD	dwCrc32;
} SSocketMessageHeader, *PSSocketMessageHeader;


#pragma pack(pop)

/**
* Ожидание изменения состояния на чтение
* @param[in]	hSocket			Сокет
* @param[in]	pTimeout		Таймаут
* @return						Результат работы.
*/
BOOL
SocketReadSelect(
	SOCKET	 hSocket,
	PTIMEVAL pTimeout
);

/**
* Отправить данные по сокету
* @param[in] hSocket	Сокет
* @param[in] pBuffer	Буфер с данными
* @param[in] dwSize		Размер данных
* @return				Результат работы.
*/
BOOL
SocketSendInternal(
	SOCKET   hSocket,
	PBYTE	 pBuffer,
	DWORD	 dwSize
);

/**
* Принять данные по сокету
* @param[in]	hSocket			Сокет
* @param[in]	pBuffer			Буфер с данными
* @param[inout] dwSize			Размер данных
* @param[in]	pTimeout		Таймаут
* @return						Результат работы.
*/
BOOL
SocketReceiveInternal(
	SOCKET   hSocket,
	PBYTE    pBuffer,
	DWORD	 dwSize,
	PTIMEVAL pTimeout
);

/**
* Инициализировать механизм WSA
* @return	Результат работы.
*/
BOOL
InitializeWSA()
{
	INT nResult = WSAStartup(MAKEWORD(2, 2), &g_wsaData);
	if (nResult != 0)
	{
		LogError("Ошибка инициализации механизма сокетов : %d", WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

/**
* Деинициализировать механизм WSA
*/
VOID
DeinitializeWSA()
{
	WSACleanup();
}

/**
* Создать новый сокет
* @param[out] psSocket	Экзмепляр
* @param[in]  szAddress Адрес
* @param[in]  szPort	Порт
* @return				Результат работы.
*/
BOOL
CreateSocket(
	PSSocket psSocket,
	PCHAR	 szAddress,
	PCHAR	 szPort
)
{
	ADDRINFO sHints;

	memset(&sHints, 0, sizeof(ADDRINFO));
	sHints.ai_family = PF_INET;
	sHints.ai_socktype = SOCK_STREAM;
	sHints.ai_protocol = IPPROTO_TCP;
	sHints.ai_flags = AI_PASSIVE;

	psSocket->pAddrInfo = NULL;

	INT nResult = getaddrinfo(
		szAddress, szPort,
		&sHints, &psSocket->pAddrInfo);
	if (nResult != ERROR_SUCCESS)
	{
		LogError("[Socket Wrapper] Ошибка получения адреса : %d", WSAGetLastError());
		return FALSE;
	}

	psSocket->hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (psSocket->hSocket == INVALID_SOCKET)
	{
		freeaddrinfo(psSocket->pAddrInfo);

		LogError("[Socket Wrapper] Ошибка создания сокета : %d", WSAGetLastError());
		return FALSE;
	}
	
	DWORD dwOption = 1;
	nResult = setsockopt(
		psSocket->hSocket,
		SOL_SOCKET,
		SO_REUSEADDR,
		&dwOption,
		sizeof(dwOption));
	if (nResult == SOCKET_ERROR)
	{
		LogError("[Socket Wrapper] Ошибка установки параметра");
	}

	return TRUE;
}


/**
* Связать сокет с локальным адресом
* @param[in] sSocket	Экзмепляр
* @return				Результат работы.
*/
BOOL
BindSocket(
	SSocket	 sSocket
)
{
	INT nResult = bind(
		sSocket.hSocket,
		sSocket.pAddrInfo->ai_addr,
		sSocket.pAddrInfo->ai_addrlen);
	if (nResult == SOCKET_ERROR)
	{
		LogError("[Socket Wrapper] Ошибка связывания сокета с адресом : %d", WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

/**
* Начать прослушивать сокет
* @param[in] psSocket	Экзмепляр
* @param[in] dwPort		Порт
* @return				Результат работы.
*/
BOOL
ListenSocket(
	SSocket		socket,
	DWORD		dwMaxConnections
)
{
	INT nResult = listen(socket.hSocket, dwMaxConnections);
	if (nResult == SOCKET_ERROR)
	{
		LogError("[Socket Wrapper] Ошибка прослушивания сокета : %d", WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

/**
* Получить клиентское подключение
* @param[in]  sServerSocket	 Серверный сокет.
* @param[out] psClientSocket Клиентский сокет.
* @return					 Результат работы.
*/
BOOL
AcceptSocket(
	SSocket		sServerSocket,
	PSSocket	psClientSocket
)
{
	SOCKET		 hSocket = INVALID_SOCKET;
	PSOCKADDR_IN pAddressInternet = NULL;
	DWORD		 dwAddressSize = sizeof(SOCKADDR_IN);

	hSocket = accept(
		sServerSocket.hSocket,
		(LPSOCKADDR)pAddressInternet,
		&dwAddressSize);
	if (hSocket == INVALID_SOCKET)
	{
		LogError("[Socket Wrapper] Ошибка получения клиентского сокета : %d", WSAGetLastError());
		return FALSE;
	}

	psClientSocket->hSocket = hSocket;
	psClientSocket->pAddrInfo = pAddressInternet;

	return TRUE;
}

/**
* Создать новое подключение по адресу
* @param[in] sSocket	Сокет
* @return				Результат работы.
*/
BOOL
ConnectSocket(
	SSocket  sSocket
)
{
	INT nResult = connect(
		sSocket.hSocket,
		sSocket.pAddrInfo->ai_addr,
		sSocket.pAddrInfo->ai_addrlen);
	if (nResult == SOCKET_ERROR)
	{
		LogError("[Socket Wrapper] Ошибка подключения : %d", WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

/**
* Отправить данные по сокету
* @param[in] sSocket	Сокет
* @param[in] pBuffer	Буфер с данными
* @param[in] dwSize		Размер данных
* @return				Результат работы.
*/
BOOL
SocketSend(
	SSocket  sSocket,
	PBYTE	 pBuffer,
	DWORD	 dwSize
)
{
	SSocketMessageHeader sHeader = { 0 };
	sHeader.dwSize = dwSize;

	DWORD dwHeaderSize = sizeof(SSocketMessageHeader);
	BOOL bResult = SocketSendInternal(
		sSocket.hSocket,
		&sHeader,
		dwHeaderSize);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = SocketSendInternal(
		sSocket.hSocket,
		pBuffer,
		dwSize);
	if (!bResult)
	{
		return FALSE;
	}

	return TRUE;
}

/**
* Принять данные по сокету
* @param[in]	sSocket			Сокет
* @param[in]	pBuffer			Буфер с данными
* @param[inout] dwSize			Размер данных
* @param[in]	lTimeout		Таймаут
* @return						Результат работы.
* Если таймаут равен нулю, то вызов ждет бесконечно
* Если размер буфера равен нулю, то функция сама выделит
* буфер нужного размера
*/
BOOL
SocketReceive(
	SSocket sSocket,
	PBYTE	*pBuffer,
	PDWORD	pdwSize,
	LONG	lTimeout
)
{
	TIMEVAL	 sTimeVal;
	PTIMEVAL psTimeVal = NULL;
	BOOL	 bReallocated = FALSE;

	if (lTimeout != 0)
	{
		sTimeVal.tv_sec = lTimeout / 1000;
		sTimeVal.tv_usec = lTimeout % 1000;

		psTimeVal = &sTimeVal;
	}

	SSocketMessageHeader sHeader;
	BOOL bResult = SocketReceiveInternal(
		sSocket.hSocket,
		&sHeader,
		sizeof(SSocketMessageHeader), 
		psTimeVal);
	if (!bResult)
	{
		LogError("[Socket Wrapper] Ошибка принятия заголовка : %d", WSAGetLastError());
		return FALSE;
	}

	if (*pdwSize < sHeader.dwSize)
	{
		PBYTE pbAllocatedBuffer = NULL;
		if (*pdwSize == 0)
		{
			pbAllocatedBuffer = malloc(sHeader.dwSize);
			if (!pbAllocatedBuffer)
			{
				LogError("[Socket Wrapper] Ошибка выделения памяти");
				LogDebug("[Socket Wrapper] SocketRecevice -> malloc");
				return FALSE;
			}
		}
		else
		{
			pbAllocatedBuffer = realloc(*pBuffer, sHeader.dwSize);
			if (!pbAllocatedBuffer)
			{
				LogError("[Socket Wrapper] Ошибка реаллоцирования памяти");
				LogDebug("[Socket Wrapper] SocketRecevice -> realloc");
				return FALSE;
			}
		}
		
		*pBuffer = pbAllocatedBuffer;
		bReallocated = TRUE;
	}

	*pdwSize = sHeader.dwSize;

	bResult = SocketReceiveInternal(
		sSocket.hSocket,
		*pBuffer,
		*pdwSize,
		psTimeVal);
	if (!bResult)
	{
		if (bReallocated)
		{
			free(*pBuffer);
		}
		return FALSE;
	}

	return TRUE;
}

/**
* Отправить данные по сокету с отправкой хэш-суммы
* @param[in] sSocket	Сокет
* @param[in] pBuffer	Буфер с данными
* @param[in] dwSize		Размер данных
* @return				Результат работы.
*/
BOOL
SocketSendWithCheck(
	SSocket  sSocket,
	PBYTE	 pBuffer,
	DWORD	 dwSize
)
{
	DWORD dwHash = Crc32BufferCompute(pBuffer, dwSize);

	SSocketMessageHeader sHeader = { 0 };
	sHeader.dwSize = dwSize;
	sHeader.dwCrc32 = dwHash;

	INT nResult = send(
		sSocket.hSocket,
		(PBYTE)& sHeader,
		sizeof(SSocketMessageHeader), 0);
	if (nResult != sizeof(SSocketMessageHeader))
	{
		LogError("[Socket Wrapper] Ошибка отправки заголовка : %d", WSAGetLastError());
		return FALSE;
	}

	BOOL bResult = SocketSendInternal(
		sSocket.hSocket,
		pBuffer,
		dwSize);
	if (!bResult)
	{
		return FALSE;
	}

	return TRUE;
}

/**
* Принять данные по сокету с проверкой хэш суммы
* @param[in]	sSocket			Сокет
* @param[in]	pBuffer			Буфер с данными
* @param[inout] dwSize			Размер данных
* @param[in]	lTimeout		Таймаут
* @return						Результат работы.
* Если таймаут равен нулю, то вызов ждет бесконечно
* Если размер буфера равен нулю, то функция сама выделит
* буфер нужного размера
*/
BOOL
SocketReceiveWithCheck(
	SSocket sSocket,
	PBYTE	*pBuffer,
	PDWORD	pdwSize,
	LONG	lTimeout
)
{
	TIMEVAL	 sTimeVal;
	PTIMEVAL psTimeVal = NULL;
	BOOL	 bReallocated = FALSE;

	if (lTimeout != 0)
	{
		sTimeVal.tv_sec = lTimeout / 1000;
		sTimeVal.tv_usec = lTimeout % 1000;

		psTimeVal = &sTimeVal;
	}

	BOOL bResult = SocketReadSelect(
		sSocket.hSocket, psTimeVal);
	if (!bResult)
	{
		return FALSE;
	}

	SSocketMessageHeader sHeader;
	INT nResult = recv(
		sSocket.hSocket,
		&sHeader,
		sizeof(SSocketMessageHeader), 0);
	if (nResult != sizeof(SSocketMessageHeader))
	{
		LogError("[Socket Wrapper] Ошибка принятия заголовка : %d", WSAGetLastError());
		return FALSE;
	}

	if (*pdwSize < sHeader.dwSize)
	{
		PBYTE pbAllocatedBuffer = NULL;
		if (*pdwSize == 0)
		{
			pbAllocatedBuffer = malloc(sHeader.dwSize);
			if (!pbAllocatedBuffer)
			{
				LogError("[Socket Wrapper] Ошибка выделения памяти");
				LogDebug("[Socket Wrapper] SocketRecevice -> malloc");
				return FALSE;
			}
		}
		else
		{
			pbAllocatedBuffer = realloc(*pBuffer, sHeader.dwSize);
			if (!pbAllocatedBuffer)
			{
				LogError("[Socket Wrapper] Ошибка реаллоцирования памяти");
				LogDebug("[Socket Wrapper] SocketRecevice -> realloc");
				return FALSE;
			}
		}

		bReallocated = TRUE;
		*pBuffer = pbAllocatedBuffer;
	}

	*pdwSize = sHeader.dwSize;

	bResult = SocketReceiveInternal(
		sSocket.hSocket,
		*pBuffer,
		*pdwSize,
		psTimeVal);
	if (!bResult)
	{
		if (bReallocated)
		{
			free(*pBuffer);
		}
		return FALSE;
	}

	DWORD dwHash = Crc32BufferCompute(*pBuffer, *pdwSize);
	if (dwHash != sHeader.dwCrc32)
	{
		if (bReallocated)
		{
			free(*pBuffer);
		}
		return FALSE;
	}

	return TRUE;
}

/**
* Проверить закрыт ли сокет
* @param[in]	sSocket			Сокет
* @return						Закрыт ли сокет
*/
BOOL
SocketIsClosed(
	SSocket sSocket
)
{
	fd_set hSet;
	FD_ZERO(&hSet);
	FD_SET(sSocket.hSocket, &hSet);

	TIMEVAL sTimeout = { 0 };
	select(
		sSocket.hSocket + 1,
		&hSet, 0, 0, &sTimeout);
	if (!FD_ISSET(sSocket.hSocket, &hSet))
	{
		return FALSE;
	}
	
	int nBytesCanRead = 0;
	ioctlsocket(
		sSocket.hSocket,
		FIONREAD,
		&nBytesCanRead);
	return nBytesCanRead == 0;
}

/**
* Закрыть сокет
* @param[in] psSocket	Сокет
*/
VOID
CloseSocket(
	PSSocket psSocket
)
{
	if (psSocket->hSocket != INVALID_SOCKET)
	{
		closesocket(psSocket->hSocket);
		freeaddrinfo(psSocket->pAddrInfo);

		psSocket->hSocket = INVALID_SOCKET;
	}
}

/**
* Ожидание изменения состояния на чтение
* @param[in]	hSocket			Сокет
* @param[in]	pTimeout		Таймаут
* @return						Результат работы.
*/
BOOL
SocketReadSelect(
	SOCKET	 hSocket,
	PTIMEVAL pTimeout
)
{
	fd_set fdReadSet;

	FD_ZERO(&fdReadSet);
	FD_SET(hSocket, &fdReadSet);

	INT nResult = select(
		hSocket + 1,
		&fdReadSet,
		NULL, NULL, pTimeout);
	if (nResult == SOCKET_ERROR)
	{
		LogError("[Socket Wrapper] Ошибка ожидания изменения статуса сокета на чтение : %d", WSAGetLastError());
		return FALSE;
	}
	else if (nResult == 0)
	{
		WSASetLastError(TIMEOUT_EXPIRED);
		return FALSE;
	}

	if (!FD_ISSET(hSocket, &fdReadSet))
	{
		LogError("[Socket Wrapper] Кто-то попытался записать в сокет, хотя ожидалось чтение");
		return FALSE;
	}

	return TRUE;
}

/**
* Отправить данные по сокету
* @param[in] hSocket	Сокет
* @param[in] pBuffer	Буфер с данными
* @param[in] dwSize		Размер данных
* @return				Результат работы.
*/
BOOL
SocketSendInternal(
	SOCKET   hSocket,
	PBYTE	 pBuffer,
	DWORD	 dwSize
)
{
	CHAR szBuffer[SOCKET_RECEIVE_CHUNK];
	DWORD dwSentBytes = 0;

	DWORD dwMinSize = dwSize < SOCKET_RECEIVE_CHUNK ?
		dwSize : SOCKET_RECEIVE_CHUNK;

	while (TRUE)
	{
		DWORD dwBytesLeft = dwSize - dwSentBytes >= dwMinSize ?
			dwMinSize : dwSize - dwSentBytes;
		memcpy(szBuffer, pBuffer + dwSentBytes, dwBytesLeft);

		INT nResult = send(
			hSocket,
			pBuffer + dwSentBytes,
			dwBytesLeft, 0);
		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAENOBUFS)
			{
				Sleep(20);
			}
			else
			{
				LogError("[Socket Wrapper] Ошибка записи в сокет : %d", WSAGetLastError());
				return FALSE;
			}
		}
		else
		{
			dwSentBytes += nResult;
			if (dwSentBytes == dwSize)
			{
				break;
			}
		}
	}

	return TRUE;
}

/**
* Принять данные по сокету
* @param[in]	hSocket			Сокет
* @param[in]	pBuffer			Буфер с данными
* @param[inout] dwSize			Размер данных
* @param[in]	pTimeout		Таймаут
* @return						Результат работы.
*/
BOOL
SocketReceiveInternal(
	SOCKET   hSocket,
	PBYTE    pBuffer,
	DWORD	 dwSize,
	PTIMEVAL pTimeout
)
{
	CHAR	szBuffer[SOCKET_RECEIVE_CHUNK];
	DWORD	dwReadByte = 0;

	do
	{
		BOOL bResult = SocketReadSelect(
			hSocket, pTimeout);
		if (!bResult)
		{
			return FALSE;
		}

		DWORD dwDifference = dwSize - dwReadByte;
		DWORD dwNeedRead = (dwDifference) < SOCKET_RECEIVE_CHUNK ?
			dwDifference : SOCKET_RECEIVE_CHUNK;
		INT nResult = recv(
			hSocket, szBuffer,
			dwNeedRead, 0);
		if (nResult > 0)
		{
			if (dwSize < dwReadByte + nResult)
			{
				LogError("[Socket Wrapper] Ошибка чтения из сокета");
				return FALSE;
			}
			memcpy(pBuffer + dwReadByte, szBuffer, nResult);
			dwReadByte += nResult;

			if (dwReadByte == dwSize)
			{
				break;
			}
		}
		else
		{
			LogError("[Socket Wrapper] Ошибка чтения из сокета");
			return FALSE;
		}
	} while (TRUE);

	return TRUE;
}