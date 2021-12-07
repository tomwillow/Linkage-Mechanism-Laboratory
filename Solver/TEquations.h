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

	void CalcPhiValue(Ostream *pOS,const TPEquations &Equations, TVector &PhiValue);
	void MatrixMultiplyVector(TVector &Result, const TMatrix &Matrix, const TVector &Vector);
	void CalcJacobianValue(Ostream *pOS, TMatrix &JacobianValueResult, const TJacobian &Jacobian);
	int GetMaxAbsRowIndex(const TMatrix &A, int RowStart, int RowEnd, int Col);
	void SwapRow(TMatrix &A, TVector &b, int i, int j);
	bool AllIs0(TVector &V);
	bool VectorAdd(TVector &Va, const TVector &Vb);
	void Output(Ostream *pOS, TMatrix& m);
	void Output(Ostream *pOS, TVector& v);
	void ReleaseTPEquations(TPEquations &Equations);
	void ReleaseJacobian(TJacobian &Jacobian);
	void SubsVar(Ostream *pOS, TPEquations &Equations, TVariableTable &LinkVariableTable, String VarStr, double Value);
	void BuildJacobian_inner(TJacobian &JacobianResult,const TPEquations &Equations,TVariableTable &VariableTable);
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

	TExpressionTree * GetLastExpressionTree()
	{
		return Equations.back();
	}
	void BuildEquationsV(Ostream *pOS);
	void BuildEquationsA_Phitt(Ostream *pOS);
	void CalcEquationsARight(Ostream *pOS, TVector &Right);
	void OutputPhi(Ostream *pOS, TPEquations &Equations);
	void BuildJacobian(Ostream *pOS);//建立Jacobian
	void CopyJacobian(TJacobian &Result, const TJacobian &Origin);
	void OutputJacobian(Ostream *pOS, const TJacobian &Jacobian);
	void BuildVariableTableV(Ostream *pOS);
	void BuildVariableTableA(Ostream *pOS);
	size_t GetEquationsCount();
	void AddEquation(Ostream *pOS, String szInput, bool istemp);//添加方程
	void RemoveTempEquations();//移除临时方程
	enumError SolveLinear(TMatrix &A, TVector &x, TVector &b);//解线性方程组 系数A，未知数x
	void SolveEquations(Ostream *pOS);//求解方程组
	void SolveEquationsV(Ostream *pOS);//求解方程组V
	void SolveEquationsA(Ostream *pOS);//求解方程组A
	void SimplifyEquations(Ostream *pOS);//将方程组中的简单方程解出
	void DefineVariable(Ostream *pOS, const String input_str, const String input_num = TEXT(""), bool bIgnoreReDef = false);
	void DefineOneVariable(Ostream *pOS, String var, double value,bool bIgnoreReDef = false);
	void Subs(Ostream *pOS, const String var, double value);
	void Subs(Ostream *pOS, const String subsVars, const String subsValues);//代入
	void Subs(Ostream *pOS,const std::vector<String> &subsVars,const std::vector<double> &subsValue);//代入
	void SubsV(Ostream *pOS, String VarStr, double Value);
	void SubsA(Ostream *pOS, String VarStr, double Value);
	double GetValue(const String &var);
	void CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector);

};

