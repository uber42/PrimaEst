#ifndef TIMER_H
#define TIMER_H

typedef PVOID CHANNEL_TIMER;

/**
* Создать таймер
* @param[out]   hTimer			Экземпляр таймера.
* @param[in]	hMessageChannel	Канал для подачи сигнала истечения таймера.
* @param[in]	dwSignalId		ИД для передачи в канал.
* @return Результат работы функции.
*/
BOOL
CreateChannelTimer(
	CHANNEL_TIMER*	hTimer,
	MESSAGE_CHANNEL	hMessageChannel,
	DWORD			dwSignalId
);

/**
* Установить таймер
* @param[out]   hTimer			Экземпляр таймера.
* @param[in]	dwTimeout		Время срабатывания.
* @return Результат работы функции.
*/
BOOL
SetChannelTimer(
	CHANNEL_TIMER	hTimer,
	DWORD			dwTimeout
);

/**
* Сбросить таймер
* @param[out]   hTimer			Экземпляр таймера.
*/
VOID
ResetChannelTimer(
	CHANNEL_TIMER	hTimer
);

/**
* Удалить таймер
* @param[out]   hTimer			Экземпляр таймера.
*/
VOID
DeleteChannelTimer(
	CHANNEL_TIMER	hTimer
);


#endif