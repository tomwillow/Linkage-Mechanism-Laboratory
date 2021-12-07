#include <iostream>
#include "TEquations.h"

int main()
{
	setlocale(LC_ALL, "");
	using std::endl;

	//TEquations Eq;

	//Eq.max_step = 100;
	//Eq.epsilon = 1e-6;
	//Eq.DefineVariable(&COUT, TEXT("x y"), TEXT("0.1 0.1"));
	//Eq.AddEquation(&COUT, TEXT("sin(x)^2+x*y+y-3"), false);
	//Eq.AddEquation(&COUT, TEXT("4*x+y^2"), false);
	//Eq.BuildJacobian(&COUT);
	//Eq.SolveEquations(&COUT);

	//TEquations Eq;

	//Eq.max_step = 100;
	//Eq.epsilon = 1e-16;
	//Eq.DefineVariable(&COUT, TEXT("x"),TEXT("0"));
	//Eq.AddEquation(&COUT, TEXT("sin(x)*(cos(x)/cos(0.6981)-1)^1.5-0.0027"), false);
	//Eq.BuildJacobian(&COUT);
	//Eq.SolveEquations(&COUT);

	TExpressionTree Expr;
	TVariableTable VarTable;
	VarTable.Define(&COUT, TEXT("x y"));
	Expr.LinkVariableTable(&VarTable);

	COUT << TEXT(">>Read:") << endl;
	Expr.Read(TEXT("-sin(x)+0.75*y^3-(y/ln(x))*4"), true);

	//COUT << TEXT(">>Subs(TEXT(\"x y\"), TEXT(\"x+1 2*y\"), true):") << endl;
	//COUT << Expr.Subs(TEXT("x y"), TEXT("x+1 2*y"), true) << endl << endl;

	//COUT << TEXT(">>Diff(TEXT(\"x\"), 2, true)") << endl;
	//COUT << Expr.Diff(TEXT("x"), 2, true) << endl << endl;

	//COUT << TEXT(">>Simplify(true)") << endl;
	//COUT << Expr.Simplify(true) << endl << endl;

	//COUT << TEXT(">>Subs(TEXT(\"x y\"), TEXT(\"1 2\"), true)") << endl;
	//COUT << Expr.Subs(TEXT("x y"), TEXT("1 2"), true) << endl << endl;

	//COUT << TEXT(">>Calc()") << endl;
	//COUT << Expr.Calc() << endl << endl;

	//TExpressionTree Expr2;
	//Expr2.LinkVariableTable(&VarTable);

	//COUT << TEXT(">>Solve(\"7*(tan(x+1)-2)+5=0\"):") << endl;
	//COUT << Expr2.Read(TEXT("7*(tan(x+1)-2)+5"), false);
	//double v;
	//TCHAR *var_x;
	//COUT << TEXT("x = ");
	//COUT << Expr2.Solve(var_x, v);
	//COUT << TEXT(" = ") << v << endl;

	system("pause");

	return 0;
}