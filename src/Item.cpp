#include "Item.h"

Point::Point()
{
	index = -2;
}

Point::Point(Production pr, int i) : Production(pr)
{
	if(i == MAX_POINT)
		i = t.size();
	index = i;
}

ostream& operator<<(ostream& out, const Point& p)
{
	out << p.z << " -> ";
	for(int i = 0; i < p.t.size(); i++)
	{
		if(i == p.index)
			out << "•"; // char(26)
		out << p.t[i];
	}
	if(p.index == p.t.size())
		out << "•"; // char(26)
	return out;
}

bool Point::operator==(const Point& p)
{
	if(index == p.index && !strcmp(z, p.z) && t.size() == p.t.size())
	{
		for(int i = 0; i < t.size(); i++)
			if(strcmp(t[i], p.t[i]))
				return false;
		return true;
	}
	return false;
}

bool Point::operator!=(const Point& p)
{
	return !(*this == p);
}
