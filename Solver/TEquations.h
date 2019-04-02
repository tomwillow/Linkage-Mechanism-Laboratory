#pragma once
#include "..\Solver\TVariableTable.h"
#include "..\Solver\TExpressionTree.h"
#include "..\enumError.h"

class TEquations
{
private:
	typedef std::vector<std::vector<double>> TMatrix;
	typedef std::vector<double> TVector;
	typedef std::vector<TExpressionTree *> TPEquations;
	typedef std::vector<TPEquations> TJacobian;

	//enumError eError;
	std::vector<bool> EquationIsTemp;

	TJacobian Jacobian;
	TPEquations Equations, EquationsV, EquationsA;
	TVariableTable VariableTableSolved;//已解出变量表

	void TEquations::CalcPhiValue(Ostream *pOS,const TPEquations &Equations, TVector &PhiValue);
	void TEquations::MatrixMultiplyVector(TVector &Result, const TMatrix &Matrix, const TVector &Vector);
	void TEquations::CalcJacobianValue(Ostream *pOS, TMatrix &JacobianValueResult, const TJacobian &Jacobian);
	int TEquations::GetMaxAbsRowIndex(const TMatrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(TMatrix &A, TVector &b, int i, int j);
	bool TEquations::AllIs0(TVector &V);
	bool TEquations::VectorAdd(TVector &Va, const TVector &Vb);
	void TEquations::Output(Ostream *pOS, TMatrix& m);
	void TEquations::Output(Ostream *pOS, TVector& v);
	void TEquations::ReleaseTPEquations(TPEquations &Equations);
	void TEquations::ReleaseJacobian(TJacobian &Jacobian);
	void TEquations::SubsVar(Ostream *pOS, TPEquations &Equations, TVariableTable &LinkVariableTable, String VarStr, double Value);
	void TEquations::BuildJacobian_inner(TJacobian &JacobianResult,const TPEquations &Equations,TVariableTable &VariableTable);
public:
	TEquations();
	~TEquations();

	double epsilon = 1e-12;
	unsigned int max_step = 20;
	bool hasSolved;
	TVariableTable VariableTable; //总变量表
	TVariableTable VariableTableUnsolved;
	TVariableTable VariableTableV; //速度总变量表
	TVariableTable VariableTableA; //速度总变量表

	TExpressionTree * TEquations::GetLastExpressionTree()
	{
		return Equations.back();
	}
	void TEquations::BuildEquationsV(Ostream *pOS);
	void TEquations::BuildEquationsA_Phitt(Ostream *pOS);
	void TEquations::CalcEquationsARight(Ostream *pOS, TVector &Right);
	void TEquations::OutputPhi(Ostream *pOS, TPEquations &Equations);
	void TEquations::BuildJacobian(Ostream *pOS);//建立Jacobian
	void TEquations::CopyJacobian(TJacobian &Result, const TJacobian &Origin);
	void TEquations::OutputJacobian(Ostream *pOS, const TJacobian &Jacobian);
	void TEquations::BuildVariableTableV(Ostream *pOS);
	void TEquations::BuildVariableTableA(Ostream *pOS);
	size_t TEquations::GetEquationsCount();
	void TEquations::AddEquation(Ostream *pOS, String szInput, bool istemp);//添加方程
	void TEquations::RemoveTempEquations();//移除临时方程
	enumError TEquations::SolveLinear(TMatrix &A, TVector &x, TVector &b);//解线性方程组 系数A，未知数x
	void TEquations::SolveEquations(Ostream *pOS);//求解方程组
	void TEquations::SolveEquationsV(Ostream *pOS);//求解方程组V
	void TEquations::SolveEquationsA(Ostream *pOS);//求解方程组A
	void TEquations::SimplifyEquations(Ostream *pOS);//将方程组中的简单方程解出
	void TEquations::DefineVariable(Ostream *pOS, const String input_str, const String input_num = TEXT(""), bool bIgnoreReDef = false);
	void TEquations::DefineOneVariable(Ostream *pOS, String var, double value,bool bIgnoreReDef = false);
	void TEquations::Subs(Ostream *pOS, const String var, double value);
	void TEquations::Subs(Ostream *pOS, const String subsVars, const String subsValues);//代入
	void TEquations::Subs(Ostream *pOS,const std::vector<String> &subsVars,const std::vector<double> &subsValue);//代入
	void TEquations::SubsV(Ostream *pOS, String VarStr, double Value);
	void TEquations::SubsA(Ostream *pOS, String VarStr, double Value);
	void TEquations::CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector);

};

