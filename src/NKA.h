#pragma once
/* Класс представляет собой недетеминированный конечный автомат. Конструктору передаются:
   1. Таблица переходов НКА - map<char, vector<int>>*
	  Обозначения:
	  @ - пустой символ
	  ~ - цифры от 0 до 9
	  NKA::EMPTY_SET - пустое множество
   2. Количество состояний НКА - int
   3. Булев массив, проиндексированный состояниями НКА для указания принимающих состояний - bool*
   4. Булево значение, указывающее, считать ли автомату все цифры как символ '~' или они нет, т.к. они отдельно указаны в таблице переходов
   
   Методы:
   bool Simulation(char* str) - Сканирует строку str. Если НКА принимает строку, 
       то возвращается true, иначе - false.
 */

#include <vector>
#include <set>
#include <map>
using namespace std;

bool IsDigit(char); // Функция-предикат, возвращающая true, если ее аргумент символ c - цифра от 0 до 9

class NKA
{
protected:
	map<char, vector<int>>* move; // Таблица переходов НКА
	const int N; // Кол-во состояний НКА
	bool* fStates; // Принимающие состояния
	bool st; // см. п.4 в описании класса
	
	set<int> EmptyClosure(int);
	set<int> EmptyClosure(set<int>);
	set<int> Move(set<int>, char);

public:
	static const int EMPTY_SET = -100; // Пустое множество
	NKA(map<char, vector<int>>*, int, bool*, bool);
	virtual ~NKA();
	virtual bool Simulation(char*);
};