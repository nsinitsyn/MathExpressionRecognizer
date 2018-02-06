#pragma once
#include <iostream>
using namespace std;

class Token
{
	int n; // имя токена
	char* v; // значение атрибута
	bool attr; // установлен ли атрибут
	char** mN; // Отображение числового имени токена на строчное имя
	int cTokens; // Число токенов
	void InitMN(); // Инициализация mN. Вызывается во всех конструкторах

public:
	enum { NUM = 256, TR = 257, CONST = 258, SQRT = 259, LOG = 260, LC = 261 }; // Возможные имена токенов, не считая промежуток от 0 до 255 для одиночных символов

	Token();
	Token(int);
	Token(int, char*);
	Token(const Token&);
	~Token();

	Token& operator=(const Token&);
	int GetName(); // Получить имя токена (число)
	char* GetType(); // Получить значение атрибута
	bool GetAttributeState(); // Получить факт наличия атрибута
	char* GetCharName(); // Получить символьное представление имени токена (терминал)
	friend ostream& operator<<(ostream&, const Token&); // Вывод токена в виде <имя токена, значение атрибута> (имя токена в символьном виде)
};