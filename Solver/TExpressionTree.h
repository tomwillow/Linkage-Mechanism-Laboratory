#pragma once

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
		MATH_NULL,
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

	enum enumNodeType{ NODE_NULL, NODE_NUMBER, NODE_OPERATOR, NODE_VARIABLE, NODE_FUNCTION };

	/* 单个元素 */
	struct TNode
	{
		enumNodeType eType;
		enumMathOperator eOperator;
		double value;
		String varname;
		TNode *parent;
		TNode *left, *right;
		TNode() :eType(NODE_NULL), eOperator(MATH_NULL), value(0), parent(nullptr), left(nullptr), right(nullptr) {}
	};

	void Release();

	//运算符性质函数
	int GetOperateNum(enumMathOperator eOperator);
	static String EnumOperatorToTChar(enumMathOperator eOperator);
	bool isLeft2Right(enumMathOperator eOperator);
	bool inAssociativeLaws(enumMathOperator eOperator);
	int Rank(enumMathOperator eOperator);

	bool isBaseOperator(TCHAR c);
	bool isDoubleChar(TCHAR c);
	bool isLegal(TCHAR c);

	static enumMathOperator BaseOperatorCharToEnum(TCHAR c);
	static enumMathOperator Str2Function(String s);
	static String Function2Str(enumMathOperator eOperator);

	TVariableTable *pVariableTable;
	int iVarAppearedCount;
	TNode *LastVarNode;
	//TVariableTable SelfVariableTable;
	bool IsIntAndEven(double n);
	void InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder);
	void ReadToInOrder(String expression, std::queue<TNode *> &InOrder);
	static String Node2Str(const TNode &node);
	void BuildExpressionTree(std::vector<TNode *> &PostOrder);
	void TraverseInOrder(TNode *now, String &output);
	void CalcNode(TNode *Operator, const TNode *Node1, const TNode *Node2);//计算节点值，支持所有运算符和函数，结果存进Operator
	void Simplify(TNode *now);
	void GetNodeNum(TNode *now, int &n);
	int GetNodeNum(TNode *head);
	void DeleteNode(TNode *node);
	void DeleteNodeTraversal(TNode *node);
	void Diff(TNode *now, String var);
	TNode * CopyNodeTree(TNode *oldNode);
	//String  FindVariableTableFrom(const String varstr, std::vector<String > newVariableTable);//查找变量是否在变量表中，没有则返回NULL
	void GetVariablePos(TNode *now, const String var, std::vector<TNode *> &VarsPos);
	void GetVariablePos(const String var, std::vector<TNode *> &VarsPos);
	void CopyVariableTable(std::vector<String > &Dest, const std::vector<String > source);
	//void ReplaceNodeVariable(TNode *now, std::vector<String > &newVariableTable);
	bool CanCalc(TNode *now);
	void Calc(TNode *now);
	void LinkParent(TNode *child, TNode *ignore);
	TNode * NewNode(enumNodeType eType, enumMathOperator eOperator = MATH_NULL);
	void ReleaseVectorTNode(std::vector<TNode *> vec);
	void Vpa_inner(TNode *now);
	void Solve(TNode *now, TNode *&write_pos);
	void CheckOnlyOneVar(TNode *now);
	void Subs_inner(TNode *node, String ptVar, double value);
public:
	TNode *head;
	void Reset();
	void  Vpa(bool bOutput);
	void  LinkVariableTable(TVariableTable *p);//链接变量表
	void  Read(const String expression, bool bOutput);
	void  Read(double num, bool bOutput);//读入只有1个数字的表达式

	//所有操作符未完成
	String  Solve(String &var, double &value);//求解单变量方程 不验证可求解性，需提前调用HasOnlyOneVar确认 不改动表达式内容
	String  OutputStr();
	void  Simplify(bool bOutput);//化简
	String  Diff(String var, int n, bool bOutput);//对变量求导
	void  Subs(String ptVar, double value, bool output);
	void  Subs(const String vars,const String nums, bool output);//vars为被替换变量，nums为替换表达式，以空格分隔
	void  Subs(std::vector<String > VarsVector, std::vector<double> NumsVector, bool output);
	bool CanCalc();//检查是否还有变量存在，可以计算则返回true
	bool IsSingleVar();//检查是否为一元(not used)
	bool HasOnlyOneVar();//只有一个变量（只有刚read才有效）
	bool CheckOnlyOneVar();//只有一个变量（实时验证）
	double Value(bool operateHeadNode);//不验证可计算性，必须与CanCalc合用
	String  Calc(double *result = NULL);//计算表达式的值，若传入了result则把结果存入。返回值为结果字符串或表达式串。

	TExpressionTree& operator=(const TExpressionTree &expr);
	TExpressionTree& operator+(const TExpressionTree &expr);
	TExpressionTree & operator*(double value);
	TExpressionTree & operator+(double value);
	TExpressionTree & operator-(double value);

	TExpressionTree();
	~TExpressionTree();
};
