#pragma once
#include "TVariableTable.h"
#include "TExpressionTree.h"
#include "enumError.h"

class TEquations
{
private:
	const double epsilon = 1e-6;

	enumError eError;
	typedef std::vector<std::vector<double>> TMatrix;
	typedef std::vector<double> TVector;
	typedef std::vector<TExpressionTree *> TPEquations;
	typedef std::vector<TPEquations> TJacobian;
	TJacobian Jacobian;
	TJacobian JacobianV;
	std::vector<bool> EquationIsTemp;
	void TEquations::CalcPhiValue(String *pStr, TPEquations Equations, TVector &PhiValue);
	void TEquations::CalcJacobianValue(String *pStr, TMatrix &JacobianValue);
	int TEquations::GetMaxAbsRowIndex(const TMatrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(TMatrix &A, TVector &b, int i, int j);
	bool TEquations::AllIs0(TVector &V);
	bool TEquations::VectorAdd(TVector &Va, const TVector &Vb);
	void TEquations::Output(String *pStr, TMatrix& m);
	void TEquations::Output(String *pStr, TVector& v);
	void TEquations::RemoveTPEquations(TPEquations &Equations);
	void TEquations::RemoveJacobian(TJacobian &Jacobian);
	void TEquations::SubsVar(String *pStr, TPEquations &Equations, TVariableTable &LinkVariableTable, TCHAR *VarStr, double Value);
public:
	TEquations();
	~TEquations();
	bool hasSolved;
	TPEquations Equations, EquationsV;
	TVariableTable VariableTable; //总变量表
	TVariableTable VariableTableV; //总变量表
	TVariableTable VariableTableSolved;//已解出变量表
	TVariableTable VariableTableUnsolved;
	TExpressionTree * TEquations::GetLastExpressionTree()
	{
		return Equations.back();
	}
	void TEquations::BuildEquationsV(String *pStr);
	void TEquations::BuildJacobian(String *pStr);//建立Jacobian
	void TEquations::BuildJacobianV(String *pStr);//建立JacobianV
	void TEquations::BuildVariableTableV(String *pStr);
	size_t TEquations::GetEquationsCount();
	void TEquations::AddEquation(String *pStr, TCHAR *szInput, bool istemp);//添加方程
	void TEquations::RemoveTempEquations();//移除临时方程
	enumError TEquations::SolveLinear(TMatrix &A, TVector &x, TVector &b);//解线性方程组 系数A，未知数x
	void TEquations::SolveEquations(String *pStr);//求解方程组
	void TEquations::SolveEquationsV(String *pStr);//求解方程组V
	void TEquations::SimplifyEquations(String *pStr);//将方程组中的简单方程解出
	void TEquations::DefineVariable(String *pStr, TCHAR *input_str, TCHAR *input_num);
	void TEquations::Subs(String *pStr, TCHAR *subsVar, TCHAR *subsValue);//代入
	void TEquations::SubsV(String *pStr, TCHAR *VarStr, double Value);

};

