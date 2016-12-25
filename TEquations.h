#pragma once
#include <string>
#include "TVariableTable.h"
#include "TExpressionTree.h"
#include "enumError.h"

class TEquations
{
private:
	const double epsilon = 1e-6;

#ifdef _UNICODE
#define String wstring
#else
#define String string
#endif
	std::String Str;
	TCHAR *szStr;
	typedef std::vector<std::vector<double>> Matrix;
	typedef std::vector<double> Vector;
	std::vector<TExpressionTree *> Equations;
	void TEquations::CalcPhiValue(Vector &PhiValue, const Vector &Q);
	void TEquations::CalcJacobiValue(Matrix &JacobiValue,const Vector &Q);
	int TEquations::GetMaxAbsRowIndex(const Matrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(Matrix &A, Vector &b, int i, int j);
	bool TEquations::AllIs0(Vector &V);
	bool TEquations::VectorAdd(Vector &Va, const Vector &Vb);
	void TEquations::Output(Matrix& m);
	void TEquations::Output(Vector& v);
public:
	TEquations();
	~TEquations();
	TVariableTable VariableTable; 
	std::vector<std::vector<TExpressionTree *>> Jacobi;
	TCHAR * TEquations::AddEquation(TCHAR *input, bool output);
	const TCHAR * TEquations::BuildJacobi(bool bOutput, TCHAR *subsVar, TCHAR *subsValue);
	bool TEquations::SolveLinear(Matrix &A, Vector &x, Vector &b);
	const TCHAR * TEquations::Solve(bool bOutput);
};

