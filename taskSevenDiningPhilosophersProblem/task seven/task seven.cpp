// task seven.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>

using namespace std;

int n;
int m;

HANDLE *hThreadArray;
HANDLE *hMutexArray;
HANDLE semaphore;

DWORD WINAPI ThreadFunction(LPVOID arg)
{
	int index = (int)arg;

	srand(index);

	int t;

	for (int i = 0; i < m; i++)
	{
		t = rand() % 5 + 1;
		printf("\n Философ под номером %d %s", index," задумался");
		Sleep(t * 1000);

		WaitForSingleObject(semaphore, INFINITE);

		WaitForSingleObject(hMutexArray[index], INFINITE);
		printf("\n Философ под номером %d %s %d", index, " взял левую палочку под номером", index);

		int num_right = index == 0 ? n - 1 : index - 1;
		WaitForSingleObject(hMutexArray[num_right], INFINITE);
		printf("\n Философ под номером %d %s %d", index, " взял правую палочку под номером", num_right);

		t = rand() % 3 + 1;
		printf("\n Филосооф под номером %d %s", index, " обедает");
		Sleep(t * 1000);

		printf("\n Филосооф под номером %d %s", index, " закончил обедать");
		ReleaseMutex(hMutexArray[index]);
		ReleaseMutex(hMutexArray[num_right]);
		ReleaseSemaphore(semaphore, 1, NULL);
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Rus");
	cout << "Введите количество философов\n";
	cin >> n;
	cout << "Введите число, сколько раз каждый будет обедать\n";
	cin >> m;

	hMutexArray = new HANDLE[n];
	for (int i = 0; i < n; i++)
		hMutexArray[i] = CreateMutex(NULL, false, NULL);

	semaphore = CreateSemaphore(NULL, n - 1, n - 1, NULL);
	
	hThreadArray = new HANDLE[n];
	for (int i = 0; i < n; i++)
		hThreadArray[i] = CreateThread(NULL, 0, ThreadFunction, (LPVOID)i, 0, 0);
	
	WaitForMultipleObjects(n, hThreadArray, TRUE, INFINITE);

	for (int i = 0; i < n; i++)
	{
		CloseHandle(hMutexArray[i]);
		CloseHandle(hThreadArray[i]);
	}
	
	CloseHandle(semaphore);
	
	delete[] hThreadArray;
	delete[] hMutexArray;

	return 0;
}