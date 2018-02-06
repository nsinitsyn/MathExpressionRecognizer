/*
	Класс представляет LR(0)-пункт. Наследуется от класса Production.
 */

#pragma once
#include "Production.h"

const int MAX_POINT = -1;

class Point : public Production
{
protected:
	int index; // Индекс символа в векторе t, перед которым стоит точка. Если index == t.size(), значит точка в конце

public:
	Point(); // Продукция NULL -> NULL, пункт не определен
	Point(Production pr, int i);
	friend ostream& operator<<(ostream& out, const Point& p);
	bool operator==(const Point& p);
	bool operator!=(const Point& p);

	friend class GeneratorSLRTable;
};