/*
	Пустая строка в грамматике обозначается строкой "EMPTY_STR" и считается терминалом.
	Стартовый символ S' обязателен!
 */

#pragma once
#include <string>
#include <stack>
#include <set>
#include "Item.h"
#include "Token.h"

bool CompareVectors(vector<Point> v1, vector<Point> v2);

#define MAP map<string, GSInfo>

enum TYPE_ACTION { MOVE, TURN, ERROR, ACCESS, CONFLICT };
const int NOT_VALUE = -1;

struct TypeAction // Действие в таблице действий и переходов
{
	TypeAction() : type(TYPE_ACTION::ERROR), value(NOT_VALUE) { }
	TypeAction(TYPE_ACTION _type, int _value = NOT_VALUE) : type(_type), value(_value) { }
	TYPE_ACTION type; // Тип действия
	int value; // Состояние в которое надо перейти или номер продукции, по которой надо свернуть (индекс в G' != 0)
};

//struct Info
//{
//	Info() { }
//	Info(string _s, int _q = -1) : s(_s), q(_q) { }
//	bool operator==(Info& a)
//	{
//		return s == a.s;
//	}
//	string s; // символ грамматики
//	int q; // очередность
//};

//class Compare
//{
//public:
//	bool operator()(Info a, Info b)
//	{
//		return a.q < b.q;
//	}
//};

class GeneratorSLRTable
{
protected:
	vector<Production> G; // Расширенная граматика G'

	struct GSInfo
	{
		GSInfo() { }
		GSInfo(bool isTerminal) : terminal(isTerminal) { }
		bool terminal;
		vector<int> indexes;
	};

	/*map<Info, GSInfo, Compare> _GS;*/

	MAP GS;

public:
	GeneratorSLRTable(vector<Production>, vector<char*>, int); // Инициализировать расширенной грамматикой G'
	GeneratorSLRTable(const char*); // Инициализировать путем к файлу с расширенной грамматикой G' и дополнительными данными (см. пред. конструктор)
	void PrintG(); // Вывести расширенную грамматику G'
	void PrintC(ostream&, const vector<vector<Point>>&); // Вывести канонический набор множеств LR(0)-пунктов в поток
	void PrintSLRTable(ostream&, map<string, TypeAction>*&, int); // Вывести SLR-таблицу в поток
	void PrintConflictsTable(ostream&, vector<vector<TypeAction>>&); // Вывести таблицу конфликтов
	vector<vector<Point>> CreateC(); // Сформировать канонический набор множеств LR(0)-пунктов
	bool CreateSLRTable(map<string, TypeAction>*&, int&, vector<vector<TypeAction>>&); // Сгенерировать SLR-таблицу
	bool StartSLRAnalysis(vector<Token>, map<string, TypeAction>*&, int); // Синтаксический SRL-анализ (на входе поток токенов, таблица SRL анализа и ее размер (кол-во состояний))

	vector<Point> CLOSURE(vector<Point>);
	vector<Point> GOTO(vector<Point>, const char*);
	vector<char*> FIRST(char*); // ЭТА ФУНКЦИЯ НЕ ПРОТЕСТИРОВАНА НА ГРАММАТИКУ G' С EMPTY_STR
	bool GeneratorSLRTable::IsEmptyStr(char*); // Функция выясняет, можно ли породить из нетерминала (параметр функции) пустую строку (EMPTY_STR) (Исп. в FIRST)
	vector<char*> FOLLOW(char*);
};