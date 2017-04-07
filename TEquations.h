#pragma once
#include "TVariableTable.h"
#include "TExpressionTree.h"
#include "enumError.h"

class TEquations
{
private:
	const double epsilon = 1e-6;

	enumError eError;
	typedef std::vector<std::vector<double>> Matrix;
	typedef std::vector<double> Vector;
	std::vector<std::vector<TExpressionTree *>> Jacobian;
	std::vector<TExpressionTree *> Equations,EquationsSolved;
	std::vector<bool> EquationIsTemp;
	void TEquations::CalcPhiValue(String *pStr, Vector &PhiValue, const Vector &Q);
	void TEquations::CalcJacobianValue(String *pStr, Matrix &JacobianValue, const Vector &Q);
	int TEquations::GetMaxAbsRowIndex(const Matrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(Matrix &A, Vector &b, int i, int j);
	bool TEquations::AllIs0(Vector &V);
	bool TEquations::VectorAdd(Vector &Va, const Vector &Vb);
	void TEquations::Output(String *pStr, Matrix& m);
		void TEquations::Output(String *pStr, Vector& v);
	void TEquations::BuildJacobian(String *pStr, TCHAR *subsVar=NULL, TCHAR *subsValue=NULL);//建立Jacobian
public:
	TEquations();
	~TEquations();
	bool hasSolved;
	TVariableTable VariableTable; //总变量表
	TVariableTable VariableTableSolved;//已解出变量表
	TVariableTable VariableTableUnsolved;
	TExpressionTree * TEquations::GetLastExpressionTree()
	{
		return Equations.back();
	}
	size_t TEquations::GetEquationsCount();
	void TEquations::AddEquation(String *pStr, TCHAR *szInput, bool istemp);//添加方程
	void TEquations::RemoveTempEquations();//移除临时方程
	enumError TEquations::SolveLinear(Matrix &A, Vector &x, Vector &b);//解线性方程组 系数A，未知数x
	void TEquations::SolveEquations(String *pStr);//求解方程组
	void TEquations::SimplifyEquations(String *pStr);//将方程组中的简单方程解出
	void TEquations::DefineVariable(String *pStr, TCHAR *input_str, TCHAR *input_num);
	void TEquations::Subs(String *pStr, TCHAR *subsVar, TCHAR *subsValue);//代入

};

