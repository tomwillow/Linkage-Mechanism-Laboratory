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
		ERROR_EMPTY_INPUT
	} eError;
	enum enumMathOperator{
		MATH_NOT_AVAILIALBE,
		MATH_POSITIVE, MATH_NEGATIVE,
		MATH_SIN, MATH_COS, MATH_TAN,
		MATH_ARCSIN, MATH_ARCCOS, MATH_ARCTAN,MATH_SQRT,
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
	std::queue<TNode *> InOrder;// , PostOrder;
	std::vector<TNode *> PostOrder;
	TNode *head;
	int TExpressionTree::Rank(enumMathOperator eOperator);
	enumError TExpressionTree::InQueue2PostQueue();
	TExpressionTree::enumError TExpressionTree::ReadToInOrder(TCHAR *expression);
	void TExpressionTree::Node2Str(TNode &node, TCHAR *result);
	TExpressionTree::enumError TExpressionTree::BuildExpressionTree();
	void TExpressionTree::TraverseInOrder(TNode *now, TCHAR *output, TCHAR *buffer);
	void TExpressionTree::CalcNode(TNode *Operator, TNode *Node1, TNode *Node2);
	void TExpressionTree::Simplify(TNode *now);
public:
	TCHAR * TExpressionTree::Read(TCHAR *expression);//返回0为出错，出错信息保存在ErrorInfo中。
	TCHAR * TExpressionTree::OutputStr();//输出表达式
	TCHAR * TExpressionTree::OutputPostOrderStr();//输出PostOrder表达式
	TCHAR * TExpressionTree::Simplify();//化简
	//void TExpressionTree::Output(TCHAR *buffer){};

	TExpressionTree();
	~TExpressionTree();
};

//替换全部 src源字符串，sub被替换内容，dest替换的内容
//返回替换过的数量
//src大小需自行保证，函数内不验证指针越界
int Replace(TCHAR *src, TCHAR *sub, TCHAR *dest);