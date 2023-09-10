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

	void CalcPhiValue(std::ostream *pOS,const TPEquations &Equations, TVector &PhiValue);
	void MatrixMultiplyVector(TVector &Result, const TMatrix &Matrix, const TVector &Vector);
	void CalcJacobianValue(std::ostream *pOS, TMatrix &JacobianValueResult, const TJacobian &Jacobian);
	int GetMaxAbsRowIndex(const TMatrix &A, int RowStart, int RowEnd, int Col);
	void SwapRow(TMatrix &A, TVector &b, int i, int j);
	bool AllIs0(TVector &V);
	bool VectorAdd(TVector &Va, const TVector &Vb);
	void Output(std::ostream *pOS, TMatrix& m);
	void Output(std::ostream *pOS, TVector& v);
	void ReleaseTPEquations(TPEquations &Equations);
	void ReleaseJacobian(TJacobian &Jacobian);
	void SubsVar(std::ostream *pOS, TPEquations &Equations, TVariableTable &LinkVariableTable, std::string VarStr, double Value);
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
	void BuildEquationsV(std::ostream *pOS);
	void BuildEquationsA_Phitt(std::ostream *pOS);
	void CalcEquationsARight(std::ostream *pOS, TVector &Right);
	void OutputPhi(std::ostream *pOS, TPEquations &Equations);
	void BuildJacobian(std::ostream *pOS);//建立Jacobian
	void CopyJacobian(TJacobian &Result, const TJacobian &Origin);
	void OutputJacobian(std::ostream *pOS, const TJacobian &Jacobian);
	void BuildVariableTableV(std::ostream *pOS);
	void BuildVariableTableA(std::ostream *pOS);
	size_t GetEquationsCount();
	void AddEquation(std::ostream *pOS, std::string szInput, bool istemp);//添加方程
	void RemoveTempEquations();//移除临时方程
	enumError SolveLinear(TMatrix &A, TVector &x, TVector &b);//解线性方程组 系数A，未知数x
	void SolveEquations(std::ostream *pOS);//求解方程组
	void SolveEquationsV(std::ostream *pOS);//求解方程组V
	void SolveEquationsA(std::ostream *pOS);//求解方程组A
	void SimplifyEquations(std::ostream *pOS);//将方程组中的简单方程解出
	void DefineVariable(std::ostream *pOS, const std::string input_str, const std::string input_num = "", bool bIgnoreReDef = false);
	void DefineOneVariable(std::ostream *pOS, std::string var, double value,bool bIgnoreReDef = false);
	void Subs(std::ostream *pOS, const std::string var, double value);
	void Subs(std::ostream *pOS, const std::string subsVars, const std::string subsValues);//代入
	void Subs(std::ostream *pOS,const std::vector<std::string> &subsVars,const std::vector<double> &subsValue);//代入
	void SubsV(std::ostream *pOS, std::string VarStr, double Value);
	void SubsA(std::ostream *pOS, std::string VarStr, double Value);
	double GetValue(const std::string &var);
	void CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector);

};

