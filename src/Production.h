/*
	Класс представляет продукцию.
 */

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

class Production
{
protected:
	char* z; // Заголовок
	vector<char*> t; // Тело

public:
	Production(); // Инициализация продукцией NULL -> NULL
	Production(char* zg, int count, char* s, ...);
	Production(const Production& pr);
	friend ostream& operator<<(ostream& out, const Production& p);
	bool operator==(const Production& p);

	friend class GeneratorSLRTable;
};