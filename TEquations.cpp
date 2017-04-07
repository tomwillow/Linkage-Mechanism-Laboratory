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
	for (int i = 0; i < Equations.size(); i++)
		delete Equations[i];

	for (int i = 0; i < EquationsSolved.size(); i++)
		delete EquationsSolved[i];

	//释放雅可比
	for (int i = 0; i < Jacobian.size(); i++)
		for (int j = 0; j < Jacobian[i].size(); j++)
			delete Jacobian[i][j];
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
		if (pStr!=NULL) *pStr += temp->GetErrorInfo();
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
}

void TEquations::Subs(String *pStr, TCHAR *subsVar, TCHAR *subsValue)//代入
{
	if (eError != ERROR_NO)
		return;


	TVariableTable exceptVars;
	//若输入了替换变量，则进行定义
	if (subsVar != NULL)
	{
		exceptVars.Define(pStr, subsVar,subsValue);
		TCHAR *ptVar;
		for (int i = 0; i < exceptVars.VariableTable.size(); ++i)
		{
			if ((ptVar = VariableTable.FindVariableTable(exceptVars.VariableTable[i])) && VariableTableSolved.FindVariableTable(ptVar)==NULL)
			{
				VariableTableSolved.VariableTable.push_back(ptVar);
				VariableTableSolved.VariableValue.push_back(exceptVars.VariableValue[i]);
			}
		}
		//VariableTableSolved.SetValueByVarTable(exceptVars);
	}

	if (pStr!=NULL) *pStr += TEXT("替换完成。当前方程：\r\n");
	for (int i = 0; i < Equations.size(); i++)//遍历方程
	{
		Equations[i]->LinkVariableTable(&VariableTableUnsolved);

		//替换掉机架点坐标
		if (subsVar != NULL && _tcslen(subsVar)>0)
			Equations[i]->Subs(subsVar, subsValue, false);

		if (pStr != NULL)
		{
			*pStr += Equations[i]->OutputStr(false);
			*pStr += TEXT("\r\n");
		}
	}

		//剔除掉被替换掉的变量
	if (subsVar != NULL && _tcslen(subsVar) > 0)
	{
		VariableTableUnsolved.Remove(pStr,subsVar);
	}

		//if (bOutput)
		//	return Str.c_str();
		//else
		//	return TEXT("");
}

void TEquations::BuildJacobian(String *pStr, TCHAR *subsVar, TCHAR *subsValue)
{
	if (eError != ERROR_NO)
		return;

	//释放旧的雅可比
	for (int i = 0; i < Jacobian.size(); i++)
		for (int j = 0; j < Jacobian[i].size(); j++)
			delete Jacobian[i][j];

	TExpressionTree *temp;
	TVariableTable exceptVars;
	//若输入了替换变量，则进行定义
	if (subsVar != NULL)
		exceptVars.Define(pStr, subsVar);

	//构建雅可比矩阵
	Jacobian.clear();
	Jacobian.resize(Equations.size());
	for (int i = 0; i < Equations.size(); i++)//遍历方程
	{
		//以未解出变量建立雅可比矩阵
		Equations[i]->LinkVariableTable(&VariableTableUnsolved);

		//替换掉机架点坐标
		if (subsVar != NULL && _tcslen(subsVar)>0)
			Equations[i]->Subs(subsVar, subsValue, false);

		Equations[i]->Simplify(false);
		for (int j = 0; j < VariableTableUnsolved.VariableTable.size(); j++)
		{
			//被排除变量不进入雅可比矩阵列
			if (exceptVars.FindVariableTable(VariableTableUnsolved.VariableTable[j]) == NULL)//非排除变量
			{
				temp = new TExpressionTree;
				*temp = *Equations[i];
				temp->Diff(VariableTableUnsolved.VariableTable[j], 1, false);
				temp->Simplify(false);
				Jacobian[i].push_back(temp);
			}
		}
	}

	//剔除掉被替换掉的变量
	if (subsVar != NULL && _tcslen(subsVar)>0)
		VariableTableUnsolved.Remove(pStr,subsVar);

	//
	if (pStr!=NULL)
	{
		*pStr+=TEXT("\r\nPhi=\r\n[");
		for (int i = 0; i < Equations.size(); i++)
		{
			*pStr+=Equations[i]->OutputStr();
			if (i != Equations.size() - 1)
				*pStr += TEXT(";\r\n");
		}
		*pStr += TEXT("]\r\n");

		*pStr += TEXT("\r\nJacobian=\r\n[");
		for (int ii = 0; ii < Jacobian.size(); ii++)
		{
			for (int jj = 0; jj < Jacobian[ii].size(); jj++)
			{
				*pStr += Jacobian[ii][jj]->OutputStr();
				*pStr += TEXT(" ");
			}
			if (ii != Jacobian.size() - 1)
				*pStr += TEXT(";\r\n");
		}
		*pStr += TEXT("]\r\n");
	}
}

void TEquations::Output(String *pStr,Matrix& m)
{
	if (pStr != NULL)
	{
		TCHAR *temp = new TCHAR[20];
		*pStr += TEXT("[");
		for (int i = 0; i < m.size(); i++)
		{
			for (int j = 0; j < m[i].size(); j++)
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

void TEquations::Output(String *pStr,Vector& v)
{
	if (pStr != NULL)
	{
	TCHAR *temp = new TCHAR[20];
	*pStr += TEXT("[");
	for (int i = 0; i < v.size(); i++)
	{
		_stprintf(temp, TEXT("%f"), v[i]);
		*pStr += temp;
		*pStr += TEXT(" ");
	}
	*pStr += TEXT("]");
	delete[] temp;

	}
}

//利用变量表中的值计算雅可比
void TEquations::CalcJacobianValue(String *pStr,Matrix &JacobianValue, const Vector &Q)
{
	JacobianValue.clear();
	JacobianValue.resize(Jacobian.size());
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
				JacobianValue[i].push_back(temp->Value(true));//得到临时表达式值存入雅可比
			}
			catch (enumError& err)
			{
				if (pStr != NULL)
				{
					*pStr+=TEXT("ERROR:");
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

//利用变量表中的值计算，每个值前均加了负号
void TEquations::CalcPhiValue(String *pStr,Vector &PhiValue, const Vector &Q)
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

int TEquations::GetMaxAbsRowIndex(const Matrix &A, int RowStart, int RowEnd, int Col)
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

void TEquations::SwapRow(Matrix &A, Vector &b, int i, int j)
{
	if (i == j)
		return;
	Vector temp(A[i].size());
	temp = A[i];
	A[i] = A[j];
	A[j] = temp;

	double n;
	n = b[i];
	b[i] = b[j];
	b[j] = n;
}

enumError TEquations::SolveLinear(Matrix &A, Vector &x, Vector &b)
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
				for (int col = x; col < n; col++)//row行第x个->第n个
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
	for (int i = m - 1; i >= 0; i--)//最后1行->第1行
	{
		double sum_others = 0.0;
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

bool TEquations::AllIs0(Vector &V)
{
	for (int i = 0; i < V.size(); i++)
	{
		if (abs(V[i]) >= epsilon)
			return false;
	}
	return true;
}

bool TEquations::VectorAdd(Vector &Va, const Vector &Vb)
{
	if (Va.size() != Vb.size())
		return false;
	for (int i = 0; i < Va.size(); i++)
	{
		Va[i] += Vb[i];
	}
	return true;
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
			*pStr+= TEXT("");
			*pStr += TEXT("当前未知量：\n\r");
		}
		VariableTableUnsolved.Output(pStr);//输出当前变量

		BuildJacobian(pStr);//建立Jacobian, subsVar, subsValue

		TCHAR *buffer = new TCHAR[20];
		Matrix JacobianValue;
		Vector PhiValue, DeltaQ, &Q = VariableTableUnsolved.VariableValue;
		Vector VariableValueBackup = VariableTableUnsolved.VariableValue;
		int n = 0;

		while (1)
		{
			if (pStr!=NULL)
			{
				*pStr+= TEXT("q(");
				*pStr += TTransfer::int2TCHAR(n, buffer);
				*pStr += TEXT(")=\r\n");
				Output(pStr,Q);
				*pStr += TEXT("\r\n");
			}

			try
			{
				CalcJacobianValue(pStr,JacobianValue, Q);
			}
			catch (enumError& err)
			{
				if (pStr != NULL) *pStr += TEXT("无法计算。\r\n");
				delete[] buffer;
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}

			if (pStr != NULL)
			{
				*pStr += TEXT("Jacobian(");
				*pStr += TTransfer::int2TCHAR(n, buffer);
				*pStr += TEXT(")=\r\n");
				Output(pStr,JacobianValue);
				*pStr += TEXT("\r\n");
			}

			try
			{
				CalcPhiValue(pStr,PhiValue, Q);
			}
			catch (enumError& err)
			{
				if (pStr != NULL) *pStr += TEXT("无法计算。\r\n");
				delete[] buffer;
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return ;
			}
			if (pStr != NULL)
			{
				*pStr += TEXT("Phi(");
				*pStr += TTransfer::int2TCHAR(n, buffer);
				*pStr += TEXT(")=\r\n");
				Output(pStr,PhiValue);
				*pStr += TEXT("\r\n");
			}

			switch (SolveLinear(JacobianValue, DeltaQ, PhiValue))
			{
			case ERROR_SINGULAR_MATRIX:
				//矩阵奇异
				if (pStr != NULL) *pStr += TEXT("Jacobian矩阵奇异且无解（存在矛盾方程）。\r\n");
				delete[] buffer;
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return ;
			case ERROR_INDETERMINATE_EQUATION:
				if (pStr != NULL) *pStr += TEXT("不定方程组。返回一组特解。\r\n");
				break;
			case ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW:
				if (pStr != NULL) *pStr += TEXT("Jacobian矩阵与Phi向量行数不等，程序出错。\r\n");
				delete[] buffer;
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return ;
			case ERROR_INFINITY_SOLUTIONS:
				if (pStr != NULL) *pStr += TEXT("Jacobian矩阵奇异，但有无穷多解（存在等价方程）。返回一组特解。\r\n");
				break;
			case ERROR_OVER_DETERMINED_EQUATIONS:
				if (pStr != NULL) *pStr += TEXT("矛盾方程组，无法求解。\r\n");
				delete[] buffer;
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return ;
			}

			if (pStr != NULL)//输出DeltaQ
			{
				*pStr += TEXT("Δq(");
				*pStr += TTransfer::int2TCHAR(n, buffer);
				*pStr += TEXT(")=\r\n");
				Output(pStr,DeltaQ);
				*pStr += TEXT("\r\n\r\n");
			}

			VectorAdd(Q, DeltaQ);

			if (AllIs0(DeltaQ))
				break;

			if (n > 19)//
			{
				if (pStr != NULL) *pStr += TEXT("超过20步仍未收敛。\r\n");
				delete[] buffer;
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return ;
			}
			n++;
		}
		//此处已解出

		VariableTable.SetValueByVarTable(VariableTableUnsolved);

		hasSolved = true;
		delete[] buffer;
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
			pExpr->Subs(VariableTableSolved.VariableTable, VariableTableSolved.VariableValue,pStr!=NULL);

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
	for (int i = 0; i < VariableTableSolved.VariableTable.size(); ++i)
	{
		VariableTableUnsolved.DeleteByAddress(VariableTableSolved.VariableTable[i]);
	}

	if (pStr!=NULL) *pStr+= TEXT("简单方程：\r\n");

	//清理掉已解出方程
	for (int i = vecHasSolved.size()-1; i >=0 ; --i)
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
	}
}