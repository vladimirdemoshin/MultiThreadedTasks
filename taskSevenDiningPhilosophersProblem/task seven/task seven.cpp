// task seven.cpp: ���������� ����� ����� ��� ����������� ����������.
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
		printf("\n ������� ��� ������� %d %s", index," ���������");
		Sleep(t * 1000);

		WaitForSingleObject(semaphore, INFINITE);

		WaitForSingleObject(hMutexArray[index], INFINITE);
		printf("\n ������� ��� ������� %d %s %d", index, " ���� ����� ������� ��� �������", index);

		int num_right = index == 0 ? n - 1 : index - 1;
		WaitForSingleObject(hMutexArray[num_right], INFINITE);
		printf("\n ������� ��� ������� %d %s %d", index, " ���� ������ ������� ��� �������", num_right);

		t = rand() % 3 + 1;
		printf("\n �������� ��� ������� %d %s", index, " �������");
		Sleep(t * 1000);

		printf("\n �������� ��� ������� %d %s", index, " �������� �������");
		ReleaseMutex(hMutexArray[index]);
		ReleaseMutex(hMutexArray[num_right]);
		ReleaseSemaphore(semaphore, 1, NULL);
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Rus");
	cout << "������� ���������� ���������\n";
	cin >> n;
	cout << "������� �����, ������� ��� ������ ����� �������\n";
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