#include "Date.h"

bool Date::operator<(const Date& d) const {
	if (_year < d._year) {
		return true;
	}
	else if (_year == d._year && _month < d._month) {
		return true;
	}
	else if (_year == d._year && _month == d._month && _day < d._day) {
		return true;
	}
	else {
		return false;
	}
}

bool Date::operator==(const Date& d) const {
	return ((_year == d._year) && (_month == d._month) && (_day == d._day));
}

bool Date::operator<=(const Date& d) const {
	return ((*this < d) || (*this == d));
}

bool Date::operator>(const Date& d) const {
	return !(*this <= d);
}

bool Date::operator>=(const Date& d) const {
	return !(*this < d);
}

bool Date::operator!=(const Date& d) const {
	return !(*this == d);
}

int Date::GetMonthDay(const int year, const int month) const {
	if (month < 1 || month > 12) {
		cout << "月份非法" << endl;
	}

	static int monthday[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };

	if ((month == 2) && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
		return 29;
	}
	else {
		return monthday[month];
	}
}

Date& Date::operator+=(int day) {
	if (day < 0) {
		return *this -= abs(day);
	}

	_day += day;

	while (_day > GetMonthDay(_year, _month)) {
		_day -= GetMonthDay(_year, _month);
		++_month;
		if (_month > 12) {
			++_year;
			_month = 1;
		}
	}

	return *this;
}

Date Date::operator+(int day) const {
	Date tmp(*this);
	tmp += day;
	return tmp;
}

Date& Date::operator-=(int day) {
	if (day < 0) {
		return *this += abs(day);
	}

	_day -= day;

	while (_day <= 0) {
		if (_month == 1) {
			--_year;
			_month = 12;
		}
		else {
			--_month;
		}
		_day += GetMonthDay(_year, _month);
	}

	return *this;
}

Date Date::operator-(int day) const {
	Date tmp(*this);
	tmp -= day;
	return tmp;
}

Date& Date::operator++() {
	return *this += 1;
}

Date Date::operator++(int) {
	Date tmp(*this);
	*this += 1;
	return tmp;
}

Date& Date::operator--() {
	return *this -= 1;
}

Date Date::operator--(int) {
	Date tmp(*this);
	*this -= 1;
	return tmp;
}

int Date::operator-(const Date& d) const {
	Date big = *this;
	Date small = d;
	int flag = 1;

	if (*this < d) {
		big = d;
		small = *this;
	}

	int n = 0;
	while (small != big) {
		++small;
		++n;
	}

	return n * flag;
}

ostream& operator<<(ostream& out, const Date& d) {
	out << d._year << "年" << d._month << "月" << d._day << "日" << endl;
	return out;
}

istream& operator>>(istream& in, Date& d) {
	int year, month, day;
	in >> year >> month >> day;

	if (month > 0 && month < 13 &&  day > 0 && day <= d.GetMonthDay(year, month)) {
		d._year = year;
		d._month = month;
		d._day = day;
	}
	else {
		cout << "非法日期" << endl;
		assert(false);
	}

	return in;
}