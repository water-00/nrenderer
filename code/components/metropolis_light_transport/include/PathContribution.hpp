#pragma once
#ifndef __PATH_CONTRIBUTION__
#define __PATH_CONTRIBUTION__

#include "Metropolis.hpp"

#define PI ((double)3.1415926535898)
#define MAX(x, y) ((x > y) ? x : y)
#define MIN(x, y) ((x < y) ? x : y)

// parameters
#define MinPathLength 3 // avoid sampling direct illumination. light subpath + eye subpath + 连接，一条完整的BPT path至少3段
#define MaxPathLength 20
#define N_Init 10000
#define LargeStepProb 0.3

// scene independent constants
#define NumRNGsPerEvent 2									 // 每次反射所用的随机数数量
#define MaxVertex (MaxPathLength + 1)						 // 一条路径的最大顶点数
#define NumStatesSubpath ((MaxVertex + 2) * NumRNGsPerEvent) // 一条子路径所用到的最多随机数
#define NumStates (NumStatesSubpath * 2)					 // 两条子路径所能组成的最多的新路径的数量

#define light_id -3
#define camera_id -2


namespace Metropolis
{
	// path data
	struct Vert
	{
		Vec3d p, n;
		int id;
		Vert() {};
		Vert(Vec3d p_, Vec3d n_, int id_) : p(p_), n(n_), id(id_) {}
	};
	struct Path
	{
		Vert x[MaxVertex];
		int n; // 一条路径中的vertex数量
		Path() { n = 0; }
	};
	struct Contribution
	{
		double x, y;
		Vec3d c; // 颜色
		Contribution() {};
		Contribution(double x_, double y_, Vec3d c_) : x(x_), y(y_), c(c_) {}
	};
	struct PathContribution
	{
		Contribution c[MaxVertex * MaxVertex];
		int n;
		double sc; // scalar contribution function
		PathContribution()
		{
			n = 0;
			sc = 0.0;
		}
	};
}

#endif