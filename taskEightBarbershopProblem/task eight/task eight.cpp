// task eight.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <queue>

using namespace std;

int clientCount;
int haircutCount;
int barberCount;
int spotCount;

HANDLE *sHaircutStatus;
HANDLE sVacantSpot;
HANDLE sClientReadyToHaircut;
HANDLE sClientLeaveSpot;

HANDLE mTableClientRoom;
HANDLE mTableBarberRoom;

queue<int> tableClientRoom;
queue<int> tableBarberRoom;

DWORD WINAPI ClientThreadFunction(LPVOID arg);
DWORD WINAPI BarberThreadFunction(LPVOID arg);

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Rus");
	cout << "Введите количество клиентов\n";
	cin >> clientCount;
	cout << "Введите количество стрижек\n";
	cin >> haircutCount;
	cout << "Введите количество парикмахеров\n";
	cin >> barberCount;
	cout << "Введите количество мест на диване\n";
	cin >> spotCount;

	sHaircutStatus = new HANDLE[barberCount + spotCount];
	for (int i = 0; i < barberCount + spotCount; i++)
	{
		tableClientRoom.push(i);
		sHaircutStatus[i] = CreateSemaphore(0, 0, 1, 0);
	}

	sVacantSpot = CreateSemaphore(0,barberCount,barberCount,0);
	sClientReadyToHaircut = CreateSemaphore(0, 0, barberCount, 0);
	sClientLeaveSpot = CreateSemaphore(0, 0, barberCount, 0);

	mTableClientRoom = CreateMutex(0, 0, 0);
	mTableBarberRoom = CreateMutex(0, 0, 0);

	HANDLE *clients = new HANDLE[clientCount];
	HANDLE *barbers = new HANDLE[barberCount];

	for (int i = 0; i < clientCount; i++)
	{
		clients[i] = CreateThread(0,0,ClientThreadFunction,(LPVOID)i,0,0);
	}

	for (int i = 0; i < barberCount; i++)
	{
		barbers[i] = CreateThread(0, 0, BarberThreadFunction, (LPVOID)i, 0, 0);
	}

	WaitForMultipleObjects(clientCount,clients,TRUE,INFINITE);

	for (int i = 0; i < clientCount; i++)
	{
		CloseHandle(clients[i]);
	}

	CloseHandle(sVacantSpot);
	CloseHandle(sClientReadyToHaircut);
	CloseHandle(sClientLeaveSpot);

	CloseHandle(mTableClientRoom);
	CloseHandle(mTableBarberRoom);

	delete[] sHaircutStatus;

	delete[] clients;
	delete[] barbers;
	
	return 0;
}

DWORD WINAPI ClientThreadFunction(LPVOID arg)
{
	int clientIndex = (int)arg;
	int currentHaircutCount = 0;
	while (currentHaircutCount < haircutCount)
	{
		WaitForSingleObject(mTableClientRoom,INFINITE);

		if (tableClientRoom.size() == 0)
		{
			ReleaseMutex(mTableClientRoom);
			printf("Свободных мест нет. Клиент %d уходит из парикмахерской\n",clientIndex);
		}
		else
		{
			int number = tableClientRoom.front();
			tableClientRoom.pop();
			ReleaseMutex(mTableClientRoom);
			printf("Клиент %d зашёл в парикмахерскую и взял номерок %d\n",clientIndex,number);
			WaitForSingleObject(sVacantSpot,INFINITE);
			WaitForSingleObject(mTableBarberRoom, INFINITE);
			tableBarberRoom.push(number);
			ReleaseMutex(mTableBarberRoom);
			printf("Клиент %d садится в кресло парикмахера\n",clientIndex);
			ReleaseSemaphore(sClientReadyToHaircut,1,NULL);
			WaitForSingleObject(sHaircutStatus[number],INFINITE);
			printf("Клиент %d подстригся и уходит из парикмахерско\n", clientIndex);
			ReleaseSemaphore(sClientLeaveSpot,1,NULL);
			currentHaircutCount++;
		}

		int t = 1000 * (rand() % 5 + 1);
		Sleep(t);
	}

	return 0;
}

DWORD WINAPI BarberThreadFunction(LPVOID arg)
{
	int barberIndex = (int)arg;
	while (true)
	{
		WaitForSingleObject(sClientReadyToHaircut,INFINITE);
		WaitForSingleObject(mTableBarberRoom, INFINITE);
		int number = tableBarberRoom.front();
		tableBarberRoom.pop();
		ReleaseMutex(mTableBarberRoom);
		printf("Парикмахер %d стрижёт клиента с номерком %d\n",barberIndex,number);
		int t = 1000 * (rand() % 3 + 1);
		Sleep(t);
		ReleaseSemaphore(sHaircutStatus[number],1,NULL);
		WaitForSingleObject(sClientLeaveSpot,INFINITE);
		WaitForSingleObject(mTableClientRoom,INFINITE);
		tableClientRoom.push(number);
		ReleaseMutex(mTableClientRoom);
		printf("Парикмахер %d готов принять следующего клиента\n",barberIndex);
		ReleaseSemaphore(sVacantSpot,1,NULL);
	}
	return 0;
}