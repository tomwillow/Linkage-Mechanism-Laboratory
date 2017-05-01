#pragma once
#include "DetectMemoryLeak.h"

#include <Windows.h>
#include "String.h"
#include "TEquations.h"

#include "TTransfer.h"

TEquations::TEquations()
{
	eError = ERROR_NO;
	VariableTableUnsolved.bShared = true;
	VariableTableSolved.bShared = true;
}


TEquations::~TEquations()
{
	//释放方程组
	ReleaseTPEquations(Equations);
	ReleaseTPEquations(EquationsV);
	ReleaseTPEquations(EquationsA);

	//释放雅可比
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

	//VariableTableUnsolved = VariableTable;
	//VariableTableUnsolved.bShared = true;

}

size_t TEquations::GetEquationsCount()
{
	return Equations.size();
}

void TEquations::DefineVariable(String *pStr, TCHAR *input_str, TCHAR *input_num)
{
	VariableTable.Define(pStr, input_str, input_num);

	VariableTableUnsolved.VariableTable = VariableTable.VariableTable;
	VariableTableUnsolved.VariableValue = VariableTable.VariableValue;

}

void TEquations::AddEquation(String *pStr, TCHAR *szInput, bool istemp)
{
	TExpressionTree *temp = new TExpressionTree;
	temp->LinkVariableTable(&VariableTable);
	temp->Read(szInput, false);

	temp->Simplify(false);

	if ((eError = temp->GetError()) != ERROR_NO)//若出错
	{
		if (pStr != NULL) *pStr += temp->GetErrorInfo();
		delete temp;
		return;
	}

	//加入方程组
	Equations.push_back(temp);
	EquationIsTemp.push_back(istemp);

	hasSolved = false;

	//if (output)
	//	return temp->OutputStr();
	//else
	//	return NULL;
	if (pStr != NULL)
	{
		*pStr += TEXT(">>Add:\r\n");
		*pStr += temp->OutputStr();
		*pStr += TEXT("\r\n\r\n");
	}
}

//逐个方程对t求导，得到速度方程组右边
void TEquations::BuildEquationsV(String *pStr)
{
	bool bOutput = pStr == NULL ? false : true;

	if (pStr != NULL)
	{
		*pStr += TEXT(">>BuildEquationsV: \r\n");
		*pStr += TEXT("当前方程：\r\n");
	}

	TExpressionTree *pEquatemp;
	for (auto pEqua : Equations)
	{
		pEquatemp = new TExpressionTree;
		*pEquatemp = *pEqua;

		pEquatemp->Diff(TEXT("t"), 1, bOutput);
		pEquatemp->Simplify(bOutput);

		EquationsV.push_back(pEquatemp);

		if (pStr != NULL)
		{
			*pStr += pEquatemp->OutputStr();
			*pStr += TEXT("\r\n");
		}
	}
}

//逐个方程对t求导，得到速度方程组右边
void TEquations::BuildEquationsA_Phitt(String *pStr)
{
	bool bOutput = pStr == NULL ? false : true;

	if (pStr != NULL)
	{
		*pStr += TEXT(">>Build Equations A: \r\n");
		*pStr += TEXT("当前方程：\r\n");
	}

	TExpressionTree *pEquatemp;
	for (auto pEqua : EquationsV)
	{
		pEquatemp = new TExpressionTree;
		*pEquatemp = *pEqua;

		pEquatemp->Diff(TEXT("t"), 1, bOutput);
		pEquatemp->Simplify(bOutput);

		EquationsA.push_back(pEquatemp);

		if (pStr != NULL)
		{
			*pStr += pEquatemp->OutputStr();
			*pStr += TEXT("\r\n");
		}
	}
}

//Jacobian为符号矩阵，乘以Vector数值向量，得到符号方程向量。结果存入EquationsResult
void TEquations::CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector)
{
	TExpressionTree *expr;
	for (auto &Line : Jacobian)
	{
		expr = new TExpressionTree;
		for (auto iter = Line.begin(); iter != Line.end();++iter)
		{
			//Jacobian每项乘以q'
			(**iter)*Vector[iter - Line.begin()];
			(*iter)->Simplify(false);

			//加起来
			*expr + (**iter);
		}
		expr->Simplify(false);
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


//应在解出位置、速度方程后调用
void TEquations::CalcEquationsARight(String *pStr,TVector &Right)
{
	//复制Jacobian矩阵
	TJacobian JacobianTemp;
	CopyJacobian(JacobianTemp, Jacobian);
	//OutputJacobian(pStr, JacobianTemp);

	//Jacobian*q' 乘以q'
	TPEquations EquationsTemp;
	CalcJacobianMultiplyVector(EquationsTemp, JacobianTemp, VariableTableV.VariableValue);
	//OutputPhi(pStr, EquationsTemp);

	//(Jacobian*q')q  对q求导
	BuildJacobian_inner(JacobianTemp, EquationsTemp, VariableTableA);
	//OutputJacobian(pStr, JacobianTemp);

	//Vpa: (Jacobian*q')q
	TMatrix MatrixTemp;
	CalcJacobianValue(pStr, MatrixTemp, JacobianTemp);
	//Output(pStr, MatrixTemp);

	// *q'
	MatrixMultiplyVector(Right, MatrixTemp, VariableTableV.VariableValue);

	//-Phitt
	TVector Phitt;
	CalcPhiValue(pStr, EquationsA, Phitt);

	//-Right-Phitt
	for (auto &iter = Right.begin(); iter != Right.end(); ++iter)
		(*iter) = -(*iter) + Phitt[iter - Right.begin()];

	ReleaseJacobian(JacobianTemp);
	ReleaseTPEquations(EquationsTemp);
}

//替换单一变量
void TEquations::SubsV(String *pStr, TCHAR *VarStr, double Value)
{
	SubsVar(pStr, EquationsV, VariableTable, VarStr, Value);
}

//替换单一变量
void TEquations::SubsA(String *pStr, TCHAR *VarStr, double Value)
{
	SubsVar(pStr, EquationsA, VariableTable, VarStr, Value);
}

void TEquations::SubsVar(String *pStr, TPEquations &Equations, TVariableTable &LinkVariableTable, TCHAR *VarStr, double Value)
{
	if (eError != ERROR_NO)
		return;

	TCHAR *ptVar;
	if ((ptVar = LinkVariableTable.FindVariableTable(VarStr)))
	{
		for (auto pEquation : Equations)
		{
			pEquation->Subs(ptVar, Value, false);
		}
	}
}

//已解出变量组加入 未解出变量组剔除
void TEquations::Subs(String *pStr, TCHAR *subsVar, TCHAR *subsValue)//代入
{
	if (eError != ERROR_NO)
		return;


	TVariableTable exceptVars;
	//若输入了替换变量，则进行定义
	if (subsVar != NULL)
	{
		exceptVars.Define(pStr, subsVar, subsValue);
		TCHAR *ptVar;
		for (size_t i = 0; i < exceptVars.VariableTable.size(); ++i)
		{
			if ((ptVar = VariableTable.FindVariableTable(exceptVars.VariableTable[i])) && VariableTableSolved.FindVariableTable(ptVar) == NULL)
			{
				VariableTableSolved.VariableTable.push_back(ptVar);
				VariableTableSolved.VariableValue.push_back(exceptVars.VariableValue[i]);
			}
		}
		//VariableTableSolved.SetValueByVarTable(exceptVars);
	}

	if (pStr != NULL)
	{
		*pStr << TEXT(">>Subs: [") << subsVar;
		*pStr += TEXT("] -> [");
		*pStr += subsValue;
		*pStr += TEXT("]\r\n\r\n当前方程：\r\n");
	}
	for (auto pEquation : Equations)//遍历方程
	{
		pEquation->LinkVariableTable(&VariableTableUnsolved);

		//替换掉机架点坐标
		if (subsVar != NULL && _tcslen(subsVar) > 0)
			pEquation->Subs(subsVar, subsValue, false);

		if (pStr != NULL)
		{
			*pStr += pEquation->OutputStr(false);
			*pStr += TEXT("\r\n");
		}
	}

	if (pStr != NULL)
	{
		*pStr += TEXT("\r\n");
	}

	//剔除掉被替换掉的变量
	if (subsVar != NULL && _tcslen(subsVar) > 0)
	{
		VariableTableUnsolved.Remove(pStr, subsVar);
	}

}

//将未解出变量赋值给速度变量组
void TEquations::BuildVariableTableV(String *pStr)
{
	VariableTableV = VariableTableUnsolved;
	VariableTableV.bShared = true;
}

//将未解出变量赋值给速度变量组
void TEquations::BuildVariableTableA(String *pStr)
{
	VariableTableA = VariableTableUnsolved;
	VariableTableA.bShared = true;
}

//Copy Jacobian
void TEquations::CopyJacobian(TJacobian &ResultJacobian, const TJacobian &OriginJacobian)
{
	if (eError != ERROR_NO)
		return;

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

void TEquations::BuildJacobian_inner(TJacobian &JacobianResult,const TPEquations &Equations,TVariableTable &VariableTable)
{
	if (eError != ERROR_NO)
		return;

	//释放旧的雅可比
	ReleaseJacobian(JacobianResult);

	TExpressionTree *temp;

	//构建雅可比矩阵
	JacobianResult.resize(Equations.size());
	for (size_t i = 0; i < Equations.size(); i++)//遍历方程
	{
		//以未解出变量建立雅可比矩阵
		Equations[i]->LinkVariableTable(&VariableTable);

		Equations[i]->Simplify(false);
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

//链接VariableTableUnsolved
void TEquations::BuildJacobian(String *pStr)
{
	BuildJacobian_inner(Jacobian, Equations, VariableTableUnsolved);

	//纯输出
	if (pStr != NULL)
	{
		*pStr += TEXT(">>Build Jacobian:\r\n\r\n");

		OutputPhi(pStr, Equations);

		OutputJacobian(pStr, Jacobian);
	}
}

void TEquations::OutputPhi(String *pStr,TPEquations &Equations)
{
	if (pStr != NULL)
	{
		*pStr += TEXT("Phi(1x");
		*pStr << Equations.size();
		*pStr << TEXT(")=\r\n[");
		for (auto iter = Equations.begin(); iter != Equations.end(); ++iter)
		{
			*pStr += (*iter)->OutputStr();
			if (iter != Equations.end() - 1)
				*pStr += TEXT(";\r\n");
		}
		*pStr += TEXT("]\r\n\r\n");
	}
}

void TEquations::OutputJacobian(String *pStr,const TJacobian &Jacobian)
{
	//纯输出
	if (pStr != NULL)
	{
		*pStr += TEXT("Jacobian(");
		*pStr << (Jacobian.size()>0 ? Jacobian[0].size() : 1) << TEXT("x") << Jacobian.size();
		*pStr << TEXT(")=\r\n[");
		for (size_t ii = 0; ii < Jacobian.size(); ii++)
		{
			for (size_t jj = 0; jj < Jacobian[ii].size(); jj++)
			{
				*pStr += Jacobian[ii][jj]->OutputStr();
				*pStr += TEXT(" ");
			}
			if (ii != Jacobian.size() - 1)
				*pStr += TEXT(";\r\n");
		}
		*pStr += TEXT("]\r\n\r\n");
	}
}

void TEquations::Output(String *pStr, TMatrix& m)
{
	if (pStr != NULL)
	{
		TCHAR *temp = new TCHAR[20];
		*pStr += TEXT("[");
		for (size_t i = 0; i < m.size(); i++)
		{
			for (size_t j = 0; j < m[i].size(); j++)
			{
				_stprintf(temp, TEXT("%f"), m[i][j]);
				*pStr += temp;
				*pStr += TEXT(" ");
			}
			if (i != m.size() - 1)
				*pStr += TEXT(";\r\n");
		}
		*pStr += TEXT("]");

		delete[] temp;
	}
}

void TEquations::Output(String *pStr, TVector& v)
{
	if (pStr != NULL)
	{
		TCHAR *temp = new TCHAR[20];
		*pStr += TEXT("[");
		for (auto value:v)
		{
			_stprintf(temp, TEXT("%f"), value);
			*pStr += temp;
			*pStr += TEXT(" ");
		}
		*pStr += TEXT("]");
		delete[] temp;

	}
}

//利用变量表中的值计算雅可比
void TEquations::CalcJacobianValue(String *pStr, TMatrix &JacobianValueResult,const TJacobian &Jacobian)
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
				JacobianValueResult[i].push_back(temp->Value(true));//得到临时表达式值存入雅可比
			}
			catch (enumError& err)
			{
				if (pStr != NULL)
				{
					*pStr += TEXT("ERROR:");
					*pStr += temp->OutputStr(true);
					*pStr += TEXT("\r\nJacobian计算出错:");
					*pStr += temp->GetErrorInfo();
				}
				delete temp;
				throw err;
			}
			delete temp;
		}
	}
}

//利用变量表中的值计算，方程中不前缀负号，计算出值加负号
void TEquations::CalcPhiValue(String *pStr,const TPEquations &Equations, TVector &PhiValue)
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
			PhiValue.push_back(-temp->Value(true));//得到临时表达式值存入
		}
		catch (enumError& err)
		{
			if (pStr != NULL)
			{
				*pStr += TEXT("ERROR:");
				*pStr += temp->OutputStr(true);
				*pStr += TEXT("\r\nPhi计算出错:");
				*pStr += temp->GetErrorInfo();
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
	auto m = A.size();//行数
	auto n = m;//列数=未知数个数

	auto RankA = m, RankAb = m;//初始值

	if (x.size() != m) x.resize(m);//仅对方阵成立

	if (m != b.size())//Jacobian行数不等于Phi行数
		return ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW;

	if (m > 0)
		if ((n = A[0].size()) != m)//不是方阵
		{
			if (m > n)
				return ERROR_OVER_DETERMINED_EQUATIONS;
			x.resize(n);
			//return ERROR_INDETERMINATE_EQUATION;
		}

	std::vector<decltype(m)> TrueRowNumber(n);

	//列主元消元法
	for (decltype(m) y = 0, x = 0; y < m; y++, x++)
	{
		//if (A[i].size() != m)

		//交换本行与最大行
		SwapRow(A, b, y, GetMaxAbsRowIndex(A, y, m - 1, x));

		while (abs(A[y][x]) < epsilon)//x一直递增到非0
		{
			x++;
			if (x == n)
				break;

			//交换本行与最大行
			SwapRow(A, b, y, GetMaxAbsRowIndex(A, y, m - 1, x));
		}

		if (x != n && x > y)
		{
			TrueRowNumber[y] = x;//补齐方程时 当前行应换到x行
		}

		if (x == n)//本行全为0
		{
			RankA = y;
			if (abs(b[y]) < epsilon)
				RankAb = y;

			if (RankA != RankAb)//奇异，且系数矩阵及增广矩阵秩不相等->无解
				return ERROR_SINGULAR_MATRIX;
			else
				break;//跳出for，得到特解
		}

		//主对角线化为1
		double m_num = A[y][x];
		for (decltype(m) j = y; j < n; j++)//y行第j个->第n个
			A[y][j] /= m_num;
		b[y] /= m_num;

		//每行化为0
		for (decltype(m) row = y + 1; row < m; row++)//下1行->最后1行
		{
			if (abs(A[row][x]) < epsilon)
				;
			else
			{
				double mi = A[row][x];
				for (auto col = x; col < n; col++)//row行第x个->第n个
				{
					A[row][col] -= A[y][col] * mi;
				}
				b[row] -= b[y] * mi;
			}
		}
	}

	bool bIndeterminateEquation = false;//设置此变量是因为后面m将=n，标记以判断是否为不定方程组

	//若为不定方程组，空缺行全填0继续运算
	if (m != n)
	{
		A.resize(n);//A改为n行
		for (auto i = m; i < n; i++)//A从m行开始每行n个数
			A[i].resize(n);
		b.resize(n);
		m = n;
		bIndeterminateEquation = true;

		//调整顺序
		for (int i = m - 1; i >= 0; i--)
		{
			if (TrueRowNumber[i] != 0)
			{
				SwapRow(A, b, i, TrueRowNumber[i]);
			}
		}
	}

	//后置换得到x
	double sum_others = 0.0;
	for (int i = m - 1; i >= 0; i--)//最后1行->第1行
	{
		sum_others = 0.0;
		for (decltype(m) j = i + 1; j < m; j++)//本列 后的元素乘以已知x 加总
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
	for (auto value: V)
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


void TEquations::SolveEquationsV(String *pStr)//求解方程组V
{
	if (eError != ERROR_NO)
		return;

	TMatrix JacobianV;
	TVector Phi;
	TVector &dQ = VariableTableV.VariableValue;
	CalcPhiValue(pStr, EquationsV, Phi);
	CalcJacobianValue(pStr, JacobianV, Jacobian);
	SolveLinear(JacobianV, dQ, Phi);

	if (pStr != NULL)
	{
		*pStr += TEXT(">>SolveEquationsV:\r\n");
		if (pStr != NULL) *pStr += TEXT("\r\n得到结果：\r\n");
		VariableTableV.OutputValue(pStr);
	}
}

void TEquations::SolveEquationsA(String *pStr)//求解方程组A
{
	if (eError != ERROR_NO)
		return;

	TMatrix JacobianA;
	TVector Phi;
	TVector &ddQ = VariableTableA.VariableValue;
	CalcJacobianValue(pStr, JacobianA, Jacobian);
	CalcEquationsARight(pStr,Phi);
	SolveLinear(JacobianA, ddQ, Phi);

	if (pStr != NULL)
	{
		*pStr += TEXT(">>SolveEquationsA:\r\n");
		if (pStr != NULL) *pStr += TEXT("\r\n得到结果：\r\n");
		VariableTableA.OutputValue(pStr);
	}
}

//牛顿-拉夫森方法求解
void TEquations::SolveEquations(String *pStr)
{
	if (eError != ERROR_NO)
		return;

	if (hasSolved == false)
	{
		if (pStr != NULL)
		{
			*pStr += TEXT(">>SolveEquations:\r\n");
			*pStr += TEXT("当前未知量：\r\n");
		}
		VariableTableUnsolved.Output(pStr);//输出当前变量

		//BuildJacobian(pStr);//建立Jacobian, subsVar, subsValue

		TMatrix JacobianValue;
		TVector PhiValue, DeltaQ, &Q = VariableTableUnsolved.VariableValue;
		TVector VariableValueBackup = VariableTableUnsolved.VariableValue;
		int n = 0;

		while (1)
		{
			if (pStr != NULL)
			{
				*pStr += TEXT("q(");
				*pStr << n;
				//*pStr += TTransfer::int2TCHAR(n, buffer);
				*pStr += TEXT(")=\r\n");
				Output(pStr, Q);
				*pStr += TEXT("\r\n");
			}

			try
			{
				CalcJacobianValue(pStr, JacobianValue,Jacobian);
			}
			catch (enumError& err)
			{
				if (pStr != NULL) *pStr += TEXT("无法计算。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}

			if (pStr != NULL)
			{
				*pStr += TEXT("Jacobian(");
				*pStr << n;
				*pStr += TEXT(")=\r\n");
				Output(pStr, JacobianValue);
				*pStr += TEXT("\r\n");
			}

			try
			{
				CalcPhiValue(pStr, Equations, PhiValue);
			}
			catch (enumError& err)
			{
				if (pStr != NULL) *pStr += TEXT("无法计算。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}
			if (pStr != NULL)
			{
				*pStr += TEXT("Phi(");
				*pStr << n;
				*pStr += TEXT(")=\r\n");
				Output(pStr, PhiValue);
				*pStr += TEXT("\r\n");
			}

			switch (SolveLinear(JacobianValue, DeltaQ, PhiValue))
			{
			case ERROR_SINGULAR_MATRIX:
				//矩阵奇异
				if (pStr != NULL) *pStr += TEXT("Jacobian矩阵奇异且无解（存在矛盾方程）。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			case ERROR_INDETERMINATE_EQUATION:
				if (pStr != NULL) *pStr += TEXT("不定方程组。返回一组特解。\r\n");
				break;
			case ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW:
				if (pStr != NULL) *pStr += TEXT("Jacobian矩阵与Phi向量行数不等，程序出错。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			case ERROR_INFINITY_SOLUTIONS:
				if (pStr != NULL) *pStr += TEXT("Jacobian矩阵奇异，但有无穷多解（存在等价方程）。返回一组特解。\r\n");
				break;
			case ERROR_OVER_DETERMINED_EQUATIONS:
				if (pStr != NULL) *pStr += TEXT("矛盾方程组，无法求解。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}

			if (pStr != NULL)//输出DeltaQ
			{
				*pStr += TEXT("Δq(");
				*pStr << n;
				*pStr += TEXT(")=\r\n");
				Output(pStr, DeltaQ);
				*pStr += TEXT("\r\n\r\n");
			}

			VectorAdd(Q, DeltaQ);

			if (AllIs0(DeltaQ))
				break;

			if (n > 19)//
			{
				if (pStr != NULL) *pStr += TEXT("超过20步仍未收敛。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}
			n++;
		}
		//此处已解出

		VariableTable.SetValueByVarTable(VariableTableUnsolved);

		hasSolved = true;
	}
	else
	{
		//VariableTable.SetValueByVarTable(VariableTableSolved);
	}

	if (pStr != NULL) *pStr += TEXT("\r\n得到结果：\r\n");
	VariableTable.OutputValue(pStr);

}


void TEquations::SimplifyEquations(String *pStr)//将方程组中的简单方程解出
{
	if (eError != ERROR_NO)
		return;

	std::vector<bool> vecHasSolved(Equations.size(), false);
	//for (auto pExpr : Equations)
	for (size_t i = 0; i < Equations.size(); ++i)
	{
		if (vecHasSolved[i] == false)
		{
			TExpressionTree *pExpr = Equations[i];

			//代入
			pExpr->Subs(VariableTableSolved.VariableTable, VariableTableSolved.VariableValue, pStr != NULL);

			if (pExpr->CheckOnlyOneVar())
			{
				TCHAR *var;
				double value;
				pExpr->Solve(var, value);
				VariableTableSolved.VariableTable.push_back(var);//为共享单位，不负责析构变量
				VariableTableSolved.VariableValue.push_back(value);

				//VariableTableUnsolved.DeleteByAddress(var);//清除已解出变量

				vecHasSolved[i] = true;
				i = -1;//重回起点
			}
		}
	}

	//清除已解出变量
	for (auto pVar:VariableTableSolved.VariableTable)
	{
		VariableTableUnsolved.DeleteByAddress(pVar);
	}

	if (pStr != NULL) *pStr += TEXT(">>Simplify:\r\n\r\n");

	//清理掉已解出方程
	for (int i = vecHasSolved.size() - 1; i >= 0; --i)
	{
		if (vecHasSolved[i] == true)
		{
			if (pStr != NULL)
			{
				*pStr += Equations[i]->OutputStr();
				*pStr += TEXT("\r\n");
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
		hasSolved = true;//false由AddEquation触发

	if (pStr != NULL)
	{
		*pStr += TEXT("\r\n解得：\r\n");
		VariableTableSolved.OutputValue(pStr);
		*pStr += TEXT("\r\n");
	}
}