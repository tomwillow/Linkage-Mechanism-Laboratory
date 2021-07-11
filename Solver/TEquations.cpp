#pragma once
#include "..\Common\DetectMemoryLeak.h"

#include <Windows.h>
#include "..\Common\String.h"
#include "TEquations.h"

TEquations::TEquations()
{
	VariableTableUnsolved.bShared = true;
	VariableTableSolved.bShared = true;
}


TEquations::~TEquations()
{
	//�ͷŷ�����
	ReleaseTPEquations(Equations);
	ReleaseTPEquations(EquationsV);
	ReleaseTPEquations(EquationsA);

	//�ͷ��ſɱ�
	ReleaseJacobian(Jacobian);
}

void TEquations::ReleaseTPEquations(TPEquations &Equations)
{
	for (auto pEqua : Equations)
		delete pEqua;
	Equations.clear();
}

void TEquations::RemoveTempEquations()
{
	int i;
	i = EquationIsTemp.size() - 1;
	for (; i > -1; i--)
	{
		if (EquationIsTemp[i] == true)
		{
			std::vector<bool>::iterator iter1 = EquationIsTemp.begin() + i;
			std::vector<TExpressionTree *>::iterator iter2 = Equations.begin() + i;
			EquationIsTemp.erase(iter1);

			delete Equations[i];
			Equations.erase(iter2);
		}
	}

	ReleaseTPEquations(EquationsV);
	ReleaseTPEquations(EquationsA);

	//VariableTableUnsolved = VariableTable;
	//VariableTableUnsolved.bShared = true;

}

size_t TEquations::GetEquationsCount()
{
	return Equations.size();
}

void TEquations::DefineOneVariable(Ostream *pOS, String var, double value, bool bIgnoreReDef)
{
	VariableTable.DefineOne(pOS, var, value,bIgnoreReDef);

	VariableTableUnsolved.VariableTable = VariableTable.VariableTable;
	VariableTableUnsolved.VariableValue = VariableTable.VariableValue;

}

void TEquations::DefineVariable(Ostream *pOS, const String input_str, const String input_num, bool bIgnoreReDef)
{
	VariableTable.Define(pOS, input_str, input_num,bIgnoreReDef);

	VariableTableUnsolved.VariableTable = VariableTable.VariableTable;
	VariableTableUnsolved.VariableValue = VariableTable.VariableValue;
}

void TEquations::AddEquation(Ostream *pOS, String szInput, bool istemp)
{
	TExpressionTree *temp = new TExpressionTree;
	temp->LinkVariableTable(&VariableTable);
	temp->Read(szInput, false);

	temp->Simplify(false);

	//���뷽����
	Equations.push_back(temp);
	EquationIsTemp.push_back(istemp);

	hasSolved = false;

	if (pOS != NULL)
	{
		*pOS << TEXT(">>Add:\r\n");
		*pOS << temp->OutputStr();
		*pOS << TEXT("\r\n\r\n");
	}
}

//������̶�t�󵼣��õ��ٶȷ������ұ�
void TEquations::BuildEquationsV(Ostream *pOS)
{
	bool bOutput = pOS == NULL ? false : true;

	if (pOS != NULL)
	{
		*pOS << TEXT(">>BuildEquationsV: \r\n");
		*pOS << TEXT("��ǰ���̣�\r\n");
	}

	TExpressionTree *pEquatemp;
	for (auto pEqua : Equations)
	{
		pEquatemp = new TExpressionTree;
		*pEquatemp = *pEqua;

		pEquatemp->Diff(TEXT("t"), 1, bOutput);
		pEquatemp->Simplify(bOutput);

		EquationsV.push_back(pEquatemp);

		if (pOS != NULL)
		{
			*pOS << pEquatemp->OutputStr();
			*pOS << TEXT("\r\n");
		}
	}
}

//������̶�t�󵼣��õ��ٶȷ������ұ�
void TEquations::BuildEquationsA_Phitt(Ostream *pOS)
{
	bool bOutput = pOS == NULL ? false : true;

	if (pOS != NULL)
	{
		*pOS << TEXT(">>Build Equations A: \r\n");
		*pOS << TEXT("��ǰ���̣�\r\n");
	}

	TExpressionTree *pEquatemp;
	for (auto pEqua : EquationsV)
	{
		pEquatemp = new TExpressionTree;
		*pEquatemp = *pEqua;

		pEquatemp->Diff(TEXT("t"), 1, bOutput);
		pEquatemp->Simplify(bOutput);

		EquationsA.push_back(pEquatemp);

		if (pOS != NULL)
		{
			*pOS << pEquatemp->OutputStr();
			*pOS << TEXT("\r\n");
		}
	}
}

//JacobianΪ���ž��󣬳���Vector��ֵ�������õ����ŷ����������������EquationsResult
//Ax=b
void TEquations::CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector)
{
	TExpressionTree *expr;
	for (auto &Line : Jacobian)//ÿ��
	{
		expr = new TExpressionTree;
		for (auto iter = Line.begin(); iter != Line.end(); ++iter)//ÿ��expr
		{
			//Jacobianÿ�����q'
			(**iter)*Vector[iter - Line.begin()];
			//(*iter)->Simplify(false);

			//������
			*expr + (**iter);
		}
#ifdef _DEBUG
		expr->OutputStr();
#endif
		//expr->Simplify(false);
#ifdef _DEBUG
		expr->OutputStr();
#endif
		EquationsResult.push_back(expr);
	}
}

void TEquations::MatrixMultiplyVector(TVector &Result, const TMatrix &Matrix, const TVector &Vector)
{
	Result.clear();
	double temp;
	for (auto Row : Matrix)
	{
		temp = 0;
		for (auto iter = Row.begin(); iter != Row.end(); ++iter)
		{
			temp += (*iter)*Vector[iter - Row.begin()];
		}
		Result.push_back(temp);
	}
}


//Ӧ�ڽ��λ�á��ٶȷ��̺����
void TEquations::CalcEquationsARight(Ostream *pOS, TVector &Right)
{
	//����Jacobian����
	TJacobian JacobianTemp;
	CopyJacobian(JacobianTemp, Jacobian);
#ifdef _DEBUG
	OutputJacobian(pOS, JacobianTemp);
#endif

	//Jacobian*q' ����q'
	TPEquations EquationsTemp;
	CalcJacobianMultiplyVector(EquationsTemp, JacobianTemp, VariableTableV.VariableValue);
#ifdef _DEBUG
	OutputPhi(pOS, EquationsTemp);
#endif

	//(Jacobian*q')q  ��q��
	BuildJacobian_inner(JacobianTemp, EquationsTemp, VariableTableA);
#ifdef _DEBUG
	OutputJacobian(pOS, JacobianTemp);
#endif

	//Vpa: (Jacobian*q')q
	TMatrix MatrixTemp;
	CalcJacobianValue(pOS, MatrixTemp, JacobianTemp);
#ifdef _DEBUG
	Output(pOS, MatrixTemp);
#endif

	// *q'
	MatrixMultiplyVector(Right, MatrixTemp, VariableTableV.VariableValue);
#ifdef _DEBUG
	Output(pOS, Right);
#endif

	//-Phitt
	TVector Phitt;
	CalcPhiValue(pOS, EquationsA, Phitt);
#ifdef _DEBUG
	Output(pOS, Phitt);
#endif

	//-Right-Phitt
	for (auto &iter = Right.begin(); iter != Right.end(); ++iter)
		(*iter) = -(*iter) + Phitt[iter - Right.begin()];

#ifdef _DEBUG
	Output(pOS, Right);
#endif

	ReleaseJacobian(JacobianTemp);
	ReleaseTPEquations(EquationsTemp);
}

//�滻��һ����
void TEquations::SubsV(Ostream *pOS, String VarStr, double Value)
{
	SubsVar(pOS, EquationsV, VariableTable, VarStr, Value);
}

//�滻��һ����
void TEquations::SubsA(Ostream *pOS, String VarStr, double Value)
{
	SubsVar(pOS, EquationsA, VariableTable, VarStr, Value);
}

void TEquations::SubsVar(Ostream *pOS, TPEquations &Equations, TVariableTable &LinkVariableTable, String VarStr, double Value)
{
	auto it = LinkVariableTable.FindVariableTable(VarStr);
	if (it != LinkVariableTable.VariableTable.end())
	{
		String Var;
		Var = *it;
		for (auto pEquation : Equations)
		{
			pEquation->Subs(Var, Value, false);
		}
	}
}

void TEquations::Subs(Ostream *pOS, const String var, double value)
{
	if (var.empty())
		throw TError{ ERROR_EMPTY_INPUT, TEXT("") };

	//Table�д���
	auto find1 = VariableTable.FindVariableTable(var) != VariableTable.VariableTable.end();

	//�ѽ���д���
	auto find2 = VariableTableSolved.FindVariableTable(var) != VariableTableSolved.VariableTable.end();
	if (find1 && find2 == false)
	{
		VariableTableSolved.VariableTable.push_back(var);
		VariableTableSolved.VariableValue.push_back(value);
	}
	//else
	//	throw TError{ ERROR_UNDEFINED_VARIABLE, var };


	if (pOS != NULL)
	{
		*pOS << TEXT(">>Subs: [") << var;
		*pOS << TEXT("] -> [");
		*pOS << value;
		*pOS << TEXT("]\r\n\r\n��ǰ���̣�\r\n");
	}
	for (auto pExpr : Equations)//��������
	{
		pExpr->LinkVariableTable(&VariableTableUnsolved);

		//�滻
		pExpr->Subs(var, value, false);

		if (pOS != NULL)
		{
			*pOS << pExpr->OutputStr();
			*pOS << TEXT("\r\n");
		}
	}

	if (pOS != NULL)
	{
		*pOS << TEXT("\r\n");
	}

	//�޳������滻���ı���
	VariableTableUnsolved.RemoveOne(pOS, var,true);
}


void TEquations::Subs(Ostream *pOS, const std::vector<String> &subsVars, const std::vector<double> &subsValue)
{
	if (subsVars.size() != subsValue.size())
		throw TError{ ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT,TEXT("") };

	for (int i = 0; i < subsVars.size(); ++i)
	{
		Subs(pOS, subsVars[i], subsValue[i]);
	}
}

//�ѽ����������� δ����������޳�
void TEquations::Subs(Ostream *pOS, const String subsVars, const String subsValues)//����
{
	std::vector<String> tempVars = StrSliceToVector(subsVars);
	std::vector<double> tempValues = StrSliceToDoubleVector(subsValues);

	Subs(pOS, tempVars, tempValues);
}

//��δ���������ֵ���ٶȱ�����
void TEquations::BuildVariableTableV(Ostream *pOS)
{
	VariableTableV = VariableTableUnsolved;
	VariableTableV.bShared = true;
}

//��δ���������ֵ���ٶȱ�����
void TEquations::BuildVariableTableA(Ostream *pOS)
{
	VariableTableA = VariableTableUnsolved;
	VariableTableA.bShared = true;
}

//Copy Jacobian
void TEquations::CopyJacobian(TJacobian &ResultJacobian, const TJacobian &OriginJacobian)
{
	ReleaseJacobian(ResultJacobian);

	ResultJacobian.resize(OriginJacobian.size());
	TExpressionTree *temp;
	for (size_t i = 0; i < OriginJacobian.size(); i++)
		for (size_t j = 0; j < OriginJacobian[i].size(); j++)
		{
			temp = new TExpressionTree;
			*temp = *OriginJacobian[i][j];

			ResultJacobian[i].push_back(temp);
		}
}


void TEquations::ReleaseJacobian(TJacobian &Jacobian)
{
	for (size_t i = 0; i < Jacobian.size(); i++)
		for (size_t j = 0; j < Jacobian[i].size(); j++)
			delete Jacobian[i][j];

	Jacobian.clear();
}

void TEquations::BuildJacobian_inner(TJacobian &JacobianResult, const TPEquations &Equations, TVariableTable &VariableTable)
{
	//�ͷžɵ��ſɱ�
	ReleaseJacobian(JacobianResult);

	TExpressionTree *temp;

	//�����ſɱȾ���
	JacobianResult.resize(Equations.size());
	for (size_t i = 0; i < Equations.size(); i++)//��������
	{
		//��δ������������ſɱȾ���
		Equations[i]->LinkVariableTable(&VariableTable);

		//Equations[i]->Simplify(false);
		for (size_t j = 0; j < VariableTable.VariableTable.size(); j++)
		{
			temp = new TExpressionTree;
			*temp = *Equations[i];
			temp->Diff(VariableTable.VariableTable[j], 1, false);
			temp->Simplify(false);
			JacobianResult[i].push_back(temp);
		}
	}
}

//����VariableTableUnsolved
void TEquations::BuildJacobian(Ostream *pOS)
{
	BuildJacobian_inner(Jacobian, Equations, VariableTableUnsolved);

	//�����
	if (pOS != NULL)
	{
		*pOS << TEXT(">>Build Jacobian:\r\n\r\n");

		OutputPhi(pOS, Equations);

		OutputJacobian(pOS, Jacobian);
	}
}

void TEquations::OutputPhi(Ostream *pOS, TPEquations &Equations)
{
	if (pOS != NULL)
	{
		*pOS << TEXT("Phi(1x");
		*pOS << Equations.size();
		*pOS << TEXT(")=\r\n[");
		for (auto iter = Equations.begin(); iter != Equations.end(); ++iter)
		{
			*pOS << (*iter)->OutputStr();
			if (iter != Equations.end() - 1)
				*pOS << TEXT(";\r\n");
		}
		*pOS << TEXT("]\r\n\r\n");
	}
}

void TEquations::OutputJacobian(Ostream *pOS, const TJacobian &Jacobian)
{
	//�����
	if (pOS != NULL)
	{
		*pOS << TEXT("Jacobian(");
		*pOS << (Jacobian.size() > 0 ? Jacobian[0].size() : 1) << TEXT("x") << Jacobian.size();
		*pOS << TEXT(")=\r\n[");
		for (size_t ii = 0; ii < Jacobian.size(); ii++)
		{
			for (size_t jj = 0; jj < Jacobian[ii].size(); jj++)
			{
				*pOS << Jacobian[ii][jj]->OutputStr();
				*pOS << TEXT(" ");
			}
			if (ii != Jacobian.size() - 1)
				*pOS << TEXT(";\r\n");
		}
		*pOS << TEXT("]\r\n\r\n");
	}
}

void TEquations::Output(Ostream *pOS, TMatrix& m)
{
	if (pOS != NULL)
	{
		static TCHAR temp[64];
		*pOS << TEXT("[");
		for (size_t i = 0; i < m.size(); i++)
		{
			for (size_t j = 0; j < m[i].size(); j++)
			{
				_stprintf(temp, TEXT("%f"), m[i][j]);
				*pOS << temp;
				*pOS << TEXT(" ");
			}
			if (i != m.size() - 1)
				*pOS << TEXT(";\r\n");
		}
		*pOS << TEXT("]\r\n\r\n");
	}
}

void TEquations::Output(Ostream *pOS, TVector& v)
{
	if (pOS != NULL)
	{
		static TCHAR temp[64];
		*pOS << TEXT("[");
		for (auto value : v)
		{
			_stprintf(temp, TEXT("%f"), value);
			*pOS << temp;
			*pOS << TEXT(" ");
		}
		*pOS << TEXT("]\r\n\r\n");
	}
}

//���ñ������е�ֵ�����ſɱ�
void TEquations::CalcJacobianValue(Ostream *pOS, TMatrix &JacobianValueResult, const TJacobian &Jacobian)
{
	JacobianValueResult.clear();
	JacobianValueResult.resize(Jacobian.size());
	TExpressionTree *temp;
	for (size_t i = 0; i < Jacobian.size(); i++)
	{
		for (auto exprJacobian : Jacobian[i])
		{
			temp = new TExpressionTree;
			*temp = *exprJacobian;
			try
			{
				temp->Vpa(false);
				JacobianValueResult[i].push_back(temp->Value(true));//�õ���ʱ���ʽֵ�����ſɱ�
			}
			catch (TError err)
			{
				if (pOS != NULL)
				{
					*pOS << TEXT("ERROR:");
					*pOS << temp->OutputStr();
					*pOS << TEXT("\r\nJacobian�������:");
					*pOS << GetErrorInfo(err.id) + err.info;
				}
				delete temp;
				throw err;
			}
			delete temp;
		}
	}
}

//���ñ������е�ֵ���㣬�����в�ǰ׺���ţ������ֵ�Ӹ���
void TEquations::CalcPhiValue(Ostream *pOS, const TPEquations &Equations, TVector &PhiValue)
{
	PhiValue.clear();
	TExpressionTree *temp;
	for (auto PhiExpr : Equations)
	{
		temp = new TExpressionTree;
		*temp = *PhiExpr;
		try
		{
			temp->Vpa(false);
			PhiValue.push_back(-temp->Value(true));//�õ���ʱ���ʽֵ����
		}
		catch (TError & err)
		{
			if (pOS != NULL)
			{
				*pOS << TEXT("ERROR:");
				*pOS << temp->OutputStr();
				*pOS << TEXT("\r\nPhi�������:");
				*pOS << GetErrorInfo(err.id) + err.info;
			}
			delete temp;
			throw err;
		}
		delete temp;
	}
}

int TEquations::GetMaxAbsRowIndex(const TMatrix &A, int RowStart, int RowEnd, int Col)
{
	double max = 0.0;
	int index = RowStart;
	for (int i = RowStart; i <= RowEnd; i++)
	{
		if (abs(A[i][Col]) > max)
		{
			max = abs(A[i][Col]);
			index = i;
		}
	}
	return index;
}

void TEquations::SwapRow(TMatrix &A, TVector &b, int i, int j)
{
	if (i == j)
		return;
	TVector temp(A[i].size());
	temp = A[i];
	A[i] = A[j];
	A[j] = temp;

	double n;
	n = b[i];
	b[i] = b[j];
	b[j] = n;
}

enumError TEquations::SolveLinear(TMatrix &A, TVector &x, TVector &b)
{
	auto m = A.size();//����
	auto n = m;//����=δ֪������

	auto RankA = m, RankAb = m;//��ʼֵ

	if (x.size() != m) x.resize(m);//���Է������

	if (m != b.size())//Jacobian����������Phi����
		return ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW;

	if (m > 0)
		if ((n = A[0].size()) != m)//���Ƿ���
		{
			if (m > n)
				return ERROR_OVER_DETERMINED_EQUATIONS;//�����巽����
			else//����������
				x.resize(n);
		}

	std::vector<decltype(m)> TrueRowNumber(n);

	//����Ԫ��Ԫ��
	for (decltype(m) y = 0, x = 0; y < m && x < n; y++, x++)
	{
		//if (A[i].size() != m)

		//�ӵ�ǰ��(y)�����һ��(m-1)�У��ҳ�x������һ����y�н���
		SwapRow(A, b, y, GetMaxAbsRowIndex(A, y, m - 1, x));

		while (abs(A[y][x]) < epsilon)//�����ǰֵΪ0  xһֱ��������0
		{
			x++;
			if (x == n)
				break;

			//���������������
			SwapRow(A, b, y, GetMaxAbsRowIndex(A, y, m - 1, x));
		}

		if (x != n && x > y)
		{
			TrueRowNumber[y] = x;//���뷽��ʱ ��ǰ��Ӧ����x��
		}

		if (x == n)//����ȫΪ0
		{
			RankA = y;
			if (abs(b[y]) < epsilon)
				RankAb = y;

			if (RankA != RankAb)//���죬��ϵ��������������Ȳ����->�޽�
				return ERROR_SINGULAR_MATRIX;
			else
				break;//����for���õ��ؽ�
		}

		//���Խ��߻�Ϊ1
		double m_num = A[y][x];
		for (decltype(m) j = y; j < n; j++)//y�е�j��->��n��
			A[y][j] /= m_num;
		b[y] /= m_num;

		//ÿ�л�Ϊ0
		for (decltype(m) row = y + 1; row < m; row++)//��1��->���1��
		{
			if (abs(A[row][x]) < epsilon)
				;
			else
			{
				double mi = A[row][x];
				for (auto col = x; col < n; col++)//row�е�x��->��n��
				{
					A[row][col] -= A[y][col] * mi;
				}
				b[row] -= b[y] * mi;
			}
		}
	}

	bool bIndeterminateEquation = false;//���ô˱�������Ϊ����m��=n��������ж��Ƿ�Ϊ����������

	//��Ϊ���������飬��ȱ��ȫ��0��������
	if (m != n)
	{
		A.resize(n);//A��Ϊn��
		for (auto i = m; i < n; i++)//A��m�п�ʼÿ��n����
			A[i].resize(n);
		b.resize(n);
		m = n;
		bIndeterminateEquation = true;

		//����˳��
		for (int i = m - 1; i >= 0; i--)
		{
			if (TrueRowNumber[i] != 0)
			{
				SwapRow(A, b, i, TrueRowNumber[i]);
			}
		}
	}

	//���û��õ�x
	double sum_others = 0.0;
	for (int i = m - 1; i >= 0; i--)//���1��->��1��
	{
		sum_others = 0.0;
		for (decltype(m) j = i + 1; j < m; j++)//���� ���Ԫ�س�����֪x ����
		{
			sum_others += A[i][j] * x[j];
		}
		x[i] = b[i] - sum_others;
	}

	if (RankA < n && RankA == RankAb)
	{
		if (bIndeterminateEquation)
			return ERROR_INDETERMINATE_EQUATION;
		else
			return ERROR_INFINITY_SOLUTIONS;
	}

	return ERROR_NO;
}

bool TEquations::AllIs0(TVector &V)
{
	for (auto value : V)
	{
		if (abs(value) >= epsilon)
			return false;
	}
	return true;
}

bool TEquations::VectorAdd(TVector &Va, const TVector &Vb)
{
	if (Va.size() != Vb.size())
		return false;
	for (size_t i = 0; i < Va.size(); i++)
	{
		Va[i] += Vb[i];
	}
	return true;
}


void TEquations::SolveEquationsV(Ostream *pOS)//��ⷽ����V
{
	TMatrix JacobianV;
	TVector Phi;
	TVector &dQ = VariableTableV.VariableValue;
	CalcPhiValue(pOS, EquationsV, Phi);
	CalcJacobianValue(pOS, JacobianV, Jacobian);
	SolveLinear(JacobianV, dQ, Phi);

	if (pOS != NULL)
	{
		*pOS << TEXT(">>SolveEquationsV:\r\n");
		if (pOS != NULL) *pOS << TEXT("\r\n�õ������\r\n");
		VariableTableV.OutputValue(pOS);
	}
}

void TEquations::SolveEquationsA(Ostream *pOS)//��ⷽ����A
{
	TMatrix JacobianA;
	TVector Phi;
	TVector &ddQ = VariableTableA.VariableValue;
	CalcJacobianValue(pOS, JacobianA, Jacobian);//JacobianA��Jacobian���
	CalcEquationsARight(pOS, Phi);
	SolveLinear(JacobianA, ddQ, Phi);

	if (pOS != NULL)
	{
		*pOS << TEXT(">>SolveEquationsA:\r\n");
		if (pOS != NULL) *pOS << TEXT("\r\n�õ������\r\n");
		VariableTableA.OutputValue(pOS);
	}
}

//ţ��-����ɭ�������
void TEquations::SolveEquations(Ostream *pOS)
{
	if (hasSolved == false)
	{
		if (pOS != NULL)
		{
			*pOS << TEXT(">>SolveEquations:\r\n");
			*pOS << TEXT("��ǰδ֪����\r\n");
		}
		VariableTableUnsolved.Output(pOS);//�����ǰ����

		TMatrix JacobianValue;
		TVector PhiValue, DeltaQ, &Q = VariableTableUnsolved.VariableValue;
		TVector VariableValueBackup = VariableTableUnsolved.VariableValue;
		unsigned int n = 0;

		while (1)
		{
			if (pOS != NULL)
			{
				*pOS << TEXT("q(");
				*pOS << n;
				*pOS << TEXT(")=\r\n");
				Output(pOS, Q);
				*pOS << TEXT("\r\n");
			}

			try
			{
				CalcJacobianValue(pOS, JacobianValue, Jacobian);
			}
			catch (TError& err)
			{
				if (pOS != NULL)
				{
					*pOS << TEXT("�޷����㡣\r\n");
					*pOS << GetErrorInfo(err.id) << err.info << endl;
				}
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}

			if (pOS != NULL)
			{
				*pOS << TEXT("Jacobian(");
				*pOS << n;
				*pOS << TEXT(")=\r\n");
				Output(pOS, JacobianValue);
				*pOS << TEXT("\r\n");
			}

			try
			{
				CalcPhiValue(pOS, Equations, PhiValue);
			}
			catch (TError& err)
			{
				if (pOS != NULL)
				{
					*pOS << TEXT("�޷����㡣\r\n");
					*pOS << GetErrorInfo(err.id) << err.info << endl;
				}
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}
			if (pOS != NULL)
			{
				*pOS << TEXT("Phi(");
				*pOS << n;
				*pOS << TEXT(")=\r\n");
				Output(pOS, PhiValue);
				*pOS << TEXT("\r\n");
			}

			switch (SolveLinear(JacobianValue, DeltaQ, PhiValue))
			{
			case ERROR_SINGULAR_MATRIX:
				//��������
				if (pOS != NULL) *pOS << TEXT("Jacobian�����������޽⣨��ֵ�����ʻ��ߴ���ì�ܷ��̣���\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			case ERROR_INDETERMINATE_EQUATION:
				if (pOS != NULL) *pOS << TEXT("���������顣����һ���ؽ⡣\r\n");
				break;
			case ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW:
				if (pOS != NULL) *pOS << TEXT("Jacobian������Phi�����������ȣ��������\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			case ERROR_INFINITY_SOLUTIONS:
				if (pOS != NULL) *pOS << TEXT("Jacobian�������죬���������⣨���ڵȼ۷��̣�������һ���ؽ⡣\r\n");
				break;
			case ERROR_OVER_DETERMINED_EQUATIONS:
				if (pOS != NULL) *pOS << TEXT("ì�ܷ����飬�޷���⡣\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}

			if (pOS != NULL)//���DeltaQ
			{
				*pOS << TEXT("��q(");
				*pOS << n;
				*pOS << TEXT(")=\r\n");
				Output(pOS, DeltaQ);
				*pOS << TEXT("\r\n\r\n");
			}

			VectorAdd(Q, DeltaQ);

			if (AllIs0(DeltaQ))
				break;

			if (n > max_step - 1)//
			{
				if (pOS != NULL) *pOS << TEXT("����") << max_step << TEXT("����δ������\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}
			n++;
		}
		//�˴��ѽ��

		VariableTable.SetValueByVarTable(VariableTableUnsolved);

		hasSolved = true;
	}

	if (pOS != NULL) *pOS << TEXT("\r\n�õ������\r\n");
	VariableTableUnsolved.OutputValue(pOS);
}


void TEquations::SimplifyEquations(Ostream *pOS)//���������еļ򵥷��̽��
{
	std::vector<bool> vecHasSolved(Equations.size(), false);
	//for (auto pExpr : Equations)
	for (size_t i = 0; i < Equations.size(); ++i)
	{
		if (vecHasSolved[i] == false)
		{
			TExpressionTree *pExpr = Equations[i];

			//����
			pExpr->Subs(VariableTableSolved.VariableTable, VariableTableSolved.VariableValue, pOS != NULL);

			if (pExpr->CheckOnlyOneVar())
			{
				String var;
				double value;
				pExpr->Solve(var, value);
				VariableTableSolved.VariableTable.push_back(var);//Ϊ����λ����������������
				VariableTableSolved.VariableValue.push_back(value);

				//VariableTableUnsolved.DeleteByAddress(var);//����ѽ������

				vecHasSolved[i] = true;
				i = -1;//�ػ����
			}
		}
	}

	//����ѽ������
	for (auto &pVar : VariableTableSolved.VariableTable)
	{
		VariableTableUnsolved.RemoveOne(pOS,pVar,true);
	}

	if (pOS != NULL) *pOS << TEXT(">>Simplify:\r\n\r\n");

	//������ѽ������
	for (int i = vecHasSolved.size() - 1; i >= 0; --i)
	{
		if (vecHasSolved[i] == true)
		{
			if (pOS != NULL)
			{
				*pOS << Equations[i]->OutputStr();
				*pOS << TEXT("\r\n");
			}
			delete Equations[i];
			auto iter = Equations.begin() + i;
			Equations.erase(iter);

			auto iter2 = EquationIsTemp.begin() + i;
			EquationIsTemp.erase(iter2);
		}
	}

	VariableTable.SetValueByVarTable(VariableTableSolved);

	//
	if (VariableTableUnsolved.VariableTable.size() == 0)
		hasSolved = true;//false��AddEquation����

	if (pOS != NULL)
	{
		*pOS << TEXT("\r\n��ã�\r\n");
		VariableTableSolved.OutputValue(pOS);
		*pOS << TEXT("\r\n");
	}
}

double TEquations::GetValue(const String &var)
{
	auto it = VariableTable.FindVariableTable(var);
	if (it == VariableTable.VariableTable.end())
		throw TError{ ERROR_UNDEFINED_VARIABLE, var };

	return VariableTable.VariableValue[it - VariableTable.VariableTable.begin()];
}