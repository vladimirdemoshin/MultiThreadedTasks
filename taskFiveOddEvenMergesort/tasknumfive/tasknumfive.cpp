#include "stdafx.h"
#include<cmath>
#include<iostream>
#include<Windows.h>

using namespace std;

int n;
int *a;

DWORD WINAPI comparator(LPVOID arg)
{
	int* comparator_pair = (int*)arg;
	int first_index = comparator_pair[0];
	int second_index = comparator_pair[1];
	if (a[first_index] > a[second_index])
	{
		int tmp = a[first_index];
		a[first_index] = a[second_index];
		a[second_index] = tmp;
	}
	delete[] comparator_pair;
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	srand(GetTickCount());
	setlocale(LC_ALL, "RUS");
	cout << "Введите количество входов сортирующей сети:\n";
	cin >> n;
	a = new int[n];
	cout << "Исходная последовательность:\n";
	for (int i = 0; i < n; i++)
	{
		a[i] = rand() % 20;
		cout << a[i] << " ";
	}
	cout << "\n";

	HANDLE *hThreadArray = new HANDLE[n/2];
	int count_threads = 0;

	for (int count_phase = 1; count_phase < n; count_phase *= 2 )
	{
		for (int length = count_phase; length > 0; length /= 2)
		{
			for (int cascade = 0; cascade < n; cascade += 2 * count_phase)
			{
				for (int step = cascade + length % count_phase ; step < cascade + 2 * count_phase - length; )
				{
					if (step + length < n)
					{
						if (length != count_phase && ((step / length) % 2 == 0))
						{
							step = step + length;
						}
						else
						{
							int *comparator_pair = new int[2];
							comparator_pair[0] = step;
							comparator_pair[1] = step + length;
							hThreadArray[count_threads++] = CreateThread(0, 0, comparator, comparator_pair, 0, 0);
							step++;
						}
					}
					else break;
				}

				WaitForMultipleObjects(count_threads, hThreadArray, TRUE, INFINITE);
				for (int i = 0; i < count_threads; i++)
				{
					CloseHandle(hThreadArray[i]);
				}
				count_threads = 0;
			}
			
		}
	}

	cout << "Отсортированная последовательность:\n";
	for (int i = 0; i < n; i++)
	{
		cout << a[i] << " ";
	}

	delete[] a;
	delete[] hThreadArray;
	return 0;
}

