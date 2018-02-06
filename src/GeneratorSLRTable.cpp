#include "GeneratorSLRTable.h"
#include <functional>   // std::greater

bool CompareVectors(vector<Point> v1, vector<Point> v2)
{
	if(v1.size() == v2.size())
	{
		for(int i = 0; i < v1.size(); i++)
			if(v1[i] != v2[i])
				return false;
		return true;
	}

	return false;
}

GeneratorSLRTable::GeneratorSLRTable(vector<Production> Gr, vector<char*> gs, int iFirstTerminal) : G(Gr)//, GS(gs.size())
{
	/*Info tmp;*/
	for(int i = 0; i < gs.size(); i++)
	{
		//tmp = Info(gs[i], i);
		//_GS[tmp] = GSInfo(i >= iFirstTerminal);
		//if(i < iFirstTerminal) // Если gs[i] - нетерминал
		//	for(int j = 0; j < G.size(); j++)
		//		if(!strcmp(gs[i], G[j].z))
		//			_GS[Info(gs[i])].indexes.push_back(j);

		GS[gs[i]] = GSInfo(i >= iFirstTerminal);
		if(i < iFirstTerminal) // Если gs[i] - нетерминал
			for(int j = 0; j < G.size(); j++)
				if(!strcmp(gs[i], G[j].z))
					GS[gs[i]].indexes.push_back(j);
	}
	//for(MAP::iterator it = GS.begin(); it != GS.end(); it++)
	//	cout << (*it).first << endl;

	/*for(map<Info, GSInfo, Compare>::iterator it = _GS.begin(); it != _GS.end(); it++)
		cout << (*it).first.s << endl;*/
}

GeneratorSLRTable::GeneratorSLRTable(const char* fileGPath)
{
}

void GeneratorSLRTable::PrintG()
{
	for(int i = 0; i < G.size(); i++)
		cout << G[i] << endl;
}

void GeneratorSLRTable::PrintC(ostream& out, const vector<vector<Point>>& C)
{
	for(int i = 0; i < C.size(); i++)
	{
		out << "I-" << i << ":" << endl;
		for(int j = 0; j < C[i].size(); j++)
			out << C[i][j] << endl;
		out << endl;
	}
}

void GeneratorSLRTable::PrintSLRTable(ostream& out, map<string, TypeAction>*& SLR_table, int sizeTable)
{
	out << "\t";
	for(MAP::iterator it = GS.begin(); it != GS.end(); it++)
			if((*it).first != "S'")
				if((*it).first == "EMPTY_STR")
					out << "E_STR\t";
				else
					out << (*it).first << "\t";
	out << "$";
	out << endl;
	for(int i = 0; i < sizeTable; i++)
	{
		out << i << "\t";
		for(MAP::iterator it = GS.begin(); it != GS.end(); it++)
		{
			if((*it).first != "S'")
			{
				if(SLR_table[i][(*it).first].type == TYPE_ACTION::MOVE)
				{
					if(GS[(*it).first].terminal)
						out << "s" << SLR_table[i][(*it).first].value << "\t";
					else
						out << SLR_table[i][(*it).first].value << "\t";
				}
				else if(SLR_table[i][(*it).first].type == TYPE_ACTION::TURN)
					out << "r" << SLR_table[i][(*it).first].value << "\t";
				else if(SLR_table[i][(*it).first].type == TYPE_ACTION::ACCESS)
					out << "acc\t";
				else if(SLR_table[i][(*it).first].type == TYPE_ACTION::ERROR)
					out << "e\t";
				else if(SLR_table[i][(*it).first].type == TYPE_ACTION::CONFLICT)
					out << "!" << SLR_table[i][(*it).first].value << "\t";
			}
		}
		if(SLR_table[i]["$"].type == TYPE_ACTION::MOVE)
			out << "s" << SLR_table[i]["$"].value << "\t";
		else if(SLR_table[i]["$"].type == TYPE_ACTION::TURN)
			out << "r" << SLR_table[i]["$"].value << "\t";
		else if(SLR_table[i]["$"].type == TYPE_ACTION::ACCESS)
			out << "acc\t";
		else if(SLR_table[i]["$"].type == TYPE_ACTION::ERROR)
			out << "e\t";
		else if(SLR_table[i]["$"].type == TYPE_ACTION::CONFLICT)
				out << "!" << SLR_table[i]["$"].value << "\t";
		out << endl;
	}
}

void GeneratorSLRTable::PrintConflictsTable(ostream& out, vector<vector<TypeAction>>& tableConflicts)
{
	for(int i = 0; i < tableConflicts.size(); i++)
	{
		out << "Конфликт " << i << ":" << endl;
		for(int j = 0; j < tableConflicts[i].size(); j++)
		{
			if(tableConflicts[i][j].type == TYPE_ACTION::MOVE)
				out << "s" << tableConflicts[i][j].value << endl;
			else if(tableConflicts[i][j].type == TYPE_ACTION::TURN)
				out << "r" << tableConflicts[i][j].value << endl;
			else if(tableConflicts[i][j].type == TYPE_ACTION::ACCESS)
				out << "acc" << endl;
		}
	}
}

vector<vector<Point>> GeneratorSLRTable::CreateC()
{
	vector<vector<Point>> C; // Канонический набор множеств пунктов

	vector<Point> tmp;
	tmp.push_back(Point(G[0], 0));

	C.push_back(CLOSURE(tmp));
	int size_C = C.size();
	bool L;
	for(int i = 0; i < size_C; i++) // Каждое мн-во пунктов из C
	{
		for(MAP::iterator it = GS.begin(); it != GS.end(); it++) // Каждый грамматический символ грамматики G'
		{
			tmp = GOTO(C[i], (*it).first.c_str());
			if(!tmp.empty())
			{
				L = false;
				for(int k = 0; k < size_C; k++) // Ищем tmp в C
				{
					if(CompareVectors(tmp, C[k])) // tmp == C[k]
					{
						L = true;
						break;
					}
				}
				if(!L)
				{
					C.push_back(tmp);
					size_C++;
				}
			}
		}
	}

	return C;
}

bool GeneratorSLRTable::CreateSLRTable(map<string, TypeAction>*& SLR_table, int& sizeTable, vector<vector<TypeAction>>& tableConflicts)
{
	bool access = true;
	vector<vector<Point>> C = CreateC(); // Вычисляем канонический набор множеств LR(0)-пунктов

	sizeTable = C.size();
	SLR_table = new map<string, TypeAction>[sizeTable];
	// Заполняем всю таблицу Error'ами
	for(int i = 0; i < sizeTable; i++) // Каждое состояние
	{
		for(MAP::iterator it = GS.begin(); it != GS.end(); it++) // Каждый грамматический символ
			if(strcmp((*it).first.c_str(), "S'"))
				SLR_table[i][(*it).first] = TypeAction();
		SLR_table[i]["$"] = TypeAction();
	}

	vector<Point> tmp;
	vector<char*> tmp2;
	for(int i = 0; i < sizeTable; i++) // Каждое состояние таблицы (каждое мн-во пунктов в C - C[i])
	{
		for(int j = 0; j < C[i].size(); j++) // Каждый пункт в C[i] - C[i][j]
		{
			if(C[i][j].index != C[i][j].t.size()) // Если точка стоит не в конце продукции (п.1)
			{
				// s = C[i][j].t[C[i][j].index] - грамматический символ, перед которым стоит точка
				/* (пока ACTION от GOTO не отличаем) */
				//if(GS[C[i][j].t[C[i][j].index]].terminal) // Если s - терминал
				//{
					tmp = GOTO(C[i], C[i][j].t[C[i][j].index]);
					for(int k = 0; k < C.size(); k++) // Ищем мн-во пунктов tmp в каноническом наборе C
						if(CompareVectors(tmp, C[k])) // Если нашли
						{
							if(SLR_table[i][C[i][j].t[C[i][j].index]].type != TYPE_ACTION::ERROR && GS[C[i][j].t[C[i][j].index]].terminal) // Если конфликт
							{
								if(SLR_table[i][C[i][j].t[C[i][j].index]].type != TYPE_ACTION::MOVE || SLR_table[i][C[i][j].t[C[i][j].index]].value != k) // Если записано другое действие
								{
									if(SLR_table[i][C[i][j].t[C[i][j].index]].type == TYPE_ACTION::CONFLICT) // Если уже имеется запись в таблице конфликтов
									{
										tableConflicts[SLR_table[i][C[i][j].t[C[i][j].index]].value].push_back(TypeAction(TYPE_ACTION::MOVE, k));
									}
									else // Иначе (если еще нет записи в таблице конфликтов)
									{
										vector<TypeAction> tmp3;
										tmp3.push_back(TypeAction(TYPE_ACTION::MOVE, k));
										tableConflicts.push_back(tmp3);
										SLR_table[i][C[i][j].t[C[i][j].index]] = TypeAction(TYPE_ACTION::CONFLICT, tableConflicts.size() - 1);
									}
									//cout << i << " " << C[i][j].t[C[i][j].index] << " " << SLR_table[i][C[i][j].t[C[i][j].index]].value << endl;
									access = false;
								}
							}
							else // Если нет конфликта
								SLR_table[i][C[i][j].t[C[i][j].index]] = TypeAction(TYPE_ACTION::MOVE, k);
							break;
						}
				//}
			}
			else // Иначе (если точка стоит в конце продукции) (п.2)
			{
				bool L;
				// A = C[i][j].z - заголовок продукции пункта C[i][j]
				if(strcmp(C[i][j].z, "S'")) // Если заголовок продукции не S'
				{
					tmp2 = FOLLOW(C[i][j].z); // Вычисляем FOLLOW(A)
					for(int k = 0; k < tmp2.size(); k++) // Каждый элемент мн-ва FOLLOW(A)
					{
						for(int q = 0; q < G.size(); q++) // Ищем продукцию, соответствующую пункту C[i][j]
						{
							L = true;
							if(!strcmp(G[q].z, C[i][j].z))
							{
								if(G[q].t.size() == C[i][j].t.size())
								{
									for(int w = 0; w < G[q].t.size(); w++)
									{
										if(strcmp(G[q].t[w], C[i][j].t[w]))
										{
											L = false;
											break;
										}
									}
									if(L)
									{
										if(SLR_table[i][tmp2[k]].type != TYPE_ACTION::ERROR)
										{
											if(SLR_table[i][tmp2[k]].type != TYPE_ACTION::TURN || SLR_table[i][tmp2[k]].value != q) // Если записано другое действие
											{
												if(SLR_table[i][tmp2[k]].type == TYPE_ACTION::CONFLICT) // Если уже имеется запись в таблице конфликтов
												{
													tableConflicts[SLR_table[i][tmp2[k]].value].push_back(TypeAction(TYPE_ACTION::TURN, q));
												}
												else // Иначе (если еще нет записи в таблице конфликтов)
												{
													vector<TypeAction> tmp3;
													tmp3.push_back(TypeAction(TYPE_ACTION::TURN, q));
													tableConflicts.push_back(tmp3);
													SLR_table[i][tmp2[k]] = TypeAction(TYPE_ACTION::CONFLICT, tableConflicts.size() - 1);
												}
												access = false;
											}
										}
										else
											SLR_table[i][tmp2[k]] = TypeAction(TYPE_ACTION::TURN, q);
									}
								}
							}
						}
					}
				}
				else // Иначе (если заголовок продукции - S') (п.3)
				{
					if(SLR_table[i]["$"].type != TYPE_ACTION::ERROR)
					{
						if(SLR_table[i]["$"].type == TYPE_ACTION::CONFLICT) // Если уже имеется запись в таблице конфликтов
						{
							tableConflicts[SLR_table[i]["$"].value].push_back(TypeAction(TYPE_ACTION::ACCESS));
						}
						else // Иначе (если еще нет записи в таблице конфликтов)
						{
							vector<TypeAction> tmp3;
							tmp3.push_back(TypeAction(TYPE_ACTION::ACCESS));
							tableConflicts.push_back(tmp3);
							SLR_table[i]["$"] = TypeAction(TYPE_ACTION::CONFLICT, tableConflicts.size() - 1);
						}
						access = false;
					}
					else
						SLR_table[i]["$"] = TypeAction(TYPE_ACTION::ACCESS);
				}
			}
		}
	}

	return access;
}

vector<Point> GeneratorSLRTable::CLOSURE(vector<Point> I)
{
	vector<Point> J(I);

	int size_J = J.size();
	Point p;
	bool L;
	for(int i = 0; i < size_J; i++)
	{
		if(J[i].index != J[i].t.size()) // Если пункт стоит не в конце продукции
		{
			// s = J[i].t[J[i].index] - символ грамматики, перед которым стоит пункт
			// Если s - терминал, то continue (искать нечего, с терминалов продукции не начинаются);
			if(GS[J[i].t[J[i].index]].terminal)
				continue;
			for(int j = 0; j < GS[J[i].t[J[i].index]].indexes.size(); j++) // Каждая продукция, заголовок которой - s
			{
				p = Point(G[GS[J[i].t[J[i].index]].indexes[j]], 0); // Создаем новый пункт p
				L = false;
				for(int k = 0; k < size_J; k++) // Ищем p в J. Если он есть в J, то не добавляем его
				{
					if(J[k] == p) // Если нашли
					{
						L = true;
						break;
					}
				}
				if(!L) // Если p нет в J
				{
					J.push_back(p); // Добавляем p в J
					size_J++; // Т.к. кол-во пунктов в J увеличилось
				}
			}
		}
	}

	return J;
}

vector<Point> GeneratorSLRTable::GOTO(vector<Point> I, const char* X)
{
	vector<Point> J;

	Point p;
	for(int i = 0; i < I.size(); i++) // Ищем в I те пункты, точка в которых стоит перед символом X
	{
		if(I[i].index != I[i].t.size()) // Если пункт стоит не в конце продукции
		{
			if(!strcmp(I[i].t[I[i].index], X)) // Если нашли
			{
				// Перемещаем точку за X и полученный пункт добавляем в J
				p = I[i];
				p.index++;
				J.push_back(p);
			}
		}
	}

	return CLOSURE(J);
}

vector<char*> GeneratorSLRTable::FIRST(char* s)
{
	vector<char*> res; // Множество терминалов FIRST(s)
	vector<char*> tmp;
	if(GS[s].terminal)
		res.push_back(s);
	else
	{
		for(int i = 0; i < GS[s].indexes.size(); i++) // Каждая продукция, заголовок которой - s
		{
			for(int j = 0; j < G[GS[s].indexes[i]].t.size(); j++) // Каждый грамматический символ тела продукции G[GS[s].indexes[i]]
			{
				if(!strcmp(G[GS[s].indexes[i]].t[j], s)) // Если символ тот же самый (L -> L...)
					break;
				if(GS[G[GS[s].indexes[i]].t[j]].terminal) // Если терминал (EMPTY_STR будет добавлен здесь же)
				{
					res.push_back(G[GS[s].indexes[i]].t[j]);
					break;
				}
				tmp = FIRST(G[GS[s].indexes[i]].t[j]); // Копирование
				for(int k = 0; k < tmp.size(); k++)    // ---
					res.push_back(tmp[k]);             // ---
				if(!IsEmptyStr(G[GS[s].indexes[i]].t[j])) // Если из G[GS[s].indexes[i]].t[j] нельзя породить EMPTY_STR
					break;
			}
		}
	}
	return res;
}

bool GeneratorSLRTable::IsEmptyStr(char* s)
{
	stack<char*> st;

	// Заполняем стек единственными грамматическими символами тел одиночных продукций, заголовки которых - s
	for(int i = 0; i < GS[s].indexes.size(); i++)
		if(G[GS[s].indexes[i]].t.size() == 1) // !GS[G[GS[s].indexes[i]].t[0]].terminal
			st.push(G[GS[s].indexes[i]].t[0]);

	char* tmp;
	while(!st.empty())
	{
		tmp = st.top();
		st.pop();
		if(!strcmp(tmp, "EMPTY_STR"))
			return true;
		if(!GS[tmp].terminal)
			for(int i = 0; i < GS[tmp].indexes.size(); i++)
				if(G[GS[tmp].indexes[i]].t.size() == 1)
					st.push(G[GS[tmp].indexes[i]].t[0]);
	}

	return false;
}

vector<char*> GeneratorSLRTable::FOLLOW(char* s)
{
	/*cout << s << endl;
	for(int i = 0; i < 10000000; i++)
	{
	}*/
	vector<char*> res;
	
	if(!GS[s].terminal) // Если s - нетерминал
	{
		set<char*, greater<char*>> set_res; // Используем множество, чтобы не иметь дублирующих элементов

		// 1-ое правило
		if(!strcmp(s, G[0].z)) // Если s - стартовый символ
			set_res.insert("$");

		// 2-ое правило
		vector<char*> tmp;
		for(int i = 0; i < G.size(); i++) // Каждая продукция G[i]
		{
			for(int j = 0; j < G[i].t.size() - 1; j++) // Каждый грамматический символ тела продукции G[i], кроме последнего
			{
				if(!strcmp(s, G[i].t[j])) // Если нашли s в теле продукции
				{
					tmp = FIRST(G[i].t[j + 1]); // tmp = FIRST(C) из псевдокода ниже
					for(int k = 0; k < tmp.size(); k++)
						if(strcmp(tmp[k], "EMPTY_STR"))
							set_res.insert(tmp[k]);
				}
			}
		}

		// 3-е правило
		for(int i = 0; i < G.size(); i++) // Каждая продукция G[i]
		{
			// Для A -> Bs (см. псевдокод ниже)
			if(!strcmp(G[i].t[G[i].t.size() - 1], s)) // Если последний грамматический символ этой продукции - s
			{
				if(!strcmp(s, G[i].z)) // Избавляемся от бесконечной рекурсии
					continue;
				tmp = FOLLOW(G[i].z);
				for(int j = 0; j < tmp.size(); j++)
					set_res.insert(tmp[j]);
			}
			// Для A -> BsC (см. псевдокод ниже)
			for(int j = 0; j < G[i].t.size() - 1; j++) // Каждый грамматический символ тела продукции G[i], кроме последнего
			{
				if(!strcmp(s, G[i].t[j])) // Если нашли s в теле продукции
				{
					tmp = FIRST(G[i].t[j + 1]);
					for(int k = 0; k < tmp.size(); k++)
						if(!strcmp(tmp[k], "EMPTY_STR"))
						{
							if(!strcmp(s, G[i].z)) // Избавляемся от бесконечной рекурсии
								continue;
							tmp = FOLLOW(G[i].z);
							for(int q = 0; q < tmp.size(); q++)
								set_res.insert(tmp[q]);
								//res.push_back(tmp[q]);
							break;
						}
				}
			}
		}

		for(set<char*, greater<char*>>::iterator it = set_res.begin(); it != set_res.end(); it++)
			res.push_back(*it);
		
		/* 
		ПСЕВДОКОД:

		// 2-ое правило
		vector<char*> tmp;
		for(каждая продукция из G' вида A -> BsC || A -> sC)
		{
			tmp = FIRST(C);
			for(int i = 0; i < tmp.size(); i++)
				if(strcmp(tmp[i], "EMPTY_STR"))
					res.push_back(tmp[i]);
		}

		// 3-е правило
		for(каждая продукция G' - S)
		{
			if(S имеет вид A -> Bs || (S имеет вид A -> BsC && EMPTY_STR (- FIRST(C)))
			{
				tmp = FOLLOW(A);
				for(int i = 0; i < tmp.size(); i++)
					res.push_back(tmp[i]);
			}
		}
		*/
	}

	return res;
}

bool GeneratorSLRTable::StartSLRAnalysis(vector<Token> tokens, map<string, TypeAction>*& table, int size)
{
	bool res;

	tokens.push_back(Token(int('$'), "$"));

	stack<int> states;
	states.push(0);

	int s;
	int index = 0;
	char* a = tokens[index++].GetCharName();
	while(true)
	{
		s = states.top();
		if(table[s][a].type == TYPE_ACTION::MOVE)
		{
			states.push(table[s][a].value);
			a = tokens[index++].GetCharName();
		}
		else if(table[s][a].type == TYPE_ACTION::TURN)
		{
			for(int i = 0; i < G[table[s][a].value].t.size(); i++)
				states.pop();
			states.push(table[states.top()][G[table[s][a].value].z].value);
			/*cout << G[table[s][a].value].z << " -> ";
			for(int i = 0; i < G[table[s][a].value].t.size(); i++)
				cout << G[table[s][a].value].t[i];
			cout << endl;*/
		}
		else if(table[s][a].type == TYPE_ACTION::ACCESS)
		{
			// Синтаксический анализ успешно завершен
			res = true;
			break;
		}
		else
		{
			// Синтаксическая ошибка
			res = false;
			break;
		}
	}

	return res;
}