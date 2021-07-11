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
	TVariableTable VariableTableSolved;//�ѽ��������

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
	TVariableTable VariableTable; //�ܱ�����
	TVariableTable VariableTableUnsolved;
	TVariableTable VariableTableV; //�ٶ��ܱ�����
	TVariableTable VariableTableA; //�ٶ��ܱ�����

	TExpressionTree * TEquations::GetLastExpressionTree()
	{
		return Equations.back();
	}
	void TEquations::BuildEquationsV(Ostream *pOS);
	void TEquations::BuildEquationsA_Phitt(Ostream *pOS);
	void TEquations::CalcEquationsARight(Ostream *pOS, TVector &Right);
	void TEquations::OutputPhi(Ostream *pOS, TPEquations &Equations);
	void TEquations::BuildJacobian(Ostream *pOS);//����Jacobian
	void TEquations::CopyJacobian(TJacobian &Result, const TJacobian &Origin);
	void TEquations::OutputJacobian(Ostream *pOS, const TJacobian &Jacobian);
	void TEquations::BuildVariableTableV(Ostream *pOS);
	void TEquations::BuildVariableTableA(Ostream *pOS);
	size_t TEquations::GetEquationsCount();
	void TEquations::AddEquation(Ostream *pOS, String szInput, bool istemp);//��ӷ���
	void TEquations::RemoveTempEquations();//�Ƴ���ʱ����
	enumError TEquations::SolveLinear(TMatrix &A, TVector &x, TVector &b);//�����Է����� ϵ��A��δ֪��x
	void TEquations::SolveEquations(Ostream *pOS);//��ⷽ����
	void TEquations::SolveEquationsV(Ostream *pOS);//��ⷽ����V
	void TEquations::SolveEquationsA(Ostream *pOS);//��ⷽ����A
	void TEquations::SimplifyEquations(Ostream *pOS);//���������еļ򵥷��̽��
	void TEquations::DefineVariable(Ostream *pOS, const String input_str, const String input_num = TEXT(""), bool bIgnoreReDef = false);
	void TEquations::DefineOneVariable(Ostream *pOS, String var, double value,bool bIgnoreReDef = false);
	void TEquations::Subs(Ostream *pOS, const String var, double value);
	void TEquations::Subs(Ostream *pOS, const String subsVars, const String subsValues);//����
	void TEquations::Subs(Ostream *pOS,const std::vector<String> &subsVars,const std::vector<double> &subsValue);//����
	void TEquations::SubsV(Ostream *pOS, String VarStr, double Value);
	void TEquations::SubsA(Ostream *pOS, String VarStr, double Value);
	double TEquations::GetValue(const String &var);
	void TEquations::CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector);

};

