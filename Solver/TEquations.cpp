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

	//加入方程组
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

//逐个方程对t求导，得到速度方程组右边
void TEquations::BuildEquationsV(Ostream *pOS)
{
	bool bOutput = pOS == NULL ? false : true;

	if (pOS != NULL)
	{
		*pOS << TEXT(">>BuildEquationsV: \r\n");
		*pOS << TEXT("当前方程：\r\n");
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

//逐个方程对t求导，得到速度方程组右边
void TEquations::BuildEquationsA_Phitt(Ostream *pOS)
{
	bool bOutput = pOS == NULL ? false : true;

	if (pOS != NULL)
	{
		*pOS << TEXT(">>Build Equations A: \r\n");
		*pOS << TEXT("当前方程：\r\n");
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

//Jacobian为符号矩阵，乘以Vector数值向量，得到符号方程向量。结果存入EquationsResult
//Ax=b
void TEquations::CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector)
{
	TExpressionTree *expr;
	for (auto &Line : Jacobian)//每行
	{
		expr = new TExpressionTree;
		for (auto iter = Line.begin(); iter != Line.end(); ++iter)//每个expr
		{
			//Jacobian每项乘以q'
			(**iter)*Vector[iter - Line.begin()];
			//(*iter)->Simplify(false);

			//加起来
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


//应在解出位置、速度方程后调用
void TEquations::CalcEquationsARight(Ostream *pOS, TVector &Right)
{
	//复制Jacobian矩阵
	TJacobian JacobianTemp;
	CopyJacobian(JacobianTemp, Jacobian);
#ifdef _DEBUG
	OutputJacobian(pOS, JacobianTemp);
#endif

	//Jacobian*q' 乘以q'
	TPEquations EquationsTemp;
	CalcJacobianMultiplyVector(EquationsTemp, JacobianTemp, VariableTableV.VariableValue);
#ifdef _DEBUG
	OutputPhi(pOS, EquationsTemp);
#endif

	//(Jacobian*q')q  对q求导
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

//替换单一变量
void TEquations::SubsV(Ostream *pOS, String VarStr, double Value)
{
	SubsVar(pOS, EquationsV, VariableTable, VarStr, Value);
}

//替换单一变量
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

	//Table中存在
	auto find1 = VariableTable.FindVariableTable(var) != VariableTable.VariableTable.end();

	//已解出中存在
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
		*pOS << TEXT("]\r\n\r\n当前方程：\r\n");
	}
	for (auto pExpr : Equations)//遍历方程
	{
		pExpr->LinkVariableTable(&VariableTableUnsolved);

		//替换
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

	//剔除掉被替换掉的变量
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

//已解出变量组加入 未解出变量组剔除
void TEquations::Subs(Ostream *pOS, const String subsVars, const String subsValues)//代入
{
	std::vector<String> tempVars = StrSliceToVector(subsVars);
	std::vector<double> tempValues = StrSliceToDoubleVector(subsValues);

	Subs(pOS, tempVars, tempValues);
}

//将未解出变量赋值给速度变量组
void TEquations::BuildVariableTableV(Ostream *pOS)
{
	VariableTableV = VariableTableUnsolved;
	VariableTableV.bShared = true;
}

//将未解出变量赋值给速度变量组
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
	//释放旧的雅可比
	ReleaseJacobian(JacobianResult);

	TExpressionTree *temp;

	//构建雅可比矩阵
	JacobianResult.resize(Equations.size());
	for (size_t i = 0; i < Equations.size(); i++)//遍历方程
	{
		//以未解出变量建立雅可比矩阵
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

//链接VariableTableUnsolved
void TEquations::BuildJacobian(Ostream *pOS)
{
	BuildJacobian_inner(Jacobian, Equations, VariableTableUnsolved);

	//纯输出
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
	//纯输出
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

//利用变量表中的值计算雅可比
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
				JacobianValueResult[i].push_back(temp->Value(true));//得到临时表达式值存入雅可比
			}
			catch (TError err)
			{
				if (pOS != NULL)
				{
					*pOS << TEXT("ERROR:");
					*pOS << temp->OutputStr();
					*pOS << TEXT("\r\nJacobian计算出错:");
					*pOS << GetErrorInfo(err.id) + err.info;
				}
				delete temp;
				throw err;
			}
			delete temp;
		}
	}
}

//利用变量表中的值计算，方程中不前缀负号，计算出值加负号
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
			PhiValue.push_back(-temp->Value(true));//得到临时表达式值存入
		}
		catch (TError & err)
		{
			if (pOS != NULL)
			{
				*pOS << TEXT("ERROR:");
				*pOS << temp->OutputStr();
				*pOS << TEXT("\r\nPhi计算出错:");
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
				return ERROR_OVER_DETERMINED_EQUATIONS;//过定义方程组
			else//不定方程组
				x.resize(n);
		}

	std::vector<decltype(m)> TrueRowNumber(n);

	//列主元消元法
	for (decltype(m) y = 0, x = 0; y < m && x < n; y++, x++)
	{
		//if (A[i].size() != m)

		//从当前行(y)到最后一行(m-1)中，找出x列最大的一行与y行交换
		SwapRow(A, b, y, GetMaxAbsRowIndex(A, y, m - 1, x));

		while (abs(A[y][x]) < epsilon)//如果当前值为0  x一直递增到非0
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


void TEquations::SolveEquationsV(Ostream *pOS)//求解方程组V
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
		if (pOS != NULL) *pOS << TEXT("\r\n得到结果：\r\n");
		VariableTableV.OutputValue(pOS);
	}
}

void TEquations::SolveEquationsA(Ostream *pOS)//求解方程组A
{
	TMatrix JacobianA;
	TVector Phi;
	TVector &ddQ = VariableTableA.VariableValue;
	CalcJacobianValue(pOS, JacobianA, Jacobian);//JacobianA与Jacobian相等
	CalcEquationsARight(pOS, Phi);
	SolveLinear(JacobianA, ddQ, Phi);

	if (pOS != NULL)
	{
		*pOS << TEXT(">>SolveEquationsA:\r\n");
		if (pOS != NULL) *pOS << TEXT("\r\n得到结果：\r\n");
		VariableTableA.OutputValue(pOS);
	}
}

//牛顿-拉夫森方法求解
void TEquations::SolveEquations(Ostream *pOS)
{
	if (hasSolved == false)
	{
		if (pOS != NULL)
		{
			*pOS << TEXT(">>SolveEquations:\r\n");
			*pOS << TEXT("当前未知量：\r\n");
		}
		VariableTableUnsolved.Output(pOS);//输出当前变量

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
					*pOS << TEXT("无法计算。\r\n");
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
					*pOS << TEXT("无法计算。\r\n");
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
				//矩阵奇异
				if (pOS != NULL) *pOS << TEXT("Jacobian矩阵奇异且无解（初值不合适或者存在矛盾方程）。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			case ERROR_INDETERMINATE_EQUATION:
				if (pOS != NULL) *pOS << TEXT("不定方程组。返回一组特解。\r\n");
				break;
			case ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW:
				if (pOS != NULL) *pOS << TEXT("Jacobian矩阵与Phi向量行数不等，程序出错。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			case ERROR_INFINITY_SOLUTIONS:
				if (pOS != NULL) *pOS << TEXT("Jacobian矩阵奇异，但有无穷多解（存在等价方程）。返回一组特解。\r\n");
				break;
			case ERROR_OVER_DETERMINED_EQUATIONS:
				if (pOS != NULL) *pOS << TEXT("矛盾方程组，无法求解。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}

			if (pOS != NULL)//输出DeltaQ
			{
				*pOS << TEXT("Δq(");
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
				if (pOS != NULL) *pOS << TEXT("超过") << max_step << TEXT("步仍未收敛。\r\n");
				VariableTableUnsolved.VariableValue = VariableValueBackup;
				return;
			}
			n++;
		}
		//此处已解出

		VariableTable.SetValueByVarTable(VariableTableUnsolved);

		hasSolved = true;
	}

	if (pOS != NULL) *pOS << TEXT("\r\n得到结果：\r\n");
	VariableTableUnsolved.OutputValue(pOS);
}


void TEquations::SimplifyEquations(Ostream *pOS)//将方程组中的简单方程解出
{
	std::vector<bool> vecHasSolved(Equations.size(), false);
	//for (auto pExpr : Equations)
	for (size_t i = 0; i < Equations.size(); ++i)
	{
		if (vecHasSolved[i] == false)
		{
			TExpressionTree *pExpr = Equations[i];

			//代入
			pExpr->Subs(VariableTableSolved.VariableTable, VariableTableSolved.VariableValue, pOS != NULL);

			if (pExpr->CheckOnlyOneVar())
			{
				String var;
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
	for (auto &pVar : VariableTableSolved.VariableTable)
	{
		VariableTableUnsolved.RemoveOne(pOS,pVar,true);
	}

	if (pOS != NULL) *pOS << TEXT(">>Simplify:\r\n\r\n");

	//清理掉已解出方程
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
		hasSolved = true;//false由AddEquation触发

	if (pOS != NULL)
	{
		*pOS << TEXT("\r\n解得：\r\n");
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