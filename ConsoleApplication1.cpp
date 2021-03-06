﻿#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <windows.h>
#include <map>
#include <string>
#include <algorithm>
#include <ctime>
#include "Allocator.h"

struct charptr_cmp 
{
	bool operator()(char* a, char* b) const
	{
		while (isalpha(*a) && isalpha(*b))
		{
			if (tolower(*a) != tolower(*b))
				return tolower(*a) < tolower(*b);
			a++, b++;
		}
		return isalpha(*a) < isalpha(*b);
	}
};

bool decrease_cmp(std::pair<int, char*> n, std::pair<int, char*> m)
{
	if (n.first != m.first)
		return n.first > m.first;
	return charptr_cmp()(n.second, m.second);
}


bool try_read(size_t Size, HANDLE hFile, char* Text)
{
	DWORD buf;
	if (!ReadFile(hFile, Text, Size, &buf, NULL))
		return 0;
	if (buf > 0)
		Text[buf] = '\0';
	return 1;
}

size_t get_size(HANDLE hFile)
{
	LARGE_INTEGER fileSize;
	GetFileSizeEx(hFile, &fileSize);
	std::cout << "Size of file: " << fileSize.QuadPart << '\n';
	return fileSize.QuadPart;
}

template<class Comparator = std::less<char*>,
		 class Allocator = std::allocator<std::pair<const char*, size_t>>>
std::map<char*, size_t, Comparator, Allocator> parse_text(char* Text, size_t Size)
{
	std::map<char*, size_t, Comparator, Allocator> q;
	bool isword = false;
	for (size_t i = 0; i < Size && Text[i] != '\0'; ++i)
	{
		if (isalpha(Text[i]))
		{
			if (!isword)
			{
				q[Text + i]++;
				isword = true;
			}
		}
		else
			isword = false;
	}
	return q;
}


std::map<std::string, size_t> parse_text_using_string(char* Text, size_t Size)
{
	std::map<std::string, size_t> q;
	std::string current;
	for (size_t i = 0; i < Size && Text[i] != '\0'; ++i)
	{
		if (isalpha(Text[i]))
			current += tolower(Text[i]);
		else if (current != "")
			q[current]++, current = "";
	}
	if (current != "")
		q[current]++;
	return q;
}

template<class Comparator, class Allocator>
void print_result(std::map<char*, size_t, Comparator, Allocator> q)
{
	std::pair<int, char*>* result = new std::pair<int, char*>[q.size()];
	int j = 0;
	for (auto i = q.begin(); i != q.end(); ++i, ++j) 
		result[j] = { i->second, i->first };
	std::sort(result, result + q.size(), decrease_cmp);
	for (j = 0; j < q.size(); ++j)
	{
		auto i = result[j];
		auto ptr = i.second;
		while (isalpha(*ptr)) 
		{
			std::cout << (char)tolower(*ptr);
			ptr++;
		}
		std::cout << " : " << i.first << '\n';
	}
	delete[] result;
}


template<class Str, class Comparator, class Allocator>
double launch(std::map<Str, size_t, Comparator, Allocator>(*func)(char*, size_t),
				char* Text, size_t Size, size_t l_cnt = 10)
{
	double sum_time = 0;
	func(Text, Size);
	for (int i = 0; i < l_cnt; ++i)
	{
		clock_t st = clock();
		func(Text, Size);
		clock_t nd = clock();
		sum_time += 1.0 * (nd - st) / CLOCKS_PER_SEC;
	}
	return sum_time / l_cnt;
}

void time_compare(char* Text, size_t Size)
{
	double my_alloc = launch(parse_text<charptr_cmp, MyAlloc<char*>>, Text, Size);
	std::cout << std::setprecision(5) << std::fixed <<  "My allocator: "
		<< my_alloc << " sec" <<  std::endl;
	double internal_alloc = launch(parse_text<>, Text, Size);
	std::cout << std::setprecision(5) << std::fixed << "Internal allocator: "
		<< internal_alloc << " sec" << std::endl;
	double str_working = launch(parse_text_using_string, Text, Size);
	std::cout << std::setprecision(5) << std::fixed << "String working: "
		<< str_working << " sec" << std::endl;
}


TCHAR* request_file()
{
	std::cout << "Write full path to the text file:\n";
	std::cin >> raw_path;
	TCHAR *path = new TCHAR[strlen(raw_path) + 1];
	mbstowcs(path, raw_path, strlen(raw_path) + 1);
	return path;
}


int main()
{
	HANDLE hFile = CreateFile(request_file(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Cannot open file\n";
		CloseHandle(hFile);
		ExitProcess(5);
	}
	size_t Size = get_size(hFile);
	char* Text = new char[Size + 1];
	if (!try_read(Size, hFile, Text))
	{
		std::cerr << "Cannot read file\n";
		CloseHandle(hFile);
		delete[] Text;
		ExitProcess(3);
	}
	std::cout << "Would you like compare time or print result? 1 - Compare, 2 - Print result\n";
	char c;
	std::cin >> c;
	if (c == '1')
		time_compare(Text, Size);
	else if (c == '2')
		print_result(parse_text<charptr_cmp, MyAlloc<char*>>(Text, Size));
	delete[] Text;
	CloseHandle(hFile);
	ExitProcess(0);
}