#include "stdafx.h"
#include<cmath>
#include<iostream>
#include<Windows.h>
#include<algorithm>

using namespace std;

int n;
int p;
int count_blocks;
int size;
int *a;

DWORD WINAPI sort(LPVOID arg)
{
	int index = (int)arg;
	int first = index * size;
	int last = (index + 1) * size;
	sort(a + first, a + last);
	first = last;
	last = (index + 2)*size;
	if (last > n)
		last = n;
	sort(a + first, a + last);
	return 0;
}

DWORD WINAPI MERGER_SORTER(LPVOID arg)
{
	int *comparator_pair = (int*)arg;
	int first_block = comparator_pair[0];
	int second_block = comparator_pair[1];
	int *arr = new int[2 * size];

	int left_first = first_block * size;
	int left_second = second_block * size;
	int right_first = (first_block + 1)*size;
	int right_second = (second_block + 1)*size;

	if (right_second >= n) right_second = n;

	int i = 0;
	while (left_first < right_first && left_second < right_second)
	{
		if (a[left_first] < a[left_second])
		{
			arr[i] = a[left_first];
			left_first++;
		}
		else
		{
			arr[i] = a[left_second];
			left_second++;
		}
		i++;
	}
	
	while (left_first < right_first)
	{
		arr[i] = a[left_first];
		i++;
		left_first++;
	}

	while (left_second < right_second)
	{
		arr[i] = a[left_second];
		i++;
		left_second++;
	}

	i = 0;
	for (int j = first_block * size; j < right_first; j++)
	{
		a[j] = arr[i];
		i++;
	}
	for (int j = second_block * size; j < right_second; j++)
	{
		a[j] = arr[i];
		i++;
	}

	delete[] arr;
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
	cout << "\nВведите количество процессоров:\n";
	cin >> p;

	count_blocks = 2 * p;
	size = n / count_blocks;
	if (n % count_blocks != 0)
	{
		size++;
		count_blocks = n / size;
		if (n % size != 0)
			count_blocks++;
	}
	
	HANDLE *hThreadArray = new HANDLE[p];
	int i = 0;
	for ( ; i < count_blocks; i+=2)
		hThreadArray[i / 2] = CreateThread(0,0,sort,(LPVOID)i,0,0);
	WaitForMultipleObjects(i / 2, hThreadArray, TRUE, INFINITE);
	for (int j = 0; j < i / 2; j++)
		CloseHandle(hThreadArray[j]);
	delete[] hThreadArray;

	HANDLE *hThreadArr = new HANDLE[count_blocks / 2];
	int count_threads = 0;

	for (int count_phase = 1; count_phase < count_blocks; count_phase *= 2)
	{
		for (int length = count_phase; length > 0; length /= 2)
		{
			for (int cascade = 0; cascade < count_blocks; cascade += 2 * count_phase)
			{
				for (int step = cascade + length % count_phase; step < cascade + 2 * count_phase - length;)
				{
					if (step + length < count_blocks)
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
							hThreadArr[count_threads++] = CreateThread(0, 0, MERGER_SORTER, comparator_pair, 0, 0);
							step++;
						}
					}
					else break;
				}

				WaitForMultipleObjects(count_threads, hThreadArr, TRUE, INFINITE);
				for (int i = 0; i < count_threads; i++)
				{
					CloseHandle(hThreadArr[i]);
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

	delete[] hThreadArr;
	delete[] a;

	return 0;
}
