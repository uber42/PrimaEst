/**
 * @file SocketWrapper.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

static WSADATA g_wsaData = { 0 };

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

	INT nResult = send(
		sSocket.hSocket,
		(PBYTE)&sHeader,
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
* Принять данные по сокету
* @param[in]	sSocket			Сокет
* @param[in]	pBuffer			Буфер с данными
* @param[inout] dwSize			Размер данных
* @param[in]	lTimeout		Таймаут
* @param[out]	pbReallocated	Реаллоцирована ли память
* @return						Результат работы.
* Если таймаут равен нулю, то вызов ждет бесконечно
*/
BOOL
SocketReceive(
	SSocket sSocket,
	PBYTE*  ppBuffer,
	PDWORD	pdwSize,
	LONG	lTimeout,
	PBOOL	pbReallocated
)
{
	TIMEVAL	 sTimeVal;
	PTIMEVAL psTimeVal = NULL;

	*pbReallocated = FALSE;

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
		*ppBuffer = malloc(sHeader.dwSize);
		if (!ppBuffer)
		{
			LogError("[Socket Wrapper] Ошибка выделения памяти");
			LogDebug("[Socket Wrapper] SocketRecevice -> malloc");
			return FALSE;
		}
		*pbReallocated = TRUE;
	}

	*pdwSize = sHeader.dwSize;

	bResult = SocketReceiveInternal(
		sSocket.hSocket,
		*ppBuffer,
		*pdwSize,
		psTimeVal);
	if (!bResult)
	{
		if (*pbReallocated)
		{
			free(*ppBuffer);
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
* @param[out]	pbReallocated	Реаллоцирована ли память
* @return						Результат работы.
* Если таймаут равен нулю, то вызов ждет бесконечно
*/
BOOL
SocketReceiveWithCheck(
	SSocket sSocket,
	PBYTE* ppBuffer,
	PDWORD	pdwSize,
	LONG	lTimeout,
	PBOOL	pbReallocated
)
{
	TIMEVAL	 sTimeVal;
	PTIMEVAL psTimeVal = NULL;

	*pbReallocated = FALSE;

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
		*ppBuffer = malloc(sHeader.dwSize);
		if (!ppBuffer)
		{
			LogError("[Socket Wrapper] Ошибка выделения памяти");
			LogDebug("[Socket Wrapper] SocketRecevice -> malloc");
			return FALSE;
		}
		*pbReallocated = TRUE;
	}

	*pdwSize = sHeader.dwSize;

	bResult = SocketReceiveInternal(
		sSocket.hSocket,
		*ppBuffer,
		*pdwSize,
		psTimeVal);
	if (!bResult)
	{
		if (*pbReallocated)
		{
			free(*ppBuffer);
		}
		return FALSE;
	}

	DWORD dwHash = Crc32BufferCompute(*ppBuffer, *pdwSize);
	if (dwHash != sHeader.dwCrc32)
	{
		if (*pbReallocated)
		{
			free(*ppBuffer);
		}
		return FALSE;
	}

	return TRUE;
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
		NULL,
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

		INT nResult = recv(
			hSocket, szBuffer,
			SOCKET_RECEIVE_CHUNK, 0);
		if (nResult > 0)
		{
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