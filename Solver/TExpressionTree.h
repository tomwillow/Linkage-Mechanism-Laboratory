#pragma once

#include "..\Common\tchar_head.h"
#include <queue>
#include <stack>
#include "..\Solver\TVariableTable.h"
#include "..\enumError.h"

class TExpressionTree
{
private:
#define MAX_VAR_NAME 32//同时也是浮点数转字符串的最大长度
#define MIN_DOUBLE  1e-6
	enum enumMathOperator{
		MATH_NOT_AVAILIALBE,
		//一元
		MATH_POSITIVE, MATH_NEGATIVE,

		//函数
		MATH_SIN, MATH_COS, MATH_TAN,
		MATH_ARCSIN, MATH_ARCCOS, MATH_ARCTAN, MATH_SQRT,
		MATH_LN, MATH_LOG10, MATH_EXP,

		//二元
		MATH_ADD, MATH_SUBSTRACT,
		MATH_MULTIPLY, MATH_DIVIDE,
		MATH_POWER, MATH_AND, MATH_OR, MATH_MOD,

		MATH_LEFT_PARENTHESIS, MATH_RIGHT_PARENTHESIS
	};

	enum enumNodeType{ NODE_NUMBER, NODE_OPERATOR, NODE_VARIABLE, NODE_FUNCTION };

	/* 单个元素 */
	struct TNode
	{
		enumNodeType eType;
		enumMathOperator eOperator;
		double value;
		String varname;
		TNode *parent;
		TNode *left, *right;
	};

	void TExpressionTree::Release();

	//运算符性质函数
	int TExpressionTree::GetOperateNum(TExpressionTree::enumMathOperator eOperator);
	static String TExpressionTree::EnumOperatorToTChar(TExpressionTree::enumMathOperator eOperator);
	bool TExpressionTree::isLeft2Right(enumMathOperator eOperator);
	bool TExpressionTree::inAssociativeLaws(enumMathOperator eOperator);
	int TExpressionTree::Rank(enumMathOperator eOperator);

	bool TExpressionTree::isBaseOperator(TCHAR c);
	bool TExpressionTree::isDoubleChar(TCHAR c);
	bool TExpressionTree::isLegal(TCHAR c);

	static TExpressionTree::enumMathOperator TExpressionTree::BaseOperatorCharToEnum(TCHAR c);
	static TExpressionTree::enumMathOperator TExpressionTree::Str2Function(String s);
	static String TExpressionTree::Function2Str(TExpressionTree::enumMathOperator eOperator);

	TVariableTable *pVariableTable;
	int iVarAppearedCount;
	TNode *LastVarNode;
	//TVariableTable SelfVariableTable;
	bool TExpressionTree::IsIntAndEven(double n);
	void TExpressionTree::InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder);
	void TExpressionTree::ReadToInOrder(String expression, std::queue<TNode *> &InOrder);
	static String TExpressionTree::Node2Str(const TNode &node);
	void TExpressionTree::BuildExpressionTree(std::vector<TNode *> &PostOrder);
	void TExpressionTree::TraverseInOrder(TNode *now, String &output);
	void TExpressionTree::CalcNode(TNode *Operator, const TNode *Node1, const TNode *Node2);//计算节点值，支持所有运算符和函数，结果存进Operator
	void TExpressionTree::Simplify(TNode *now);
	void TExpressionTree::GetNodeNum(TNode *now, int &n);
	int TExpressionTree::GetNodeNum(TNode *head);
	void TExpressionTree::DeleteNode(TNode *node);
	void TExpressionTree::DeleteNodeTraversal(TNode *node);
	void TExpressionTree::Diff(TNode *now, String var);
	TExpressionTree::TNode * TExpressionTree::CopyNodeTree(TNode *oldNode);
	//String  TExpressionTree::FindVariableTableFrom(const String varstr, std::vector<String > newVariableTable);//查找变量是否在变量表中，没有则返回NULL
	void TExpressionTree::GetVariablePos(TNode *now, const String var, std::vector<TNode *> &VarsPos);
	void TExpressionTree::GetVariablePos(const String var, std::vector<TNode *> &VarsPos);
	void TExpressionTree::CopyVariableTable(std::vector<String > &Dest, const std::vector<String > source);
	//void TExpressionTree::ReplaceNodeVariable(TNode *now, std::vector<String > &newVariableTable);
	bool TExpressionTree::CanCalc(TNode *now);
	void TExpressionTree::Calc(TNode *now);
	void TExpressionTree::LinkParent(TNode *child, TNode *ignore);
	TExpressionTree::TNode * TExpressionTree::NewNode(enumNodeType eType, enumMathOperator eOperator = MATH_NOT_AVAILIALBE);
	void TExpressionTree::ReleaseVectorTNode(std::vector<TNode *> vec);
	void TExpressionTree::Vpa_inner(TNode *now);
	void TExpressionTree::Solve(TNode *now, TNode *&write_pos);
	void TExpressionTree::CheckOnlyOneVar(TNode *now);
	void TExpressionTree::Subs_inner(TNode *node, String ptVar, double value);
public:
	TNode *head;
	void TExpressionTree::Reset();
	void  TExpressionTree::Vpa(bool bOutput);
	void  TExpressionTree::LinkVariableTable(TVariableTable *p);//链接变量表
	void  TExpressionTree::Read(const String expression, bool bOutput);
	void  TExpressionTree::Read(double num, bool bOutput);//读入只有1个数字的表达式

	//所有操作符未完成
	String  TExpressionTree::Solve(String &var, double &value);//求解单变量方程 不验证可求解性，需提前调用HasOnlyOneVar确认 不改动表达式内容
	String  TExpressionTree::OutputStr();
	void  TExpressionTree::Simplify(bool bOutput);//化简
	String  TExpressionTree::Diff(String var, int n, bool bOutput);//对变量求导
	void  TExpressionTree::Subs(String ptVar, double value, bool output);
	void  TExpressionTree::Subs(const String vars,const String nums, bool output);//vars为被替换变量，nums为替换表达式，以空格分隔
	void  TExpressionTree::Subs(std::vector<String > VarsVector, std::vector<double> NumsVector, bool output);
	bool TExpressionTree::CanCalc();//检查是否还有变量存在，可以计算则返回true
	bool TExpressionTree::IsSingleVar();//检查是否为一元(not used)
	bool TExpressionTree::HasOnlyOneVar();//只有一个变量（只有刚read才有效）
	bool TExpressionTree::CheckOnlyOneVar();//只有一个变量（实时验证）
	double TExpressionTree::Value(bool operateHeadNode);//不验证可计算性，必须与CanCalc合用
	String  TExpressionTree::Calc(double *result = NULL);//计算表达式的值，若传入了result则把结果存入。返回值为结果字符串或表达式串。

	TExpressionTree& TExpressionTree::operator=(const TExpressionTree &expr);
	TExpressionTree& TExpressionTree::operator+(const TExpressionTree &expr);
	TExpressionTree & TExpressionTree::operator*(double value);
	TExpressionTree & TExpressionTree::operator+(double value);
	TExpressionTree & TExpressionTree::operator-(double value);

	TExpressionTree();
	~TExpressionTree();
};
