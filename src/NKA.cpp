#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include "NKA.h"

bool IsDigit(char c) { return (c >= 48 && c <= 57); }

NKA::NKA(map<char, vector<int>>* tranTable, int n, bool* finalStates, bool stateForDigits) : N(n), st(stateForDigits)
{
	if(N <= 0 || !tranTable)
		throw exception("Конструктору NKA были переданы неверные параметры");
	for(int i = 0; i < N; i++)
		if(tranTable[i].size() == 0)
			throw exception("Конструктору NKA были переданы неверные параметры");

	// Копируем таблицу переходов
	move = new map<char, vector<int>>[N];
	for(int i = 0; i < N; i++)
		move[i] = tranTable[i];

	// Копируем принимающие состо¤ни¤
	fStates = new bool[N];
	for(int i = 0; i < N; i++)
		fStates[i] = finalStates[i];
}

NKA::~NKA()
{
	delete[] move;
}

bool NKA::Simulation(char* str)
{
	if(!str)
		throw exception("Методу Simulation были переданы неверные параметры");

	set<int> S = EmptyClosure(0);
	//if(S.empty()) // Здесь S не может быть пустым. т.к. состояние 0 в нем будет в любом случае
	//	cout << "!" << endl;
	for(set<int>::iterator it = S.begin(); it != S.end(); it++)
		cout << *it << " ";
	cout << endl;
	char c = *str++;
	while(c)
	{
		if(c != ' ')
		{
			S = EmptyClosure(Move(S, c));
			if(S.empty())
				cout << "!" << endl;
			for(set<int>::iterator it = S.begin(); it != S.end(); it++)
				cout << *it << " ";
			cout << endl;
		}
		c = *str++;
	}
	for(set<int>::iterator it = S.begin(); it != S.end(); it++)
		if(fStates[*it])
			return true;

	return false;
}

set<int> NKA::EmptyClosure(int s)
{
	if(s < 0 || s >= N)
		throw exception("Методу EmptyClosure были переданы неверные параметры");

	set<int> res;
	stack<int> st;
	int p;
	st.push(s);
	while(!st.empty())
	{
		p = st.top();
		st.pop();
		res.insert(p);
		for(int i = 0; i < move[p]['@'].size(); i++)
			if(move[p]['@'][i] != EMPTY_SET)
				st.push(move[p]['@'][i]);
	}

	return res;
}

set<int> NKA::EmptyClosure(set<int> states)
{
	set<int> res;
	set<int> tmp;
	for(set<int>::iterator it = states.begin(); it != states.end(); it++)
	{
		tmp = EmptyClosure(*it);
		for(set<int>::iterator it2 = tmp.begin(); it2 != tmp.end(); it2++)
			res.insert(*it2);
	}

	return res;
}

set<int> NKA::Move(set<int> states, char c)
{
	if(st)
		if(IsDigit(c))
			c = '~';
	set<int> res;
	for(set<int>::iterator it = states.begin(); it != states.end(); it++)
		for(int i = 0; i < move[*it][c].size(); i++)
			if(move[*it][c][i] != EMPTY_SET)
				res.insert(move[*it][c][i]);

	return res;
}