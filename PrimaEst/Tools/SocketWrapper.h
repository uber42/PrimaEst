﻿/**
 * @file SocketWrapper.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef SOCKET_WRAPPER_H
#define SOCKET_WRAPPER_H

#define SOCKET_RECEIVE_CHUNK	0x1000 // 0x40000 ~ 0.25 mb

#define TIMEOUT_EXPIRED			0x00C0FFEE

#define IPV4_ADDRESS_MAX_LENGTH 16
#define PORT_MAX_SYMBOL_LENGTH	6


typedef struct _SSocket
{
	/** Дескриптор сокета */
	SOCKET		hSocket;

	/** Информация об адресе */
	PADDRINFOA	pAddrInfo;
} SSocket, *PSSocket;

/**
* Инициализировать механизм WSA
* @return	Результат работы.
*/
BOOL
InitializeWSA();

/**
* Деинициализировать механизм WSA
*/
VOID
DeinitializeWSA();

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
);

/**
* Связать сокет с локальным адресом
* @param[in] sSocket	Экзмепляр
* @return				Результат работы.
*/
BOOL
BindSocket(
	SSocket	 sSocket
);

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
);

/**
* Получить клиентское подключение
* @param[in]  sServerSocket	Серверный сокет.
* @param[out] psClientSocket	Клиентский сокет.
* @return					Результат работы.
*/
BOOL
AcceptSocket(
	SSocket		sServerSocket,
	PSSocket	psClientSocket
);

/**
* Создать новое подключение по адресу
* @param[in] sSocket	Сокет
* @return				Результат работы.
*/
BOOL
ConnectSocket(
	SSocket  sSocket
);

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
);

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
);

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
);

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
);

/**
* Проверить закрыт ли сокет
* @param[in]	sSocket			Сокет
* @return						Закрыт ли сокет
*/
BOOL
SocketIsClosed(
	SSocket sSocket
);


/**
* Закрыть сокет
* @param[in] psSocket	Сокет
*/
VOID
CloseSocket(
	PSSocket psSocket
);



#endif