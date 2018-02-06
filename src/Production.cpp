#include "Production.h"

Production::Production() : t(1)
{
	z = new char[5];
	strcpy(z, "NULL");
	t[0] = new char[5];
	strcpy(t[0], "NULL");
}

Production::Production(char* zg, int count, char* s, ...)
{
	z = new char[strlen(zg) + 1];
	strcpy(z, zg);
	char** p = &s; // ** - чтобы указывать на стек (список параметров), а не делать указатель, указывающий на ту же строку, что и s
	for(int i = 0; i < count; i++)
	{
		t.push_back(*p);
		p++;
	}
}

Production::Production(const Production& pr) : t(pr.t.size())
{
	z = new char[strlen(pr.z) + 1];
	strcpy(z, pr.z);
	for(int i = 0; i < pr.t.size(); i++)
	{
		t[i] = new char[strlen(pr.t[i]) + 1];
		strcpy(t[i], pr.t[i]);
	}
}

ostream& operator<<(ostream& out, const Production& p)
{
	out << p.z << " -> ";
	for(int i = 0; i < p.t.size(); i++)
		cout << p.t[i];
	return out;
}

bool Production::operator==(const Production& p)
{
	if(!strcmp(z, p.z) && t == p.t)
		return true;
	return false;
}