#pragma once

#include <tchar.h>
#include <queue>
#include <stack>

class TExpressionTree
{
private:
#define MAX_VAR_NAME 32//同时也是浮点数转字符串的最大长度
#define MIN_DOUBLE  1e-6
	enum enumError{
		ERROR_NO, 
		ERROR_ILLEGALCHAR,//出现非法字符
		ERROR_PARENTHESIS_NOT_MATCH,//括号不匹配
		ERROR_INVALID_VARNAME,//不正确的变量名
		ERROR_EMPTY_INPUT,
		ERROR_DIVIDE_ZERO,
		ERROR_UNKNOWN_VARIABLE,
		ERROR_ZERO_POWEROF_ZERO,
		ERROR_SUBS_NOT_EQUAL
	} eError;
	enum enumMathOperator{
		MATH_NOT_AVAILIALBE,
		MATH_POSITIVE, MATH_NEGATIVE,
		MATH_SIN, MATH_COS, MATH_TAN,
		MATH_ARCSIN, MATH_ARCCOS, MATH_ARCTAN,MATH_SQRT,
		MATH_LN,MATH_LOG10,MATH_EXP,
		MATH_POWER,MATH_AND,MATH_OR,MATH_MOD,
		MATH_ADD, MATH_SUBSTRACT, 
		MATH_MULTIPLY, MATH_DIVIDE,
		MATH_LEFT_PARENTHESIS, MATH_RIGHT_PARENTHESIS
	};

	enum enumNodeType{NODE_NUMBER,NODE_OPERATOR,NODE_VARIABLE,NODE_FUNCTION};

	/* 单个元素 */
	struct TNode
	{
		enumNodeType eType;
		enumMathOperator eOperator;
		union {
			double value;//数字的值
			TCHAR *varname;//变量名
		}ValueOrName;
		//int op_num;//操作数数量 eg. 一元，二元运算符
		TNode *parent;
		TNode *left, *right;
	};

	void TExpressionTree::Release();

	TCHAR *ErrorInfo;
	TCHAR * TExpressionTree::GetErrorInfo(TExpressionTree::enumError eError);

	bool TExpressionTree::isAlphaCharOrUnderline(TCHAR c);
	bool TExpressionTree::isBaseOperator(TCHAR c);
	bool TExpressionTree::isDoubleChar(TCHAR c);
	bool TExpressionTree::isLegal(TCHAR c);
	int TExpressionTree::GetOperateNum(TExpressionTree::enumMathOperator eOperator);
	TExpressionTree::enumMathOperator TExpressionTree::BaseOperatorCharToEnum(TCHAR c);
	TCHAR TExpressionTree::EnumOperatorToTChar(TExpressionTree::enumMathOperator eOperator);
	bool TExpressionTree::isLeft2Right(enumMathOperator eOperator);
	TExpressionTree::enumMathOperator TExpressionTree::Str2Function(TCHAR *start, TCHAR *end);
	void TExpressionTree::Function2Str(TExpressionTree::enumMathOperator eOperator, TCHAR *buffer);

	TCHAR *szPostOrder,*szInOrder;
	std::vector<TCHAR *> VariableTable;
	int TExpressionTree::Rank(enumMathOperator eOperator);
	TExpressionTree::enumError TExpressionTree::InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder);
	TExpressionTree::enumError TExpressionTree::ReadToInOrder(TCHAR *expression, std::queue<TNode *> &InOrder);
	void TExpressionTree::Node2Str(TNode &node, TCHAR *result);
	TExpressionTree::enumError TExpressionTree::BuildExpressionTree(std::vector<TNode *> &PostOrder);
	void TExpressionTree::TraverseInOrder(TNode *now, TCHAR *output, TCHAR *buffer);
	void TExpressionTree::CalcNode(TNode *Operator, TNode *Node1, TNode *Node2);
	TExpressionTree::enumError TExpressionTree::Simplify(TNode *now);
	void TExpressionTree::GetNodeNum(TNode *now, int &n);
	int TExpressionTree::GetNodeNum(TNode *head);
	void TExpressionTree::DeleteNode(TNode *node, std::vector<TNode *> DeleteList);
	void TExpressionTree::DeleteNode(TNode *node);
	TCHAR * TExpressionTree::OutputEmptyStr();
	void TExpressionTree::Diff(TNode *now, TCHAR *var);
	TExpressionTree::TNode * TExpressionTree::CopyNodeTree(TNode *oldNode);
	TCHAR * TExpressionTree::FindVariableTable(TCHAR *varstr);//查找变量是否在变量表中，没有则返回NULL
	TCHAR * TExpressionTree::FindVariableTableFrom(TCHAR *varstr,std::vector<TCHAR *> newVariableTable);//查找变量是否在变量表中，没有则返回NULL
	void TExpressionTree::GetVariablePos(TNode *now, const TCHAR *var, std::vector<TNode *> &VarsPos);
	void TExpressionTree::GetVariablePos(const TCHAR *var, std::vector<TNode *> &VarsPos);
	void TExpressionTree::CopyVariableTable(std::vector<TCHAR *> &Dest, const std::vector<TCHAR *> source);
	void TExpressionTree::ReplaceNodeVariable(TNode *now, std::vector<TCHAR *> &newVariableTable);
public:
	TNode *head;
	TCHAR * TExpressionTree::Read(TCHAR *expression, bool bOutput);
	TCHAR * TExpressionTree::OutputStr();//输出表达式
	TCHAR * TExpressionTree::OutputPostOrderStr();//输出PostOrder表达式
	TCHAR * TExpressionTree::Simplify(bool bOutput);//化简
	TCHAR * TExpressionTree::Diff(TCHAR *var, int n, bool bOutput);//对变量求导
	TCHAR * TExpressionTree::Subs(TCHAR *vars, TCHAR *nums);//vars为被替换变量，nums为替换表达式，以逗号分隔
	bool TExpressionTree::CanCalc();
	TCHAR * TExpressionTree::Calc(double *result);

	TExpressionTree();
	~TExpressionTree();
};

//替换全部 src源字符串，sub被替换内容，dest替换的内容
//返回替换过的数量
//src大小需自行保证，函数内不验证指针越界
int Replace(TCHAR *src, TCHAR *sub, TCHAR *dest);

void Split(TCHAR *src, std::vector<TCHAR *> &result, TCHAR *sub);