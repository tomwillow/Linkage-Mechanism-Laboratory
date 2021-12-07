#pragma once

#pragma warning(disable:4703)
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "..\Common\DetectMemoryLeak.h"

#include "..\Solver\TExpressionTree.h"

#include <string>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

TExpressionTree::TExpressionTree()
{
	Reset();
}


TExpressionTree::~TExpressionTree()
{
	Release();
}

void TExpressionTree::Reset()
{
	head = NULL;
	pVariableTable = NULL;

	iVarAppearedCount = 0;
}

void TExpressionTree::Release()
{
	DeleteNode(head);
	head = NULL;
}

//删除node指向对象 可删除任意位置节点，如被删节点存在父节点则父节点左右儿子置0
void TExpressionTree::DeleteNode(TNode *node)
{
	if (node != NULL)
	{
		if (node->parent != NULL)
		{
			if (node->parent->left == node)
				node->parent->left = NULL;
			if (node->parent->right == node)
				node->parent->right = NULL;
		}

		DeleteNodeTraversal(node);
	}
}

//遍历删除节点 未处理父节点，未判断左右儿子是否存在
void TExpressionTree::DeleteNodeTraversal(TNode *node)
{
	if (node->left != NULL)
		DeleteNode(node->left);
	if (node->right != NULL)
		DeleteNode(node->right);

	delete node;
}

//将PostOrder建立为树，并进行表达式有效性检验（确保二元及一元运算符、函数均有操作数）
void TExpressionTree::BuildExpressionTree(std::vector<TNode *> &PostOrder)
{
	std::stack<TNode *> tempStack;
	//逐个识别PostOrder序列，构建表达式树
	for (auto pNodeNow : PostOrder)
	{
		switch (pNodeNow->eType)
		{
		case NODE_NUMBER:
		case NODE_VARIABLE:
			tempStack.push(pNodeNow);
			break;
		case NODE_FUNCTION:
		case NODE_OPERATOR:
			if (GetOperateNum(pNodeNow->eOperator) == 2)
			{
				if (tempStack.empty())
				{
					//释放所有TNode，报错
					ReleaseVectorTNode(PostOrder);
					//Stack只是对PostOrder的重排序，所以不用delete
					throw TError{ ERROR_WRONG_EXPRESSION, TEXT("") };
					return;
				}

				pNodeNow->right = tempStack.top();
				tempStack.top()->parent = pNodeNow;
				tempStack.pop();

				if (tempStack.empty())
				{
					//释放所有TNode，报错
					ReleaseVectorTNode(PostOrder);
					//Stack只是对PostOrder的重排序，所以不用delete
					throw TError{ ERROR_WRONG_EXPRESSION, TEXT("") };
					return;
				}

				pNodeNow->left = tempStack.top();
				tempStack.top()->parent = pNodeNow;
				tempStack.pop();

				tempStack.push(pNodeNow);
			}
			else
			{
				if (tempStack.empty())
				{
					//释放所有TNode，报错
					ReleaseVectorTNode(PostOrder);
					//Stack只是对PostOrder的重排序，所以不用delete
					throw TError{ ERROR_WRONG_EXPRESSION, TEXT("") };
					return;
				}

				pNodeNow->left = tempStack.top();
				tempStack.top()->parent = pNodeNow;
				tempStack.pop();

				tempStack.push(pNodeNow);
			}
			break;
		}
	}

	//找出root
	head = PostOrder[0];
	while (head->parent != NULL)
	{
		head = head->parent;
	}
}

/* 返回运算符结合性 */
bool TExpressionTree::isLeft2Right(enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_MOD://%
	case MATH_AND://&
	case MATH_OR://|
	case MATH_MULTIPLY:
	case MATH_DIVIDE:
	case MATH_ADD:
	case MATH_SUBSTRACT:
		return true;

	case MATH_POSITIVE://正负号为右结合
	case MATH_NEGATIVE:
	case MATH_POWER://^
		return false;
		//函数和括号不计结合性
	default:
		return true;
	}
}

/* 返回运算符的优先级 */
int TExpressionTree::Rank(enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_SQRT:
	case MATH_SIN:
	case MATH_COS:
	case MATH_TAN:
	case MATH_ARCSIN:
	case MATH_ARCCOS:
	case MATH_ARCTAN:
	case MATH_LN:
	case MATH_LOG10:
	case MATH_EXP:
		return 15;

	case MATH_POSITIVE://除了函数，所有运算符均可将正负号挤出
	case MATH_NEGATIVE:
		return 14;

	case MATH_MOD://%
		return 13;

	case MATH_AND://&
	case MATH_OR://|
		return 12;

	case MATH_POWER://^
		return 11;

	case MATH_MULTIPLY:
	case MATH_DIVIDE:
		return 10;

	case MATH_ADD:
	case MATH_SUBSTRACT:
		return 5;

	case MATH_LEFT_PARENTHESIS://左右括号优先级小是为了不被其余任何运算符挤出
	case MATH_RIGHT_PARENTHESIS:
		return 0;
	default:
		throw TError{ ERROR_WRONG_MATH_OPERATOR, String(TEXT("VALUE:")) + To_string(eOperator) };
	}
}

int TExpressionTree::GetOperateNum(TExpressionTree::enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_SQRT:
	case MATH_SIN:
	case MATH_COS:
	case MATH_TAN:
	case MATH_ARCSIN:
	case MATH_ARCCOS:
	case MATH_ARCTAN:
	case MATH_LN:
	case MATH_LOG10:
	case MATH_EXP:

	case MATH_POSITIVE://正负号
	case MATH_NEGATIVE:
		return 1;

	case MATH_MOD://%
	case MATH_AND://&
	case MATH_OR://|
	case MATH_POWER://^
	case MATH_MULTIPLY:
	case MATH_DIVIDE:
	case MATH_ADD:
	case MATH_SUBSTRACT:
		return 2;

	case MATH_LEFT_PARENTHESIS:
	case MATH_RIGHT_PARENTHESIS:
		return 0;
	default:
		return 0;
	}
}

bool TExpressionTree::inAssociativeLaws(enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_SQRT:
	case MATH_SIN:
	case MATH_COS:
	case MATH_TAN:
	case MATH_ARCSIN:
	case MATH_ARCCOS:
	case MATH_ARCTAN:
	case MATH_LN:
	case MATH_LOG10:
	case MATH_EXP:

	case MATH_POSITIVE://正负号
	case MATH_NEGATIVE:

	case MATH_MOD://%
	case MATH_AND://&
	case MATH_OR://|
	case MATH_POWER://^
	case MATH_DIVIDE:
	case MATH_SUBSTRACT:

	case MATH_LEFT_PARENTHESIS:
	case MATH_RIGHT_PARENTHESIS:
		return false;

	case MATH_ADD:
	case MATH_MULTIPLY:
		return true;
	}
	assert(0);
	return false;
}

/* 是基本运算符()+-* /^&|% */
bool TExpressionTree::isBaseOperator(TCHAR c)
{
	switch (c)
	{
	case TEXT('('):
	case TEXT(')'):
	case TEXT('+'):
	case TEXT('-'):
	case TEXT('*'):
	case TEXT('/'):
	case TEXT('^'):
	case TEXT('&'):
	case TEXT('|'):
	case TEXT('%'):return true;
	}
	return false;
}

TExpressionTree::enumMathOperator TExpressionTree::Str2Function(String s)
{
	if (s == TEXT("sin"))
	{
		return MATH_SIN;
	}
	if (s == TEXT("cos"))
	{
		return MATH_COS;
	}
	if (s == TEXT("tan"))
	{
		return MATH_TAN;
	}
	if (s == TEXT("arcsin"))
	{
		return MATH_ARCSIN;
	}
	if (s == TEXT("arccos"))
	{
		return MATH_ARCCOS;
	}
	if (s == TEXT("arctan"))
	{
		return MATH_ARCTAN;
	}
	if (s == TEXT("sqrt"))
	{
		return MATH_SQRT;
	}
	if (s == TEXT("ln"))
	{
		return MATH_LN;
	}
	if (s == TEXT("log10"))
	{
		return MATH_LOG10;
	}
	if (s == TEXT("exp"))
	{
		return MATH_EXP;
	}
	return MATH_NULL;
}

String TExpressionTree::Function2Str(TExpressionTree::enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_SIN:
		return TEXT("sin");
	case MATH_COS:
		return TEXT("cos");
	case MATH_TAN:
		return TEXT("tan");
	case MATH_ARCSIN:
		return TEXT("arcsin");
	case MATH_ARCCOS:
		return TEXT("arccos");
	case MATH_ARCTAN:
		return TEXT("arctan");
	case MATH_SQRT:
		return TEXT("sqrt");
	case MATH_LN:
		return TEXT("ln");
	case MATH_LOG10:
		return TEXT("log10");
	case MATH_EXP:
		return TEXT("exp");
	}
	throw TError{ ERROR_WRONG_MATH_OPERATOR, String(TEXT("value=")) + To_string(eOperator) };
}


/*  */
TExpressionTree::enumMathOperator TExpressionTree::BaseOperatorCharToEnum(TCHAR c)
{
	switch (c)
	{
	case TEXT('('):
		return MATH_LEFT_PARENTHESIS;
	case TEXT(')'):
		return MATH_RIGHT_PARENTHESIS;
	case TEXT('+'):
		return MATH_ADD;
	case TEXT('-'):
		return MATH_SUBSTRACT;
	case TEXT('*'):
		return MATH_MULTIPLY;
	case TEXT('/'):
		return MATH_DIVIDE;
	case TEXT('^'):
		return MATH_POWER;
	case TEXT('&'):
		return MATH_AND;
	case TEXT('|'):
		return MATH_OR;
	case TEXT('%'):
		return MATH_MOD;
	default:
		return MATH_NULL;
	}
}
/*  */
String TExpressionTree::EnumOperatorToTChar(TExpressionTree::enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_POSITIVE:
		return String(TEXT("+"));
	case MATH_NEGATIVE:
		return String(TEXT("-"));
	case MATH_LEFT_PARENTHESIS:
		return String(TEXT("("));
	case MATH_RIGHT_PARENTHESIS:
		return String(TEXT(")"));
	case MATH_ADD:
		return String(TEXT("+"));
	case MATH_SUBSTRACT:
		return String(TEXT("-"));
	case MATH_MULTIPLY:
		return String(TEXT("*"));
	case MATH_DIVIDE:
		return String(TEXT("/"));
	case MATH_POWER:
		return String(TEXT("^"));
	case MATH_AND:
		return String(TEXT("&"));
	case MATH_OR:
		return String(TEXT("|"));
	case MATH_MOD:
		return String(TEXT("%"));
	default:
		throw TError{ ERROR_WRONG_MATH_OPERATOR, String(TEXT("value:")) + To_string(eOperator) };
	}
}

/* 有效性检查（返回0则出现异常字符） */
bool TExpressionTree::isLegal(TCHAR c)
{
	if (isDoubleChar(c)) return true;
	if (isBaseOperator(c)) return true;
	if (IsCharAlpha(c) || c == TEXT('_')) return true;
	return false;
}


/* 字符是0-9或. */
bool TExpressionTree::isDoubleChar(TCHAR c)
{
	if ((c >= TEXT('0') && c <= TEXT('9')) || c == TEXT('.'))
		return true;
	else
		return false;
}


/*由in order队列得到post order队列*/
void TExpressionTree::InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder)
{
	int parenthesis_num = 0;
	std::stack<TNode *> temp;
	while (InOrder.size() > 0)
	{
		if (InOrder.front()->eType == NODE_NUMBER || InOrder.front()->eType == NODE_VARIABLE)
		{
			PostOrder.push_back(InOrder.front());//数字直接入栈
			InOrder.pop();
		}
		else
		{
			if (InOrder.front()->eOperator == MATH_LEFT_PARENTHESIS) //(左括号直接入栈
			{
				temp.push(InOrder.front());
				InOrder.pop();
				parenthesis_num++;
			}
			else
			{
				if (InOrder.front()->eOperator == MATH_RIGHT_PARENTHESIS)//)出现右括号
				{
					parenthesis_num--;
					//pop至左括号
					while (temp.size() > 0)
					{
						if (temp.top()->eOperator == MATH_LEFT_PARENTHESIS)//(
						{
							delete temp.top();
							temp.pop();//扔掉左括号
							break;
						}
						else
						{
							PostOrder.push_back(temp.top());//入队
							temp.pop();
						}
					}

					//取出函数
					if (temp.size() > 0 && temp.top()->eType == NODE_FUNCTION)
					{
						PostOrder.push_back(temp.top());
						temp.pop();
					}

					//pop所有取正取负
					while (temp.size() > 0)
					{
						if (temp.top()->eOperator == MATH_POSITIVE || temp.top()->eOperator == MATH_NEGATIVE)
						{
							PostOrder.push_back(temp.top());
							temp.pop();
						}
						else
							break;
					}
					delete InOrder.front();
					InOrder.pop();//扔掉右括号
				}
				else//InOrder.front()不是括号
				{
					if (InOrder.front()->eOperator == MATH_POSITIVE || InOrder.front()->eOperator == MATH_NEGATIVE)
					{
						temp.push(InOrder.front());
						InOrder.pop();
					}
					else//不是括号也不是正负号
					{
						if (temp.size() > 0 && isLeft2Right(temp.top()->eOperator) == true)//左结合
							while (temp.size() > 0 && Rank(InOrder.front()->eOperator) <= Rank(temp.top()->eOperator))//临时栈有内容，且新进符号优先级低，则挤出高优先级及同优先级符号
							{
								PostOrder.push_back(temp.top());//符号进入post队列
								temp.pop();
							}
						else//右结合
							while (temp.size() > 0 && Rank(InOrder.front()->eOperator) < Rank(temp.top()->eOperator))//临时栈有内容，且新进符号优先级低，则挤出高优先级，但不挤出同优先级符号（因为右结合）
							{
								PostOrder.push_back(temp.top());//符号进入post队列
								temp.pop();
							};
						temp.push(InOrder.front());//高优先级已全部挤出，当前符号入栈
						InOrder.pop();
					}
				}

			}
		}
	}

	//剩下的元素全部入栈
	while (temp.size() > 0)
	{
		PostOrder.push_back(temp.top());
		temp.pop();
	}
	if (parenthesis_num != 0)
	{
		ReleaseVectorTNode(PostOrder);
		throw TError{ ERROR_PARENTHESIS_NOT_MATCH, TEXT("") };
	}
}

String TExpressionTree::Node2Str(const TNode &node)
{
	switch (node.eType)
	{
	case NODE_NUMBER:
		if (abs(node.value - (long long)node.value) < MIN_DOUBLE)
			return To_string((long long)node.value);
		else
			return To_string(node.value);
		break;
	case NODE_VARIABLE:
		return node.varname;
		break;
	case NODE_FUNCTION:
		return Function2Str(node.eOperator);
		break;
	case NODE_OPERATOR:
		return EnumOperatorToTChar(node.eOperator);
		break;
	}
	assert(0);
	return TEXT("Error");
}

void TExpressionTree::TraverseInOrder(TNode *now, String &output)
{
	int has_parenthesis = 0;
	if (GetOperateNum(now->eOperator) == 1)//一元运算符：函数和取负
	{
		if (now->eType == NODE_FUNCTION)
		{
			output += Node2Str(*now) + TEXT("(");
			has_parenthesis = 1;
		}
		else
		{
			output += TEXT("(") + Node2Str(*now);
			has_parenthesis = 1;
		}
	}

	if (GetOperateNum(now->eOperator) != 1)//非一元运算符才输出，即一元运算符的输出顺序已改变
	{
		if (now->eType == NODE_OPERATOR)//本级为运算符
			if (now->parent != NULL)
				if (
					(GetOperateNum(now->parent->eOperator) == 2 &&//父运算符存在，为二元，
						(
							Rank(now->parent->eOperator) > Rank(now->eOperator)//父级优先级高于本级->加括号
							||
							(//两级优先级相等
								Rank(now->parent->eOperator) == Rank(now->eOperator) &&
								(
									//本级为父级的右子树 且父级不满足结合律->加括号
									(inAssociativeLaws(now->parent->eOperator) == false && now == now->parent->right)
									||
									////两级都是右结合
									(isLeft2Right(now->parent->eOperator) == false && isLeft2Right(now->eOperator) == false)
									)
								)
							)
						)

					//||

					////父运算符存在，为除号，且本级为分子，则添加括号
					//(now->parent->eOperator == MATH_DIVIDE && now == now->parent->right)
					)
				{
					output += TEXT("(");
					has_parenthesis = 1;
				}
	}

	if (now->left != NULL)//左遍历
	{
		TraverseInOrder(now->left, output);
	}

	if (GetOperateNum(now->eOperator) != 1)//非一元运算符才输出，即一元运算符的输出顺序已改变
	{
		output += Node2Str(*now);
	}


	if (now->right != NULL)//右遍历
	{
		TraverseInOrder(now->right, output);
	}

	//回到本级时补齐右括号，包住前面的东西
	if (has_parenthesis)
	{
		output += TEXT(")");
	}
}

void TExpressionTree::GetNodeNum(TNode *now, int &n)
{
	if (now->left != NULL)
		GetNodeNum(now->left, n);
	if (now->right != NULL)
		GetNodeNum(now->right, n);
	n++;
}

int TExpressionTree::GetNodeNum(TNode *head)
{
	int num = 0;
	if (head != 0)
	{
		GetNodeNum(head, num);
		return num;
	}
	else
		return 0;
}

String  TExpressionTree::OutputStr()
{
	String temp;

	if (head != NULL)
		TraverseInOrder(head, temp);
	return temp;
}

void TExpressionTree::ReadToInOrder(String expression, std::queue<TNode *> &InOrder)
{
	if (expression.empty())
	{
		throw TError{ ERROR_EMPTY_INPUT, expression };
		return;
	}
	Replace(expression, TEXT(" "), TEXT(""));
	Replace(expression, TEXT("\t"), TEXT(""));
	Replace(expression, TEXT("\r"), TEXT(""));
	Replace(expression, TEXT("\n"), TEXT(""));

	//过滤掉所有多余的加减
	Replace(expression, TEXT("--"), TEXT("+"));
	Replace(expression, TEXT("+-"), TEXT("-"));
	Replace(expression, TEXT("-+"), TEXT("-"));

	//字符合法性检查
	for (auto c : expression)
		if (!isLegal(c))
		{
			throw TError{ ERROR_ILLEGALCHAR, String(TEXT("WRONG CHAR:")) + To_string(c) };
		}

	//粗切分：利用operator切分
	struct TStrPiece
	{
		bool bBaseOperator;
		String s;
		TStrPiece(bool bIn, String sIn) :bBaseOperator(bIn), s(sIn) {}
	};
	std::vector<TStrPiece> Data;

	String temp;
	for (auto c : expression)
	{
		if (!isBaseOperator(c))
		{
			temp.push_back(c);
		}
		else
		{
			if (!temp.empty())
			{
				Data.emplace_back(false, temp);
				temp.clear();
			}
			Data.emplace_back(true, String{ c });
		}
	}
	if (!temp.empty())
	{
		Data.emplace_back(false, temp);
		temp.clear();
	}


	//二次切分：切分出4类元素
	//并送入Pre In order
	std::vector<TNode *> PreInOrder;
	TNode *tempNode;
	//string tempTChar;
	enumMathOperator tempeOperator;
	for (UINT i = 0; i < Data.size(); i++)
	{
		if (Data[i].bBaseOperator)//识别出基本运算符（括号也在其中）
		{
			tempNode = new TNode;
			tempNode->eType = NODE_OPERATOR;
			tempNode->eOperator = BaseOperatorCharToEnum(Data[i].s[0]);
			PreInOrder.push_back(tempNode);
		}
		else//
		{
			//逐位检验是否为数字
			bool isDouble = true;
			for (auto c : Data[i].s)
				if (isDoubleChar(c) == false)
				{
					isDouble = false;
					break;
				}

			if (isDouble)//数字
			{
				tempNode = new TNode;
				tempNode->eType = NODE_NUMBER;
				tempNode->value = std::stod(Data[i].s);
				PreInOrder.push_back(tempNode);
			}
			else
			{
				if ((tempeOperator = Str2Function(Data[i].s)) != MATH_NULL)//识别出函数
				{
					tempNode = new TNode;
					tempNode->eType = NODE_FUNCTION;
					tempNode->eOperator = tempeOperator;
					PreInOrder.push_back(tempNode);
				}
				else//变量
				{
					//非运算符、数字、函数

					if (pVariableTable == NULL)
					{
						ReleaseVectorTNode(PreInOrder);
						throw TError{ ERROR_NOT_LINK_VARIABLETABLE, TEXT("") };
						return;
					}
					if (!IsCharAlpha(Data[i].s[0]) && Data[i].s[0] != TEXT('_'))//变量名首字符需为下划线或字母
					{
						ReleaseVectorTNode(PreInOrder);
						throw TError{ ERROR_INVALID_VARNAME, Data[i].s };
						return;
					}


					//
					if (pVariableTable->FindVariableTable(Data[i].s) == pVariableTable->VariableTable.end())
					{
						ReleaseVectorTNode(PreInOrder);
						throw TError{ ERROR_UNDEFINED_VARIABLE, Data[i].s };
						return;
					}

					tempNode = new TNode;
					tempNode->eType = NODE_VARIABLE;
					tempNode->varname = Data[i].s;
					PreInOrder.push_back(tempNode);

					////得到自身的变量表 以解方程
					//if (SelfVariableTable.FindVariableTable(tempTChar) == NULL)
					//{
					//	SelfVariableTable.VariableTable.push_back(tempTChar);
					//	SelfVariableTable.VariableValue.push_back(pVariableTable->GetValueFromVarPoint(tempTChar));
					//}
					iVarAppearedCount++;
					LastVarNode = tempNode;

				}
			}
		}
	}
	//此时4大元素均已切分入PreInOrder

	//识别取正运算符与取负运算符
	bool bFirstOrParenFirst = false;
	bool bAferOneOperator = false;
	size_t i = 0;
	if (PreInOrder[0]->eOperator == MATH_ADD)
	{
		PreInOrder[0]->eOperator = MATH_POSITIVE;
		i++;
	}
	if (PreInOrder[0]->eOperator == MATH_SUBSTRACT)
	{
		PreInOrder[0]->eOperator = MATH_NEGATIVE;
		i++;
	}
	for (; i < PreInOrder.size();)
	{
		if (PreInOrder[i]->eType == NODE_OPERATOR && PreInOrder[i]->eOperator != MATH_RIGHT_PARENTHESIS)
		{
			if (i + 1 < PreInOrder.size())
				i++;
			else
				break;
			if (PreInOrder[i]->eOperator == MATH_ADD)
			{
				PreInOrder[i]->eOperator = MATH_POSITIVE;
				i++;
				continue;
			}
			if (PreInOrder[i]->eOperator == MATH_SUBSTRACT)
			{
				PreInOrder[i]->eOperator = MATH_NEGATIVE;
				i++;
				continue;
			}
		}
		else
			i++;
	}

	for (auto pNode : PreInOrder)
	{
		InOrder.push(pNode);
	}
}


//是整数 且 为偶数
bool TExpressionTree::IsIntAndEven(double n)
{
	long long i = (long long)n;
	if (abs(n - i) < MIN_DOUBLE)
		if (i % 2 == 0)
			return true;
	return false;
}

void TExpressionTree::CalcNode(TNode *Operator, const TNode *Node1, const TNode *Node2 = NULL)
{
	double value1 = Node1->value;
	double value2 = (Node2 != NULL ? Node2->value : 0.0);
	Operator->eType = NODE_NUMBER;
	switch (Operator->eOperator)
	{
	case MATH_SQRT:
		if (value1 < 0.0)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw TError{ ERROR_I, String(TEXT("sqrt(")) + To_string(value1) + String(TEXT(")")) };
			return;
		}
		Operator->value = sqrt(value1);
		break;
	case MATH_SIN:
		Operator->value = sin(value1);
		break;
	case MATH_COS:
		Operator->value = cos(value1);
		break;
	case MATH_TAN:
	{
		//x!=k*pi+pi/2 -> 2*x/pi != 2*k+1(odd)
		double value = value1 * 2.0 / M_PI;
		if (abs(value - (int)value) < MIN_DOUBLE && (int)value % 2 != 1)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw TError{ ERROR_OUTOF_DOMAIN, String(TEXT("tan(")) + To_string(value) + String(TEXT(")")) };
			return;
		}
		Operator->value = tan(value1);
		break;
	}
	case MATH_ARCSIN:
		if (value1 < -1.0 || value1 > 1.0)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw TError{ ERROR_OUTOF_DOMAIN, String(TEXT("arcsin(")) + To_string(value1) + String(TEXT(")")) };
			return;
		}
		Operator->value = asin(value1);
		break;
	case MATH_ARCCOS:
		if (value1 < -1.0 || value1 > 1.0)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw TError{ ERROR_OUTOF_DOMAIN, String(TEXT("arccos(")) + To_string(value1) + String(TEXT(")")) };
		}
		Operator->value = acos(value1);
		break;
	case MATH_ARCTAN:
		Operator->value = atan(value1);
		break;
	case MATH_LN:
		if (value1 < MIN_DOUBLE)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw TError{ ERROR_OUTOF_DOMAIN, String(TEXT("ln(")) + To_string(value1) + String(TEXT(")")) };
		}
		Operator->value = log(value1);
		break;
	case MATH_LOG10:
		if (value1 < MIN_DOUBLE)//log(0)或log(负数)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw TError{ ERROR_OUTOF_DOMAIN, String(TEXT("log10(")) + To_string(value1) + String(TEXT(")")) };
		}
		Operator->value = log10(value1);
		break;
	case MATH_EXP:
		Operator->value = exp(value1);
		break;
	case MATH_POSITIVE:
		break;
	case MATH_NEGATIVE:
		Operator->value = -value1;
		break;

	case MATH_MOD://%
		if ((int)value2 == 0)
			throw TError{ ERROR_DIVIDE_ZERO, To_string(value2) };
		Operator->value = (int)value1 % (int)value2;
		break;
	case MATH_AND://&
		Operator->value = (int)value1 & (int)value2;
		break;
	case MATH_OR://|
		Operator->value = (int)value1 | (int)value2;
		break;

	case MATH_POWER://^
		//0^0
		if (abs(value1) < MIN_DOUBLE && abs(value2) < MIN_DOUBLE)
		{
			Operator->eType = NODE_OPERATOR;
			throw TError{ ERROR_ZERO_POWEROF_ZERO, TEXT("") };
		}

		//(-1)^0.5=i
		if (value1 < 0 && IsIntAndEven(1ll / value2))
		{
			Operator->eType = NODE_OPERATOR;
			throw TError{ ERROR_I, String(TEXT("pow(")) + To_string(value1) + TEXT(",") + To_string(value2) + TEXT(")") };
		}
		Operator->value = pow(value1, value2);
		break;

	case MATH_MULTIPLY:
		Operator->value = value1 * value2;
		break;
	case MATH_DIVIDE:
		if (abs(value2) < MIN_DOUBLE)
		{
			Operator->eType = NODE_OPERATOR;
			throw TError{ ERROR_DIVIDE_ZERO, TEXT("") };
		}
		Operator->value = value1 / value2;
		break;

	case MATH_ADD:
		Operator->value = value1 + value2;
		break;
	case MATH_SUBSTRACT:
		Operator->value = value1 - value2;
		break;
	}
	Operator->eOperator = MATH_NULL;
}

//复制节点树，返回新节点树头节点
TExpressionTree::TNode *TExpressionTree::CopyNodeTree(TNode *oldNode)
{
	TNode *newNode = new TNode;
	newNode->eType = oldNode->eType;
	newNode->eOperator = oldNode->eOperator;
	newNode->value = oldNode->value;
	newNode->varname = oldNode->varname;

	if (oldNode->left != NULL)
	{
		newNode->left = CopyNodeTree(oldNode->left);
		newNode->left->parent = newNode;
	}
	if (oldNode->right != NULL)
	{
		newNode->right = CopyNodeTree(oldNode->right);
		newNode->right->parent = newNode;
	}

	return newNode;
}

TExpressionTree::TNode *TExpressionTree::NewNode(enumNodeType eType, enumMathOperator eOperator)
{
	TNode *newNode = new TNode;
	newNode->eType = eType;
	newNode->eOperator = eOperator;
	return newNode;
}

//未完成求导：tan,arcsin,arccos,arctan
void TExpressionTree::Diff(TNode *now, String var)
{
	switch (now->eType)
	{
	case NODE_VARIABLE:
		now->eType = NODE_NUMBER;
		if (now->varname == var)
			now->value = 1;
		else
			now->value = 0;
		return;
	case NODE_NUMBER:
		now->value = 0;
		return;
	case NODE_OPERATOR://当前为运算符节点
		switch (now->eOperator)
		{
		case MATH_POSITIVE:
		case MATH_NEGATIVE:
			if (now->left != NULL)
				Diff(now->left, var);
			return;
		case MATH_ADD:
		case MATH_SUBSTRACT:
			if (now->left != NULL)
				Diff(now->left, var);
			if (now->right != NULL)
				Diff(now->right, var);
			return;
		case MATH_MULTIPLY:
			if (now->left->eType == NODE_NUMBER || now->right->eType == NODE_NUMBER)//两个操作数中有一个是数字
			{
				if (now->left->eType == NODE_NUMBER)
					Diff(now->right, var);
				else
					Diff(now->left, var);
			}
			else
			{
				TNode *plus;
				plus = new TNode;
				plus->eType = NODE_OPERATOR;
				plus->eOperator = MATH_ADD;
				if (now != head)
				{
					//plus上下行连接
					if (now->parent->left == now)
						now->parent->left = plus;
					if (now->parent->right == now)
						now->parent->right = plus;
					plus->parent = now->parent;
				}
				else
				{
					head = plus;
				}
				now->parent = plus;
				plus->left = now;

				//加入右节点
				TNode *newRight;
				newRight = CopyNodeTree(now);

				plus->right = newRight;
				newRight->parent = plus;

				Diff(plus->left->left, var);
				Diff(plus->right->right, var);
			}
			return;
		case MATH_DIVIDE:
			if (now->right->eType == NODE_NUMBER)// f(x)/number = f'(x)/number
			{
				Diff(now->left, var);
			}
			else
			{
				TNode *divide = now;
				TNode *u1 = now->left;
				TNode *v1 = now->right;

				//创建减号
				TNode *substract;
				substract = NewNode(NODE_OPERATOR, MATH_SUBSTRACT);

				//创建2个乘号
				TNode *multiply1, *multiply2;
				multiply1 = NewNode(NODE_OPERATOR, MATH_MULTIPLY);
				multiply2 = NewNode(NODE_OPERATOR, MATH_MULTIPLY);

				//创建乘方
				TNode *power;
				power = NewNode(NODE_OPERATOR, MATH_POWER);

				//连接除号下面2个节点：-, ^
				divide->left = substract;
				substract->parent = divide;
				divide->right = power;
				power->parent = divide;

				//连接减号下面2个节点
				substract->left = multiply1;
				multiply1->parent = substract;
				substract->right = multiply2;
				multiply2->parent = substract;

				//连接乘号1下面2个节点：u1, v1
				multiply1->left = u1;
				u1->parent = multiply1;
				multiply1->right = v1;
				v1->parent = multiply1;

				//创建u2, v2
				TNode *u2, *v2;
				u2 = CopyNodeTree(u1);
				v2 = CopyNodeTree(v1);

				//连接乘号2下面的u2, v2
				multiply2->left = u2;
				u2->parent = multiply2;
				multiply2->right = v2;
				v2->parent = multiply2;

				//创建v3, 2
				TNode *v3, *num2;
				v3 = CopyNodeTree(v1);
				num2 = NewNode(NODE_NUMBER);
				num2->value = 2;

				//连接^下面的v3和2
				power->left = v3;
				v3->parent = power;
				power->right = num2;
				num2->parent = power;

				Diff(u1, var);
				Diff(v2, var);

			}
			return;
		case MATH_POWER:
		{
			bool LChildIsNumber = now->left->eType == NODE_NUMBER;
			bool RChildIsNumber = now->right->eType == NODE_NUMBER;
			if (LChildIsNumber && RChildIsNumber)
			{
				delete now->left;
				delete now->right;
				now->left = NULL;
				now->right = NULL;
				now->eType = NODE_NUMBER;
				now->eOperator = MATH_NULL;
				now->value = 0.0;
				return;
			}

			TNode *multiply1 = NewNode(NODE_OPERATOR, MATH_MULTIPLY);
			TNode *multiply2 = NewNode(NODE_OPERATOR, MATH_MULTIPLY);
			TNode *power = now;
			TNode *u = now->left;
			TNode *v = now->right;
			TNode *u2 = CopyNodeTree(u);
			TNode *v2 = CopyNodeTree(v);

			if (power == head)
			{
				head = multiply1;
			}
			else
			{
				if (power->parent->left == power)
					power->parent->left = multiply1;
				if (power->parent->right == power)
					power->parent->right = multiply1;
				multiply1->parent = power->parent;
			}

			if (RChildIsNumber)
				v->value -= 1.0;

			multiply1->left = power;
			power->parent = multiply1;

			multiply1->right = multiply2;
			multiply2->parent = multiply1;

			multiply2->left = v2;
			v2->parent = multiply2;

			if (RChildIsNumber)
			{
				multiply2->right = u2;
				u2->parent = multiply2;
				Diff(u2, var);
				return;
			}
			else
			{
				TNode *ln = NewNode(NODE_FUNCTION, MATH_LN);

				multiply2->right = ln;
				ln->parent = multiply2;

				ln->left = u2;
				u2->parent = ln;

				Diff(multiply2, var);
				return;
			}
			return;
		}
		}
		break;
	case NODE_FUNCTION:
	{
		//不考虑定义域
		//函数内为数字则导为0
		if (now->left->eType == NODE_NUMBER)
		{
			now->eType = NODE_NUMBER;
			now->eOperator = MATH_NULL;
			now->value = 0;
			DeleteNode(now->left);
			now->left = NULL;
			return;
		}

		TNode *function = now;
		switch (function->eOperator)
		{
		case MATH_SQRT:
		{
			//转化为幂求导
			TNode *u = function->left;
			TNode *power = NewNode(NODE_OPERATOR, MATH_POWER);
			TNode *num1half = NewNode(NODE_NUMBER);
			num1half->value = 0.5;

			power->left = u;
			u->parent = power;

			power->right = num1half;
			num1half->parent = power;

			if (function == head)
				head = power;
			else
			{
				if (function->parent->left == function)
					function->parent->left = power;
				if (function->parent->right == function)
					function->parent->right = power;
				power->parent = function->parent;
			}

			delete function;
			Diff(power, var);

			return;
		}
		case MATH_LN:
		case MATH_LOG10:
		{
			TNode *divide = NewNode(NODE_OPERATOR, MATH_DIVIDE);
			TNode *num1 = NewNode(NODE_NUMBER);
			num1->value = 1.0;

			divide->left = num1;
			num1->parent = divide;

			TNode *u = function->left;

			if (function->eOperator == MATH_LN)//ln(x)=1/x
			{
				divide->right = u;
				u->parent = divide;
			}
			else
			{
				//log10(x)=1/(x*ln(10))
				TNode *multiply2 = NewNode(NODE_OPERATOR, MATH_MULTIPLY);

				divide->right = multiply2;
				multiply2->parent = divide;

				multiply2->left = u;
				u->parent = multiply2;

				TNode *ln = NewNode(NODE_FUNCTION, MATH_LN);

				multiply2->right = ln;
				ln->parent = multiply2;

				TNode *num10 = NewNode(NODE_NUMBER);
				num10->value = 10.0;

				ln->left = num10;
				num10->parent = ln;
			}

			TNode *top = divide;
			TNode *u2 = NULL;
			if (u->eType != NODE_VARIABLE)
			{
				TNode *multiply1 = NewNode(NODE_OPERATOR, MATH_MULTIPLY);
				u2 = CopyNodeTree(u);

				multiply1->left = divide;
				divide->parent = multiply1;

				multiply1->right = u2;
				u2->parent = multiply1;

				top = multiply1;
			}

			if (function == head)
			{
				head = top;
			}
			else
			{
				if (function->parent->left == function)
				{
					function->parent->left = top;
					top->parent = function->parent;
				}
				if (function->parent->right == function)
				{
					function->parent->right = top;
					top->parent = function->parent;
				}
			}
			delete function;

			if (u->eType != NODE_VARIABLE)
				Diff(u2, var);

		}
		return;
		case MATH_EXP:
		{
			if (function->left->eType == NODE_VARIABLE)//e^x=e^x
				return;
			TNode *multiply = NewNode(NODE_OPERATOR, MATH_MULTIPLY);
			TNode *u2 = CopyNodeTree(function->left);

			if (function == head)
			{
				head = multiply;
			}
			else
			{
				if (function->parent->left == function)
				{
					function->parent->left = multiply;
					multiply->parent = function->parent;
				}
				if (function->parent->right == function)
				{
					function->parent->right = multiply;
					multiply->parent = function->parent;
				}
			}

			multiply->left = function;
			function->parent = multiply;

			multiply->right = u2;
			u2->parent = multiply;

			Diff(u2, var);
		}
		return;
		case MATH_COS:
		{
			TNode *negative = new TNode;
			negative->eType = NODE_OPERATOR;
			negative->eOperator = MATH_NEGATIVE;

			//连接上一级和负号
			if (function != head)
			{
				if (function->parent->left == function)
				{
					function->parent->left = negative;
					negative->parent = function->parent;
				}
				if (function->parent->right == function)
				{
					function->parent->right = negative;
					negative->parent = function->parent;
				}
			}
			else
			{
				head = negative;
				negative->parent = NULL;
			}

			TNode *multiply = new TNode;
			multiply->eType = NODE_OPERATOR;
			multiply->eOperator = MATH_MULTIPLY;

			//连接负号和乘号
			negative->left = multiply;
			multiply->parent = negative;

			//连接乘号和function
			multiply->left = function;
			function->parent = multiply;

			//变更function
			function->eOperator = MATH_SIN;

			//复制u2并连接乘号
			TNode *u2 = CopyNodeTree(function->left);
			multiply->right = u2;
			u2->parent = multiply;

			Diff(u2, var);
		}
		return;
		case MATH_SIN:
		{
			TNode *multiply = new TNode;
			multiply->eType = NODE_OPERATOR;
			multiply->eOperator = MATH_MULTIPLY;

			//连接上一级和乘号
			if (function != head)
			{
				if (function->parent->left == function)
				{
					function->parent->left = multiply;
					multiply->parent = function->parent;
				}
				if (function->parent->right == function)
				{
					function->parent->right = multiply;
					multiply->parent = function->parent;
				}
			}
			else
			{
				head = multiply;
				multiply->parent = NULL;
			}

			//连接乘号和function
			multiply->left = function;
			function->parent = multiply;

			//变更function
			function->eOperator = MATH_COS;

			//复制u2并连接乘号
			TNode *u2 = CopyNodeTree(function->left);
			multiply->right = u2;
			u2->parent = multiply;

			Diff(u2, var);
		}
		//case MATH_ARCTAN:
		//{
		//	TNode *multiply = new TNode()
		//}
		return;
		default:
			throw TError(ERROR_WRONG_MATH_OPERATOR, String(TEXT("未完成的运算符")) + Function2Str(now->eOperator));
		}
	}
	}
}

String  TExpressionTree::Diff(String var, int n, bool bOutput)
{
	if (pVariableTable->FindVariableTable(var) == pVariableTable->VariableTable.end())
		throw TError{ ERROR_UNDEFINED_VARIABLE, var };

	for (int i = 0; i < n; i++)
	{
		Diff(head, var);
	}
	return OutputStr();
}

void TExpressionTree::LinkParent(TNode *child, TNode *ignore)
{
	if (ignore == head)
	{
		head = child;
		child->parent = NULL;
	}
	else
	{
		if (ignore->parent->left == ignore)
			ignore->parent->left = child;
		if (ignore->parent->right == ignore)
			ignore->parent->right = child;
		child->parent = ignore->parent;
	}
}

void TExpressionTree::Simplify(TNode *now)
{
	//左遍历
	if (now->left != NULL)
		Simplify(now->left);

	//右遍历
	if (now->right != NULL)
		Simplify(now->right);

	//化简
	//OutputStr();
	if (GetOperateNum(now->eOperator) == 1)
	{
		bool ChildIs0 = (now->left->eType == NODE_NUMBER && abs(now->left->value) < MIN_DOUBLE);
		bool ChildIs1 = (now->left->eType == NODE_NUMBER && abs(now->left->value - 1) < MIN_DOUBLE);

		//sin(0)=0
		if (now->eOperator == MATH_SIN && ChildIs0)
		{
			LinkParent(now->left, now);
			now->left->value = 0;
			delete now;
		}

		//cos(0)=1
		if (now->eOperator == MATH_COS && ChildIs0)
		{
			LinkParent(now->left, now);
			now->left->value = 1;
			delete now;
		}

		if (now->eOperator == MATH_NEGATIVE && now->left->eType == NODE_NUMBER)
		{
			TNode *negative = now;
			TNode *num = now->left;
			LinkParent(num, negative);
			num->value = -num->value;
			delete negative;
		}
	}

	if (GetOperateNum(now->eOperator) == 2)
	{
		//下列每种情况必须互斥，因为每个情况都有返回值，涉及删除操作，若不返回连续执行将导致指针出错
		//不检查左右儿子是否存在，因为此处本身就是2元运算符
		bool LChildIs0 = (now->left->eType == NODE_NUMBER && abs(now->left->value) < MIN_DOUBLE);
		bool RChildIs0 = (now->right->eType == NODE_NUMBER && abs(now->right->value) < MIN_DOUBLE);
		bool LChildIs1 = (now->left->eType == NODE_NUMBER && abs(now->left->value - 1) < MIN_DOUBLE);
		bool RChildIs1 = (now->right->eType == NODE_NUMBER && abs(now->right->value - 1) < MIN_DOUBLE);

		//被除数为0
		if (now->eOperator == MATH_DIVIDE && RChildIs0)
			throw TError{ ERROR_DIVIDE_ZERO, TEXT("") };

		//0的0次方
		if (now->eOperator == MATH_POWER && LChildIs0 && RChildIs0)
			throw TError{ ERROR_ZERO_POWEROF_ZERO, TEXT("") };

		//若左右儿子都是数字，则计算出来
		if (now->left->eType == NODE_NUMBER && now->right->eType == NODE_NUMBER)
		{
			//try
			//{
			CalcNode(now, now->left, now->right);
			delete now->left;
			delete now->right;
			now->eOperator = MATH_NULL;
			now->left = NULL;
			now->right = NULL;
			//}
			//catch (enumError err)
			//{
			//	return nowError = err;
			//}
		}

		//任何节点的0次方均等于1，除了0的0次方已在前面报错
		if (now->eOperator == MATH_POWER && RChildIs0)
		{
			//替换掉当前运算符，这个1节点将在回溯时处理
			//新建一个1节点
			TNode *temp;
			temp = new TNode;
			temp->eType = NODE_NUMBER;
			temp->value = 1;

			//0节点连接到上面
			if (now != head)
			{
				if (now->parent->left == now)
				{
					now->parent->left = temp;
					temp->parent = now->parent;
				}
				if (now->parent->right == now)
				{
					now->parent->right = temp;
					temp->parent = now->parent;
				}
			}
			else
				head = temp;

			DeleteNode(now);
		}

		//任何数乘或被乘0、被0除、0的除0外的任何次方，等于0
		if ((now->eOperator == MATH_MULTIPLY && (LChildIs0 || RChildIs0)) ||
			(now->eOperator == MATH_DIVIDE && LChildIs0) ||
			(now->eOperator == MATH_POWER && LChildIs0))
		{
			//替换掉当前运算符，这个0节点将在回溯时处理
			//新建一个0节点
			TNode *temp;
			temp = new TNode;
			temp->eType = NODE_NUMBER;
			temp->value = 0;

			//0节点连接到上面
			if (now != head)
			{
				if (now->parent->left == now)
				{
					now->parent->left = temp;
					temp->parent = now->parent;
				}
				if (now->parent->right == now)
				{
					now->parent->right = temp;
					temp->parent = now->parent;
				}
			}
			else
				head = temp;

			DeleteNode(now);
		}

		//0-x=-x
		if (now->eOperator == MATH_SUBSTRACT && LChildIs0)
		{
			TNode *LChild = now->left;
			TNode *RChild = now->right;
			now->eOperator = MATH_NEGATIVE;
			now->left = RChild;
			now->right = NULL;

			delete LChild;
		}

		//任何数加或被加0、被减0、乘或被乘1、被1除、开1次方，等于自身
		if (
			(now->eOperator == MATH_ADD && (LChildIs0 || RChildIs0)) ||
			(now->eOperator == MATH_SUBSTRACT && RChildIs0) ||
			(now->eOperator == MATH_MULTIPLY && (LChildIs1 || RChildIs1)) ||

			(now->eOperator == MATH_DIVIDE && RChildIs1) ||
			(now->eOperator == MATH_POWER && RChildIs1))
		{
			TNode *remain = NULL, *num = NULL;
			if (LChildIs1 || LChildIs0)
			{
				num = now->left;
				remain = now->right;
			}
			if (RChildIs1 || RChildIs0)
			{
				num = now->right;
				remain = now->left;
			}

			//连接父级和剩余项
			if (now != head)
			{
				if (now->parent->left == now)
				{
					now->parent->left = remain;
					remain->parent = now->parent;
				}
				if (now->parent->right == now)
				{
					now->parent->right = remain;
					remain->parent = now->parent;
				}
			}
			else
			{
				head = remain;
				head->parent = NULL;
			}
			delete num;
			delete now;
		}


	}

}

void  TExpressionTree::Simplify(bool bOutput)
{
	Simplify(head);
}

//string  TExpressionTree::FindVariableTableFrom(const string varstr, std::vector<string > newVariableTable)
//{
//	for (auto szNewVar : newVariableTable)
//		if (_tcscmp(varstr, szNewVar) == 0)
//			return szNewVar;
//	return NULL;
//}
//
//void TExpressionTree::ReplaceNodeVariable(TNode *now, std::vector<string > &newVariableTable)
//{
//	if (now->left != NULL)
//		ReplaceNodeVariable(now->left, newVariableTable);
//	if (now->right != NULL)
//		ReplaceNodeVariable(now->right, newVariableTable);
//	TCHAR *temp;
//	if (now->eType == NODE_VARIABLE && (temp = FindVariableTableFrom(now->varname.c_str(), newVariableTable)))
//		now->varname = temp;
//}

void TExpressionTree::GetVariablePos(const String var, std::vector<TNode *> &VarsPos)
{
	GetVariablePos(head, var, VarsPos);
}

void TExpressionTree::GetVariablePos(TNode *now, const String var, std::vector<TNode *> &VarsPos)
{
	if (now->eType == NODE_VARIABLE && now->varname == var)
		VarsPos.push_back(now);
	if (now->left != NULL)
		GetVariablePos(now->left, var, VarsPos);
	if (now->right != NULL)
		GetVariablePos(now->right, var, VarsPos);
}

void TExpressionTree::CopyVariableTable(std::vector<String > &Dest, const std::vector<String > source)
{
	Dest.clear();
	for (auto sz : source)
		Dest.push_back(sz);
}

//替换 VarsVector变量 NumsVector数字
void  TExpressionTree::Subs(std::vector<String > VarsVector, std::vector<double> NumsVector, bool output)
{
	if (VarsVector.size() == NumsVector.size())//替换与被替换元素数目相等
	{
		for (size_t i = 0; i < VarsVector.size(); i++)//遍历被替换变量
		{
			//查表识别被替换变量
			auto it = pVariableTable->FindVariableTable(VarsVector[i]);
			if (it != pVariableTable->VariableTable.end())//已识别出
			{
				String var = *it;
				//构建替换节点树
				TExpressionTree Expr;
				Expr.LinkVariableTable(pVariableTable);
				Expr.Read(NumsVector[i], false);

				//得到所有被替换变量的位置
				std::vector<TNode *> VarsPos;
				GetVariablePos(var, VarsPos);
				for (size_t j = 0; j < VarsPos.size(); j++)
				{
					TNode *newNode = CopyNodeTree(Expr.head);

					//连接到新节点
					if (VarsPos[j] != head)
					{
						if (VarsPos[j]->parent->left != NULL && VarsPos[j]->parent->left == VarsPos[j])
							VarsPos[j]->parent->left = newNode;
						if (VarsPos[j]->parent->right != NULL && VarsPos[j]->parent->right == VarsPos[j])
							VarsPos[j]->parent->right = newNode;
						newNode->parent = VarsPos[j]->parent;
					}
					else
						head = newNode;

					//删掉旧节点
					delete VarsPos[j];
				}
			}
		}
	}
	else
		throw TError{ ERROR_SUBS_NOT_EQUAL, TEXT("") };
}

void TExpressionTree::Subs_inner(TNode *node, String ptVar, double value)
{
	if (node->eType == NODE_VARIABLE && node->varname == ptVar)
	{
		node->eType = NODE_NUMBER;
		node->value = value;
		return;
	}

	if (node->left != NULL)
		Subs_inner(node->left, ptVar, value);

	if (node->right != NULL)
		Subs_inner(node->right, ptVar, value);
}

//替换  var变量指针 value数字
void  TExpressionTree::Subs(String ptVar, double value, bool output)
{
	Subs_inner(head, ptVar, value);
}

//替换  vars变量串 nums数字串 以空格分隔，支持表达式替换
void  TExpressionTree::Subs(const String vars, const String nums, bool output)
{
	if (vars.empty() || nums.empty())
	{
		throw TError{ ERROR_EMPTY_INPUT, TEXT("") };
		return;
	}

	std::vector<String> VarsVector = StrSliceToVector(vars);
	std::vector<String> NumsVector = StrSliceToVector(nums);

	if (VarsVector.size() == NumsVector.size())//替换与被替换元素数目相等
	{
		for (size_t i = 0; i < VarsVector.size(); i++)//遍历被替换变量
		{
			//查表识别被替换变量
			auto it = pVariableTable->FindVariableTable(VarsVector[i]);
			if (it != pVariableTable->VariableTable.end())//已识别出
			{
				String var = *it;
				//构建替换节点树
				TExpressionTree Expr;
				Expr.LinkVariableTable(pVariableTable);
				Expr.Read(NumsVector[i], false);

				//得到所有被替换变量的位置
				std::vector<TNode *> VarsPos;
				GetVariablePos(var, VarsPos);
				for (size_t j = 0; j < VarsPos.size(); j++)
				{
					TNode *newNode = CopyNodeTree(Expr.head);

					//连接到新节点
					if (VarsPos[j] != head)
					{
						if (VarsPos[j]->parent->left != NULL && VarsPos[j]->parent->left == VarsPos[j])
							VarsPos[j]->parent->left = newNode;
						if (VarsPos[j]->parent->right != NULL && VarsPos[j]->parent->right == VarsPos[j])
							VarsPos[j]->parent->right = newNode;
						newNode->parent = VarsPos[j]->parent;
					}
					else
						head = newNode;

					//删掉旧节点
					delete VarsPos[j];
				}
			}
		}
	}
	else
		throw TError{ ERROR_SUBS_NOT_EQUAL, TEXT("") };

}

void  TExpressionTree::LinkVariableTable(TVariableTable *p)
{
	pVariableTable = p;
}

void  TExpressionTree::Read(double num, bool bOutput)
{
	head = NewNode(NODE_NUMBER);
	head->value = num;
}

void TExpressionTree::Solve(TNode *now, TNode *&write_pos)
{
	TNode *parent = now->parent;
	if (parent != NULL)
	{
		TNode *brother;
		bool bVarIsLeft;
		if (parent->left == now)
		{
			brother = parent->right;
			bVarIsLeft = true;
		}
		else
		{
			brother = parent->left;
			bVarIsLeft = false;
		}

		switch (parent->eOperator)
		{
		case MATH_ADD:
			write_pos->eType = NODE_OPERATOR;
			write_pos->eOperator = MATH_SUBSTRACT;

			write_pos->right = CopyNodeTree(brother);
			write_pos->right->parent = write_pos;

			write_pos->left = new TNode;

			write_pos->left->parent = write_pos;
			Solve(parent, write_pos->left);
			break;
		case MATH_MULTIPLY:
			write_pos->eType = NODE_OPERATOR;
			write_pos->eOperator = MATH_DIVIDE;

			write_pos->right = CopyNodeTree(brother);
			write_pos->right->parent = write_pos;

			write_pos->left = new TNode;

			write_pos->left->parent = write_pos;
			Solve(parent, write_pos->left);
			break;
		case MATH_SUBSTRACT://分左右
			if (bVarIsLeft)//被减数
			{
				write_pos->eType = NODE_OPERATOR;
				write_pos->eOperator = MATH_ADD;

				write_pos->right = CopyNodeTree(brother);
				write_pos->right->parent = write_pos;

				write_pos->left = new TNode;

				write_pos->left->parent = write_pos;
				Solve(parent, write_pos->left);
			}
			else
			{
				write_pos->eType = NODE_OPERATOR;
				write_pos->eOperator = MATH_SUBSTRACT;

				write_pos->left = CopyNodeTree(brother);
				write_pos->left->parent = write_pos;

				write_pos->right = new TNode;

				write_pos->right->parent = write_pos;
				Solve(parent, write_pos->right);
			}
			break;
		case MATH_DIVIDE://分左右
			if (bVarIsLeft)//被除数
			{
				write_pos->eType = NODE_OPERATOR;
				write_pos->eOperator = MATH_MULTIPLY;

				write_pos->right = CopyNodeTree(brother);
				write_pos->right->parent = write_pos;

				write_pos->left = new TNode;

				write_pos->left->parent = write_pos;
				Solve(parent, write_pos->left);
			}
			else
			{
				write_pos->eType = NODE_OPERATOR;
				write_pos->eOperator = MATH_DIVIDE;

				write_pos->left = CopyNodeTree(brother);
				write_pos->left->parent = write_pos;

				write_pos->right = new TNode;

				write_pos->right->parent = write_pos;
				Solve(parent, write_pos->right);
			}
			break;
		case MATH_POSITIVE:
			write_pos->eType = NODE_OPERATOR;
			write_pos->eOperator = MATH_POSITIVE;

			write_pos->left = new TNode;

			write_pos->left->parent = write_pos;
			Solve(parent, write_pos->left);
			break;
		case MATH_NEGATIVE:
			write_pos->eType = NODE_OPERATOR;
			write_pos->eOperator = MATH_NEGATIVE;

			write_pos->left = new TNode;

			write_pos->left->parent = write_pos;
			Solve(parent, write_pos->left);
			break;
		case MATH_SIN:
			write_pos->eType = NODE_FUNCTION;
			write_pos->eOperator = MATH_ARCSIN;

			write_pos->left = new TNode;

			write_pos->left->parent = write_pos;
			Solve(parent, write_pos->left);
			break;
		case MATH_COS:
			write_pos->eType = NODE_FUNCTION;
			write_pos->eOperator = MATH_ARCCOS;

			write_pos->left = new TNode;

			write_pos->left->parent = write_pos;
			Solve(parent, write_pos->left);
			break;
		case MATH_TAN:
			write_pos->eType = NODE_FUNCTION;
			write_pos->eOperator = MATH_ARCTAN;

			write_pos->left = new TNode;

			write_pos->left->parent = write_pos;
			Solve(parent, write_pos->left);
			break;
		default:
			assert(0);
			break;
		}
	}
	else
	{
		//have not parent
		write_pos->eType = NODE_NUMBER;
		write_pos->value = 0.0;

	}
}

//求解单变量方程 不验证可求解性，需提前调用HasOnlyOneVar确认
String  TExpressionTree::Solve(String &var, double &value)
{
	TExpressionTree Result;

	TNode *ResultNow = new TNode;

	var = LastVarNode->varname;

	Solve(LastVarNode, ResultNow);

	Result.head = ResultNow;

	value = Result.Value(true);

	return OutputStr();

}

//读之前不清零，请自行处理
void  TExpressionTree::Read(String expression, bool bOutput)
{
	std::queue<TNode *> InOrder;
	std::vector<TNode *> PostOrder;

	ReadToInOrder(expression, InOrder);
	InQueue2PostQueue(InOrder, PostOrder);
	BuildExpressionTree(PostOrder);
}

bool TExpressionTree::CanCalc(TNode *now)
{
	if (now->left != NULL)
		if (CanCalc(now->left) == false)
			return false;
	if (now->right != NULL)
		if (CanCalc(now->right) == false)
			return false;

	if (now->eType == NODE_VARIABLE)
		return false;
	return true;
}

bool TExpressionTree::CanCalc()
{
	return CanCalc(head);
}

//迭代计算，最终剩下1个节点
void TExpressionTree::Calc(TNode *now)
{
	if (now->left != NULL)
		Calc(now->left);
	if (now->right != NULL)
		Calc(now->right);

	if (GetOperateNum(now->eOperator) == 1 && now->left->eType == NODE_NUMBER)
	{
		try
		{
			CalcNode(now, now->left);
		}
		catch (enumError &err)
		{
			throw err;
		}
		delete now->left;
		now->left = NULL;
	}

	if (GetOperateNum(now->eOperator) == 2 && now->left->eType == NODE_NUMBER && now->right->eType == NODE_NUMBER)
	{
		try
		{
			CalcNode(now, now->left, now->right);
		}
		catch (enumError &err)
		{
			throw err;
		}
		delete now->left;
		delete now->right;
		now->left = NULL;
		now->right = NULL;
	}
}

//计算表达式值 operateHeadNode决定是否操作本身的节点
double TExpressionTree::Value(bool operateHeadNode)
{
	TNode *pNode = NULL;
	if (operateHeadNode)
		pNode = head;
	else
		pNode = CopyNodeTree(head);

	try
	{
		Calc(pNode);
	}
	catch (enumError &err)
	{
		//删掉节点树并提交给上级
		if (operateHeadNode == false)
			DeleteNode(pNode);
		throw err;
	}

	//得到最终结果
	double num = pNode->value;
	//释放复制的树
	if (operateHeadNode == false)
		delete pNode;
	return num;
}

//复制出一棵临时树计算值
String  TExpressionTree::Calc(double *result)
{
	if (CanCalc())
	{
		TNode *Duplicate = CopyNodeTree(head);
		Calc(Duplicate);

		if (result != NULL)
			*result = Duplicate->value;

		String temp = Node2Str(*Duplicate);
		delete Duplicate;

		return temp;
	}
	else
		return OutputStr();
}

TExpressionTree &TExpressionTree::operator=(const TExpressionTree &expr)
{
	Release();
	Reset();
	head = CopyNodeTree(expr.head);
	LinkVariableTable(expr.pVariableTable);
	return *this;
}

void TExpressionTree::ReleaseVectorTNode(std::vector<TNode *> vec)
{
	for (auto pNode : vec)
		delete pNode;
}

void TExpressionTree::Vpa_inner(TNode *now)
{
	if (now->left != NULL)
		Vpa_inner(now->left);
	if (now->right != NULL)
		Vpa_inner(now->right);

	if (now->eType == NODE_VARIABLE)
	{
		now->eType = NODE_NUMBER;
		now->value = pVariableTable->GetValueFromVarPoint(now->varname);
	}
}

//仅将变量表内置数值代入，不进行计算
void  TExpressionTree::Vpa(bool bOutput)
{
	Vpa_inner(head);
}

bool TExpressionTree::IsSingleVar()//检查是否为一元
{
	//return SelfVariableTable.VariableTable.size() == 1;
	return true;
}


void TExpressionTree::CheckOnlyOneVar(TNode *now)//只有一个变量（实时验证）
{
	if (now->eType == NODE_VARIABLE)
	{
		iVarAppearedCount++;
		LastVarNode = now;
	}
	if (now->left != NULL)
		CheckOnlyOneVar(now->left);

	if (now->right != NULL)
		CheckOnlyOneVar(now->right);
}

bool TExpressionTree::CheckOnlyOneVar()//只有一个变量（实时验证）
{
	iVarAppearedCount = 0;
	CheckOnlyOneVar(head);
	return HasOnlyOneVar();
}

bool TExpressionTree::HasOnlyOneVar()//只有一个变量
{
	return iVarAppearedCount == 1;
}

TExpressionTree &TExpressionTree::operator*(double value)
{
	if (head == NULL)
	{
		throw TError{ ERROR_EMPTY_INPUT, TEXT("") };
		return *this;
	}

	TNode *Multiply = NewNode(NODE_OPERATOR, MATH_MULTIPLY);
	TNode *Value = NewNode(NODE_NUMBER);
	Value->value = value;

	Multiply->left = head;
	Multiply->right = Value;

	head->parent = Multiply;
	Value->parent = Multiply;

	head = Multiply;

	return *this;
}

TExpressionTree &TExpressionTree::operator+(double value)
{
	if (head == NULL)
	{
		throw TError{ ERROR_EMPTY_INPUT, TEXT("") };
		return *this;
	}

	TNode *Add = NewNode(NODE_OPERATOR, MATH_ADD);
	TNode *Value = NewNode(NODE_NUMBER);
	Value->value = value;

	Add->left = head;
	Add->right = Value;

	head->parent = Add;
	Value->parent = Add;

	head = Add;

	return *this;
}

TExpressionTree &TExpressionTree::operator-(double value)
{
	if (head == NULL)
	{
		throw TError{ ERROR_EMPTY_INPUT, TEXT("") };
		return *this;
	}

	TNode *Substract = NewNode(NODE_OPERATOR, MATH_SUBSTRACT);
	TNode *Value = NewNode(NODE_NUMBER);
	Value->value = value;

	Substract->left = head;
	Substract->right = Value;

	head->parent = Substract;
	Value->parent = Substract;

	head = Substract;

	return *this;
}

TExpressionTree &TExpressionTree::operator+(const TExpressionTree &expr)
{
	if (head == NULL)
		head = CopyNodeTree(expr.head);
	else
	{
		TNode *Add = NewNode(NODE_OPERATOR, MATH_ADD);
		TNode *Right = CopyNodeTree(expr.head);


		Add->left = head;
		Add->right = Right;

		head->parent = Add;
		Right->parent = Add;

		head = Add;
	}
	return *this;
}