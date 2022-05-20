// mDecimal.cpp : Определяет функции для статической библиотеки.
//

#include "pch.h"
#include "framework.h"
#include "mDecimal.h"

constexpr char PropertiesBasicDecimal::getcountbit() {
	char res = 0;
	LINT a = 0, b = 0;
	do
	{
		a = b;
		b |= 1;
		b = (b << 1);
		res += 1;
	} while (a < b);
	return res;
}

constexpr PropertiesBasicDecimal::LINT PropertiesBasicDecimal::getmaxvalue() {
	LINT res = 1;
	constexpr char cb = PropertiesBasicDecimal::getcountbit();

	for (char i = 1; i < cb; ++i) {
		res = (res << 1);
		res |= 1;
	}
	return res;
}

constexpr char PropertiesBasicDecimal::countdigits(BasicDecimal::LINT x) {
	char res = 0;
	do
		res++;
	while (x /= 10);
	return res;
}

BasicDecimal::LINT PropertiesBasicDecimal::leftpart(const PropertiesBasicDecimal& v) {
	return LINT(v.d / pow(10, v.m));
}


char PropertiesBasicDecimal::countbit = PropertiesBasicDecimal::getcountbit();
PropertiesBasicDecimal::LINT PropertiesBasicDecimal::maxvalue = PropertiesBasicDecimal::getmaxvalue();
PropertiesBasicDecimal::LINT PropertiesBasicDecimal::minvalue = PropertiesBasicDecimal::LINT(pow(2, PropertiesBasicDecimal::countbit));
char PropertiesBasicDecimal::maxdigits = PropertiesBasicDecimal::countdigits(PropertiesBasicDecimal::maxvalue);


constexpr mdecimal::mdecimal() { // конструктор по умолчанию, constexpr - класс литеральный
}

template <typename T>
constexpr mdecimal::mdecimal(const T lp = 0, const unsigned char m1 = 8) {
	m = m1;
	if (m1 > maxdigits) m = maxdigits;
	d = LINT(lp * pow(10, m) + 0.5); // 0.5 - окгругление
}


mdecimal mdecimal::operator + (const mdecimal& v) {
	mdecimal res;
	LINT o1;

	// в первом приближении - приведение к максимальной точности без контроля переполнения в результате сложения большого и малого чисел: нужно ограничение на точность
	if (m < v.m) {

		o1 = BasicDecimal::LINT(d * pow(10, v.m - m));

		if (overflow_add(o1, d)) {
			throw std::overflow_error("Переполнение");
		}

		res.d = o1 + v.d;
		res.m = v.m;
	}
	else {

		o1 = LINT(v.d * pow(10, m - v.m));

		if (overflow_add(o1, d)) {
			throw std::overflow_error("Переполнение");
		}

		res.d = d + o1;
		res.m = m;
	}

	return res;
}

mdecimal mdecimal::operator - (const mdecimal& v) {
	mdecimal res = v;
	res.d = -res.d;
	return *this + res;
}

mdecimal mdecimal::operator * (const mdecimal& v) {
	mdecimal res;

	res.d = d * v.d;
	res.m = m + v.m;
	return res;
}

mdecimal mdecimal::operator / (const mdecimal& v) {
	mdecimal res;
	LINT o1;

	// в первом приближении - приведение к максимальной точности без контроля переполнения
	if (m < v.m) {

		res.m = maxdigits - countdigits(d) - 1;

		o1 = LINT(d * pow(10, res.m));

		res.d = o1 / v.d;
		res.m -= 1;
	}
	else {

		res.m = maxdigits - countdigits(d) - 1; // -1, т.к. диапазон типа не 9999...

		o1 = LINT(d * pow(10, res.m));

		res.d = o1 / v.d;
		res.m += 1;
	}

	return res;
}

mdecimal& mdecimal::operator = (const mdecimal& v) {
	// приведение к точности получателя без контроля переполнения
	if (m < v.m) {
		d = LINT(v.d * pow(10, m - v.m) + 0.5); // округление до точности получателя
	}
	else {
		d = LINT(v.d * pow(10, m - v.m));

		//char dl = countdigits(leftpart(*this));
		//char dr = countdigits(leftpart(v));
		//char c; // итоговое число цифр целой части
		//
		//c = dl;
		//if (dl < dr) {
		//	c = dr;
		//}
		// уменьшаем точность на величину 
		//d = LINT(v.d * pow(10, m - v.m - c));
		//m -= c;
	}

	return *this;
}

bool mdecimal::operator < (const mdecimal& v) {
	LINT lp = leftpart(*this), lp_v = leftpart(v);

	if (lp < lp_v)
		return true;
	else
		if (d - lp < v.d - lp_v)
			return true;

	return false;
}

bool mdecimal::overflow_add(BasicDecimal::LINT& a, BasicDecimal::LINT& b) {
	// https://ru.stackoverflow.com/questions/513736/Можно-ли-на-языках-c-c-определить-целочисленное-переполнение
	// https://wiki.sei.cmu.edu/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow

	if ((b > 0) && (a > (maxvalue - b))
		|| (b < 0) && (a < (minvalue - b))) {
		return true;
		//throw std::overflow_error("Переполнение");
	}
	return false;
}

std::ostream& operator << (std::ostream& os, const mdecimal& md) {
	mdecimal::LINT lp = mdecimal::leftpart(md);
	mdecimal::LINT rp = mdecimal::LINT(md.d - lp * pow(10, md.m));
	std::cout << lp << ".";
	mdecimal::LINT t = rp % 10;
	char mm = 0;
	//while (t == 0) {
	//	rp = rp / 10;
	//	t = rp % 10;
	//	mm += 1;
	//	std::cout << "0";
	//}
	for (int i = md.m - mm - 1; rp < 1 * pow(10, i); --i) { std::cout << "0"; }
	std::cout << rp;
	return os;
}
