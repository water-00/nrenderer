#pragma once
#ifndef __T_MARKOV_CHAIN__
#define __T_MARKOV_CHAIN__

#include "Metropolis.hpp"

namespace Metropolis
{
	struct TMarkovChain
	{
		double u[NumStates]; // 一条MarkovChain中放随机数的数组
		PathContribution C;

		// xorshift PRNG，生成[0, 1) double
		inline double rnd() {
			static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
			unsigned int t = x ^ (x << 11);
			x = y;
			y = z;
			z = w;
			return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8))) * (1.0 / 4294967296.0);
		}

		TMarkovChain()
		{
			for (int i = 0; i < NumStates; i++)
				u[i] = rnd();
		}
	};
}


#endif