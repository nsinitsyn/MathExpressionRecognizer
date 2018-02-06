#include <iostream>
#include "Token.h"

void Token::InitMN()
{
	cTokens = 262;
	mN = new char*[cTokens];
	for(int i = 0; i < cTokens; i++)
		mN[i] = new char[20]; // 20 - макс. длина имени токена
	strcpy(mN[256], "num");
	strcpy(mN[257], "tr");
	strcpy(mN[258], "const");
	strcpy(mN[259], "sqrt");
	strcpy(mN[260], "log");
	strcpy(mN[261], "lc");
}

Token::Token() : attr(false)
{
	InitMN();
	n = -1;
	v = new char[2];
	strcpy(v, "0");
}

Token::Token(int name) : n(name), attr(false) 
{
	InitMN();
	v = new char[2];
	strcpy(v, "0");
}

Token::Token(int name, char* value) : n(name), attr(true)
{
	InitMN();
	v = new char[strlen(value) + 1];
	strcpy(v, value);
}

Token::Token(const Token& t)
{
	InitMN();
	attr = t.attr;
	n = t.n;
	v = new char[strlen(t.v) + 1];
	strcpy(v, t.v);
}

Token::~Token()
{
	delete[] v;
	for(int i = 0; i < cTokens; i++)
		delete[] mN[i];
	delete[] mN;
}

Token& Token::operator=(const Token& t)
{
	attr = t.attr;
	n = t.n;
	delete[] v;
	v = new char[strlen(t.v) + 1];
	strcpy(v, t.v);
	return *this;
}

int Token::GetName() 
{
	return n;
}

char* Token::GetType()
{
	char* str = new char[strlen(v) + 1];
	strcpy(str, v);
	return str;
}

bool Token::GetAttributeState()
{ 
	return attr; 
}

char* Token::GetCharName()
{
	char* res = new char[20];
	if(n < 256)
	{
		res[0] = char(n);
		res[1] = '\0';
	}
	else
		strcpy(res, mN[n]);
	return res;
}

ostream& operator<<(ostream& out, const Token& t)
{
	out << "token<";
	if(t.n < 256)
		out << char(t.n);
	else
		out << t.mN[t.n];
	if(t.attr)
		out << "," << t.v << ">";
	else
		out << ">";

	return out;
}