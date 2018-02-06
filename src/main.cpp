#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <string>
#include "NKA.h" // Недетерминированный конечный автомат
#include "Token.h" // Токен
#include "Scanner.h" // Лексический анализатор
#include "Production.h" // Продукция
#include "Item.h" // LR(0)-пункт
#include "GeneratorSLRTable.h" // Генератор SLR-таблицы и синтаксический ПС-анализатор
using namespace std;

void FillingTransitionTable(map<char, vector<int>>*, int); // Процедура заполнения таблицы переходов
void FillingFinishedStates(bool*, int); // Процедура заполнения массива принимающих состояний
void FillingMapStatesToTokensNames(int*, int); // Процедура заполнения отображения принимающих состояний на имена токенов
void AddHelper(map<char, vector<int>>*, int, char, int); // Для помощи в заполнении таблицы переходов большого НКА

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "russian");
	ofstream outProtocol("protocol.txt");

	try
	{
		/*
		 * Correct math expression examples:
		 * (-ln(e)) + 4 * x * (7) / x^5 + sin(x + 8)
		 * sinlg(x * (254 + 80.55)) / ln(e)
		 */

		const int n = 58; // Кол-во состояний НКА
		string fileContent; // Входная строка
		map<char, vector<int>>* move; // Таблица переходов НКА

		move = new map<char, vector<int>>[n];
		bool fStates[n] = { 0 };

		int* mSt = new int[n];

		FillingTransitionTable(move, n);
		FillingFinishedStates(fStates, n);
		FillingMapStatesToTokensNames(mSt, n);

		ifstream inExpression;
		inExpression.open("expression.txt");

		getline(inExpression, fileContent);
		inExpression.close();

		char *str = new char[fileContent.length() + 1];
		strcpy(str, fileContent.c_str());

		Scanner lexi(move, n, fStates, true, mSt); // true - считать все цифры, как символ '~'
		outProtocol << "Expression: " << str << endl;

		vector<Token> tokens;
		if (lexi.Scan(str, tokens))
		{
			outProtocol << "Scanner: success" << endl;

			ofstream outTokens("tokens.txt");
			for (int i = 0; i < tokens.size(); i++)
			{
				outTokens << tokens[i] << endl;
			}

			outProtocol << "Tokens was printed in tokens.txt" << endl;

			vector<Production> G; // Грамматика G'

			G.push_back(Production("S'", 1, "E")); // S' -> E
			G.push_back(Production("E", 3, "E", "+", "T")); // E -> E + T
			G.push_back(Production("E", 3, "E", "-", "T")); // E -> E - T
			G.push_back(Production("E", 1, "T")); // E -> T
			G.push_back(Production("T", 3, "T", "*", "K")); // T -> T * K
			G.push_back(Production("T", 3, "T", "/", "K")); // E -> T / K
			G.push_back(Production("T", 1, "K")); // T -> K
			G.push_back(Production("K", 3, "K", "^", "P")); // K -> K ^ P
			G.push_back(Production("K", 1, "P")); // K -> P
			G.push_back(Production("P", 2, "tr", "P")); // P -> tr P
			G.push_back(Production("P", 2, "sqrt", "P")); // P -> sqrt P
			G.push_back(Production("P", 2, "lc", "P")); // P -> lc P
			G.push_back(Production("P", 3, "log", "P", "P")); // P -> log P P
			G.push_back(Production("P", 1, "F")); // P -> F
			G.push_back(Production("F", 1, "x")); // F -> x
			G.push_back(Production("F", 1, "const")); // F -> const
			G.push_back(Production("F", 1, "num")); // F -> num
			G.push_back(Production("F", 3, "(", "E", ")")); // F -> ( E )
			G.push_back(Production("F", 3, "|", "E", "|")); // F -> | E |
			G.push_back(Production("F", 4, "(", "-", "E", ")")); // F -> ( - E )

			vector<char*> v;
			v.push_back("S'");
			v.push_back("E");
			v.push_back("T");
			v.push_back("K");
			v.push_back("P");
			v.push_back("F");
			v.push_back("+");
			v.push_back("-");
			v.push_back("*");
			v.push_back("/");
			v.push_back("^");
			v.push_back("tr");
			v.push_back("sqrt");
			v.push_back("lc");
			v.push_back("log");
			v.push_back("x");
			v.push_back("const");
			v.push_back("num");
			v.push_back("(");
			v.push_back(")");
			v.push_back("|");

			GeneratorSLRTable generator(G, v, 6);

			ofstream out("LR(0).txt");
			generator.PrintC(out, generator.CreateC());
			outProtocol << "LR(0) set was printed in LR(0).txt" << endl;

			int size;
			map<string, TypeAction>* SLR_table;
			vector<vector<TypeAction>> tableConflicts;

			if (!generator.CreateSLRTable(SLR_table, size, tableConflicts))
			{
				ofstream out2("conflicts.txt");
				outProtocol << "There are conflicts. See conflicts.txt." << endl;
				generator.PrintConflictsTable(out2, tableConflicts);
				out2.close();
			}
			ofstream out3("slr-table.txt");
			generator.PrintSLRTable(out3, SLR_table, size);
			outProtocol << "SLR Table was printed in slr-table.txt" << endl;

			if (generator.StartSLRAnalysis(tokens, SLR_table, size))
			{
				outProtocol << "Parser: success" << endl;
				cout << "Ready with succeed. See protocol.txt" << endl;
			}
			else
			{
				outProtocol << "Parser: failure" << endl;
				cout << "Ready with failure. See protocol.txt" << endl;
			}

			out.close();
			out3.close();
			outTokens.close();
		}
		else
		{
			outProtocol << "Scanner: failure" << endl;
		}
	}
	catch (exception& ex)
	{
		cout << ex.what() << endl;
	}

	outProtocol.close();

	system("pause");
	return 0;
}

void AddHelper(map<char, vector<int>>* move, int index, char c, int finish) // Для помощи в заполнении таблицы переходов большого НКА
{
	move[index][c].clear();
	move[index][c].push_back(finish);
}
void FillingTransitionTable(map<char, vector<int>>* move, int n)
{
	for (int i = 0; i < n; i++)
	{
		move[i]['~'].push_back(NKA::EMPTY_SET);
		move[i]['.'].push_back(NKA::EMPTY_SET);
		move[i]['s'].push_back(NKA::EMPTY_SET);
		move[i]['i'].push_back(NKA::EMPTY_SET);
		move[i]['n'].push_back(NKA::EMPTY_SET);
		move[i]['c'].push_back(NKA::EMPTY_SET);
		move[i]['o'].push_back(NKA::EMPTY_SET);
		move[i]['t'].push_back(NKA::EMPTY_SET);
		move[i]['g'].push_back(NKA::EMPTY_SET);
		move[i]['e'].push_back(NKA::EMPTY_SET);
		move[i]['p'].push_back(NKA::EMPTY_SET);
		move[i]['q'].push_back(NKA::EMPTY_SET);
		move[i]['r'].push_back(NKA::EMPTY_SET);
		move[i]['l'].push_back(NKA::EMPTY_SET);
		move[i]['+'].push_back(NKA::EMPTY_SET);
		move[i]['-'].push_back(NKA::EMPTY_SET);
		move[i]['*'].push_back(NKA::EMPTY_SET);
		move[i]['/'].push_back(NKA::EMPTY_SET);
		move[i]['^'].push_back(NKA::EMPTY_SET);
		move[i]['x'].push_back(NKA::EMPTY_SET);
		move[i]['('].push_back(NKA::EMPTY_SET);
		move[i][')'].push_back(NKA::EMPTY_SET);
		move[i]['|'].push_back(NKA::EMPTY_SET);
		move[i]['@'].push_back(NKA::EMPTY_SET);
	}
	move[0]['@'].clear();
	move[0]['@'].push_back(1);
	move[0]['@'].push_back(5);
	move[0]['@'].push_back(9);
	move[0]['@'].push_back(12);
	move[0]['@'].push_back(16);
	move[0]['@'].push_back(18);
	move[0]['@'].push_back(21);
	move[0]['@'].push_back(26);
	move[0]['@'].push_back(30);
	move[0]['@'].push_back(33);
	move[0]['@'].push_back(36);
	move[0]['@'].push_back(40);
	move[0]['@'].push_back(42);
	move[0]['@'].push_back(44);
	move[0]['@'].push_back(46);
	move[0]['@'].push_back(48);
	move[0]['@'].push_back(50);
	move[0]['@'].push_back(52);
	move[0]['@'].push_back(54);
	move[0]['@'].push_back(56);

	move[1]['s'].clear();
	move[1]['s'].push_back(2);

	move[2]['i'].clear();
	move[2]['i'].push_back(3);

	move[3]['n'].clear();
	move[3]['n'].push_back(4);

	move[5]['c'].clear();
	move[5]['c'].push_back(6);

	move[6]['o'].clear();
	move[6]['o'].push_back(7);

	move[7]['s'].clear();
	move[7]['s'].push_back(8);

	move[9]['t'].clear();
	move[9]['t'].push_back(10);

	move[10]['g'].clear();
	move[10]['g'].push_back(11);

	move[12]['c'].clear();
	move[12]['c'].push_back(13);

	move[13]['t'].clear();
	move[13]['t'].push_back(14);

	move[14]['g'].clear();
	move[14]['g'].push_back(15);

	move[16]['e'].clear();
	move[16]['e'].push_back(17);

	move[18]['p'].clear();
	move[18]['p'].push_back(19);

	move[19]['i'].clear();
	move[19]['i'].push_back(20);

	move[21]['s'].clear();
	move[21]['s'].push_back(22);

	move[22]['q'].clear();
	move[22]['q'].push_back(23);

	AddHelper(move, 23, 'r', 24);
	AddHelper(move, 24, 't', 25);
	AddHelper(move, 26, 'l', 27);
	AddHelper(move, 27, 'o', 28);
	AddHelper(move, 28, 'g', 29);
	AddHelper(move, 30, 'l', 31);
	AddHelper(move, 31, 'g', 32);
	AddHelper(move, 33, 'l', 34);
	AddHelper(move, 34, 'n', 35);
	AddHelper(move, 36, '~', 37);
	AddHelper(move, 37, '@', 36);
	move[37]['.'].push_back(38);
	move[37]['@'].push_back(39);
	AddHelper(move, 38, '~', 39);
	AddHelper(move, 39, '@', 38);
	AddHelper(move, 40, '+', 41);
	AddHelper(move, 42, '-', 43);
	AddHelper(move, 44, '*', 45);
	AddHelper(move, 46, '/', 47);
	AddHelper(move, 48, '^', 49);
	AddHelper(move, 50, 'x', 51);
	AddHelper(move, 52, '(', 53);
	AddHelper(move, 54, ')', 55);
	AddHelper(move, 56, '|', 57);
}

void FillingFinishedStates(bool* fStates, int n)
{
	fStates[4] = 1;
	fStates[8] = 1;
	fStates[11] = 1;
	fStates[15] = 1;
	fStates[17] = 1;
	fStates[20] = 1;
	fStates[25] = 1;
	fStates[29] = 1;
	fStates[32] = 1;
	fStates[35] = 1;
	fStates[39] = 1;
	fStates[41] = 1;
	fStates[43] = 1;
	fStates[45] = 1;
	fStates[47] = 1;
	fStates[49] = 1;
	fStates[51] = 1;
	fStates[53] = 1;
	fStates[55] = 1;
	fStates[57] = 1;
}

void FillingMapStatesToTokensNames(int* mSt, int n)
{
	for (int i = 0; i < n; i++)
		mSt[i] = -1000;
	mSt[4] = -Token::TR; // Если значение отрицательно, то требуется атрибут
	mSt[8] = -Token::TR;
	mSt[11] = -Token::TR;
	mSt[15] = -Token::TR;
	mSt[17] = -Token::CONST;
	mSt[20] = -Token::CONST;
	mSt[25] = Token::SQRT;
	mSt[29] = Token::LOG;
	mSt[32] = -Token::LC;
	mSt[35] = -Token::LC;
	mSt[39] = -Token::NUM;
	mSt[41] = int('+');
	mSt[43] = int('-');
	mSt[45] = int('*');
	mSt[47] = int('/');
	mSt[49] = int('^');
	mSt[51] = int('x');
	mSt[53] = int('(');
	mSt[55] = int(')');
	mSt[57] = int('|');
}