#pragma once
#ifndef __T_KAHAN_ADDER__
#define __T_KAHAN_ADDER__

// 补偿求和算法，用于浮点数相加
struct TKahanAdder
{
	double sum, carry, y;
	TKahanAdder(const double b = 0.0)
	{
		sum = b;
		carry = 0.0;
		y = 0.0;
	}
	inline void add(const double b)
	{
		y = b - carry;
		const double t = sum + y;
		carry = (t - sum) - y;
		sum = t;
	}
};

#endif