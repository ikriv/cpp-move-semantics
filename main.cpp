// MoveSemantics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "logger.h"
#include "matrix_counter.h"

//#define MOVE_SEMANTICS
#include "matrix.h"

using namespace std;
using namespace move_semantics;

int main()
{
	cout << "==============" << " m " << "==============" << endl;
	auto m = create_matrix<int>("m", 10, 10, [](int i, int j) { return (i+1)*(j+1); });
	cout << endl << m << endl;

	cout << "==============" << " unit " << "==============" << endl;
	auto unit = fill_matrix("unit", 10, 10, 1);

	cout << "==============" << " L+L " << "==============" << endl;	
	auto m1 = m + unit;
	cout << endl << m1 << endl;

	cout << "==============" << " L+R " << "==============" << endl;
	auto m2 = m + fill_matrix("unitA", 10, 10, 1);
	cout << endl << m2 << endl;

	cout << "==============" << " R+L " << "==============" << endl;
	auto m3 = fill_matrix("unitB", 10, 10, 1) + m;
	cout << endl << m3 << endl;

	cout << "==============" << " R+R+R " << "==============" << endl;
	auto m4 = fill_matrix("unitC", 10, 10, 1) + fill_matrix("twos", 10, 10, 2) + fill_matrix("minusOnes", 10, 10, -1);
	cout << endl << m4 << endl;

	cout << "Total matrices created: " << matrix_counter::matrices_created() << endl;

	cout << "==============" << "End of main body" << "==============" << endl;

    return 0;
}
