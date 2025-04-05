#pragma once
#include <iostream>
#include <assert.h>
using namespace std;

class Date {
	friend ostream& operator<<(ostream& out, const Date& d);
	friend istream& operator>>(istream& in, Date& d);
public:
	Date(int year, int month, int day) 	{
		if (month > 0 && month < 13 && day > 0 && day <= GetMonthDay(year, month)) {
			_year = year;
			_month = month;
			_day = day;
		}
		else {
			cout << "非法日期" << endl;
			assert(false);
		}
	}

	Date(const Date& d) {
		_year = d._year;
		_month = d._month;
		_day = d._day;
	}

	void Print() const {
		cout << _year << "年" << _month << "月" << _day << "日" << endl;
	}

	bool operator<(const Date& d) const;
	bool operator==(const Date& d) const;
	bool operator<=(const Date& d) const;
	bool operator>(const Date& d) const;
	bool operator>=(const Date& d) const;
	bool operator!=(const Date& d) const;

	int GetMonthDay(const int year, const int month) const;

	Date& operator+=(int day);
	Date operator+(int day) const;

	Date& operator-=(int day);
	Date operator-(int day) const;

	Date& operator++();
	Date operator++(int);

	Date& operator--();
	Date operator--(int);

	int operator-(const Date& d) const;

	~Date() {}
private:
	int _year = 1;
	int _month = 1;
	int _day = 1;
};

ostream& operator<<(ostream& out, const Date& d);
istream& operator>>(istream& in, Date& d);