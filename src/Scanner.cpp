#include <stack>
#include "Scanner.h"

Scanner::Scanner(map<char, vector<int>>* tranTable, int n, bool* finalStates, bool stateForDigits, int* mapStToNames) : NKA(tranTable, n, finalStates, stateForDigits)
{
	if(!mapStToNames)
		throw exception("Конструктору Scanner были переданы неверные параметры");

	mSt = new int[n];
	for(int i = 0; i < n; i++)
		mSt[i] = mapStToNames[i];
}

bool Scanner::Scan(char* str, vector<Token>& tokens)
{
	if(!str)
		throw exception("Методу Scan были переданы неверные параметры");

	stack<set<int>> oldS; // Стек для отката по состояниям

	set<int> S = EmptyClosure(0);
	oldS.push(S);

	char* lexemeBegin = str;
	char* forward = str;

	bool err;
	while(true)
	{
		if(*forward != ' ') // Пробелы пропускаем
		{
			S = EmptyClosure(Move(S, *forward));
			if(S.empty()) // Если двигаться по символу *forward некуда
			{
				err = true;
				while(!oldS.empty()) // Откат по множествам состояний
				{
					S = oldS.top();
					oldS.pop();
					if(err)
					{
						for(set<int>::iterator it = S.begin(); it != S.end(); it++) // Ищем среди множества состояний принимающее
							if(fStates[*it])
							{
								err = false; // Не выходим из цикла while, чтобы очистить стек oldS
								
								int nameToken = mSt[*it]; // В зависимости от принимающего состояния *it определяем имя токена nameToken
								if(nameToken == -1000)
									throw exception("Метод Scan: Ошибка заполнения массива fStates или mSt");

								// Определяем атрибут - значение токена (если нужно). Строка, нач. с lexemeBegin, а зак. на forward - 1
								if(nameToken < 0)
								{
									nameToken *= -1;
									char* pTemp = lexemeBegin;
									int lengthAttrToken = 0; // Считаем длину токена
									while(pTemp++ != forward)
										lengthAttrToken++;
									char* attrToken = new char[lengthAttrToken + 1];
									pTemp = lexemeBegin;
									for(int i = 0; i < lengthAttrToken; i++)
										attrToken[i] = *pTemp++;
									attrToken[lengthAttrToken] = '\0';

									tokens.push_back(Token(nameToken, attrToken));
								}
								else
									tokens.push_back(Token(nameToken));

								break;
							}
					}
				}
				if(err)
					return false; // Лексическая ошибка

				if(!*forward)
					break;

				lexemeBegin = forward;
				forward--;
				S = EmptyClosure(0);
				oldS.push(S);
			}
			else
				oldS.push(S);
		}
		if(!*forward)
			break;
		forward++;
	}

	return true; // Лексический анализ успешно завершен. Массив tokens содержит все обнаруженные токены
}