// taskFourMultiplicationMatrixVector.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include<cmath>
#include<iostream>
#include<Windows.h>
#include <utility>

using namespace std;

const int BUF_SIZE = 255;
const int MAX_VALUE = 3;
int m, n, q;
int **A;
int *B;
int *C;
HANDLE *ghMutex;

void fillRandom(int *arr, int size)
{
	for (int i = 0; i<size; i++)
	{
		int sign = 1;
		if (rand() % 2 != 0) sign = -1;
		arr[i] = (rand() % (MAX_VALUE + 1))*sign;
	}
}

DWORD WINAPI ThreadFunction(LPVOID arg)
{
	pair<int, int> *position = (pair<int, int>*)arg;
	int indexI = position->first;
	int indexJ = position->second;

	int *temp = new int[m / q];
	for (int i = 0; i<m / q; i++)
		temp[i] = 0;

	char *text = new char[BUF_SIZE];
	int size = sprintf_s(text, BUF_SIZE, "Блок (%d ; %d): ", indexI / (m / q), indexJ / (n / q));

	for (int i = indexI; i<indexI + m / q; i++)
	{
		for (int j = indexJ; j<indexJ + n / q; j++)
			temp[i - indexI] += A[i][j] * B[j];

		size += sprintf_s(text + size, BUF_SIZE - size, " %d ", temp[i - indexI]);
	}
	printf("%s \n", text);

	WaitForSingleObject(ghMutex[indexI / (m / q)], INFINITE);

	for (int i = indexI; i<indexI + m / q; i++)
		C[i] += temp[i - indexI];

	ReleaseMutex(ghMutex[indexI / (m / q)]);

	delete position;
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(GetTickCount());
	setlocale(LC_ALL, "RUS");
	cout << "Введите m и нажмите ENTER\n";
	cin >> m;
	cout << "Введите n и нажмите ENTER\n";
	cin >> n;
	cout << "Введите q и нажмите ENTER\n";
	cin >> q;

	A = new int*[m];
	for (int i = 0; i<m; i++)
	{
		A[i] = new int[n];
		fillRandom(A[i], n);
	}

	B = new int[n];
	fillRandom(B, n);

	C = new int[m];
	for (int i = 0; i<m; i++)
		C[i] = 0;

	cout << "Матрица A:\n";
	for (int i = 0; i<m; i++)
	{
		for (int j = 0; j<n; j++)
			cout << A[i][j] << " ";
		cout << "\n";
	}

	cout << "Вектор B:\n";
	for (int i = 0; i<n; i++)
		cout << B[i] << " ";
	cout << "\n";

	ghMutex = new HANDLE[q];
	for (int i = 0; i<q; i++)
		ghMutex[i] = CreateMutex(NULL, FALSE, NULL);

	cout << "Разбили матрицу A на q^2 = " << q*q << " блоков, каждый блок размерностью " << m / q << " на " << n / q << ".\n";

	HANDLE* hThreadArray = new HANDLE[q*q];
	int k = 0;
	for (int i = 0; i<m; i += m / q)
	for (int j = 0; j<n; j += n / q)
	{
		pair<int, int> *position = new pair<int, int>(i, j);
		hThreadArray[k++] = CreateThread(0, 0, ThreadFunction, position, 0, 0);
	}

	WaitForMultipleObjects(q*q, hThreadArray, TRUE, INFINITE);

	for (int i = 0; i<q*q; i++)
		CloseHandle(hThreadArray[i]);

	for (int i = 0; i<q; i++)
		CloseHandle(ghMutex[i]);

	cout << "\nРезультат произведения - вектор С:\n";
	for (int i = 0; i<m; i++)
		cout << C[i] << " ";
	cout << "\n";

	for (int i = 0; i<m; i++)
	{
		delete[] A[i];
	}
	delete[] A;
	delete[] B;
	delete[] C;
	delete[] hThreadArray;
	delete[] ghMutex;

	return 0;
}




