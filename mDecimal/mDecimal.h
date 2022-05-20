#pragma once
#include <iostream>

struct BasicDecimal {

	typedef long long LINT; // local int

//protected:
	LINT d = 0; // число
	unsigned char m{ 8 }; // точность /10^m
};

struct PropertiesBasicDecimal : public BasicDecimal {

//protected:

	constexpr static char getcountbit();
	constexpr static LINT getmaxvalue();
	constexpr static char countdigits(LINT);
	static LINT leftpart(const PropertiesBasicDecimal&);

	static char countbit; // число бит
	static char maxdigits; // число цифр
	static LINT maxvalue, minvalue;
};

struct mdecimal :private PropertiesBasicDecimal {

	constexpr mdecimal();

	template <typename T>
	constexpr explicit mdecimal(const T, const unsigned char);

	mdecimal operator + (const mdecimal&);

	mdecimal operator - (const mdecimal&);

	mdecimal operator * (const mdecimal&);

	mdecimal operator / (const mdecimal&);

	mdecimal& operator = (const mdecimal&);

	bool operator < (const mdecimal&);

	~mdecimal() = default;

private:

	static bool overflow_add(LINT&, LINT&);

	friend std::ostream& operator << (std::ostream&, const mdecimal&);

};
