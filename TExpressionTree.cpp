#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "DetectMemoryLeak.h"

#include "TExpressionTree.h"
#include <Windows.h>
#include "TTransfer.h"
#include "TMyString.h"
#define _USE_MATH_DEFINES
#include <math.h>

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
	ErrorInfo = NULL;
	szOutput = NULL;
	pVariableTable = NULL;
	eError = ERROR_NO;
}

void TExpressionTree::Release()
{
	if (ErrorInfo != NULL)
		delete[] ErrorInfo;
	ErrorInfo = NULL;

	if (szOutput != NULL)
		delete[] szOutput;
	szOutput = NULL;

	DeleteNode(head);
	head = NULL;
}

enumError TExpressionTree::GetError()
{
	return eError;
}

void TExpressionTree::DeleteNode(TNode *node)//删除node指向对象但不重置node指针
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

void TExpressionTree::DeleteNodeTraversal(TNode *node)
{
	if (node->left != NULL)
		DeleteNode(node->left);
	if (node->right != NULL)
		DeleteNode(node->right);

	delete node;
}

//将PostOrder建立为树，并进行表达式有效性检验（确保二元及一元运算符、函数均有操作数）
enumError TExpressionTree::BuildExpressionTree(std::vector<TNode *> &PostOrder)
{
	std::stack<TNode *> tempStack;
	//逐个识别PostOrder序列，构建表达式树
	for (int i = 0; i < PostOrder.size(); i++)
	{
		switch (PostOrder[i]->eType)
		{
		case NODE_NUMBER:
		case NODE_VARIABLE:
			tempStack.push(PostOrder[i]);
			break;
		case NODE_FUNCTION:
		case NODE_OPERATOR:
			if (GetOperateNum(PostOrder[i]->eOperator) == 2)
			{
				if (tempStack.empty())
				{
					//释放所有TNode，报错
					ReleaseVectorTNode(PostOrder);
					//Stack只是对PostOrder的重排序，所以不用delete
					return eError = ERROR_WRONG_EXPRESSION;
				}

				PostOrder[i]->right = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();

				if (tempStack.empty())
				{
					//释放所有TNode，报错
					ReleaseVectorTNode(PostOrder);
					//Stack只是对PostOrder的重排序，所以不用delete
					return eError = ERROR_WRONG_EXPRESSION;
				}

				PostOrder[i]->left = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();

				tempStack.push(PostOrder[i]);
			}
			else
			{
				if (tempStack.empty())
				{
					//释放所有TNode，报错
					ReleaseVectorTNode(PostOrder);
					//Stack只是对PostOrder的重排序，所以不用delete
					return eError = ERROR_WRONG_EXPRESSION;
				}

				PostOrder[i]->left = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();

				tempStack.push(PostOrder[i]);
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
	return ERROR_NO;
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

/* 是基本运算符 */
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

TExpressionTree::enumMathOperator TExpressionTree::Str2Function(TCHAR *start, TCHAR *end)
{
	int len = end - start;
	TCHAR *temp = new TCHAR[len + 1];
	_tcsncpy(temp, start, len);
	temp[len] = TEXT('\0');
	if (_tcscmp(temp, TEXT("sin")) == 0)
	{
		delete[] temp;
		return MATH_SIN;
	}
	if (_tcscmp(temp, TEXT("cos")) == 0)
	{
		delete[] temp;
		return MATH_COS;
	}
	if (_tcscmp(temp, TEXT("tan")) == 0)
	{
		delete[] temp;
		return MATH_TAN;
	}
	if (_tcscmp(temp, TEXT("arcsin")) == 0)
	{
		delete[] temp;
		return MATH_ARCSIN;
	}
	if (_tcscmp(temp, TEXT("arccos")) == 0)
	{
		delete[] temp;
		return MATH_ARCCOS;
	}
	if (_tcscmp(temp, TEXT("arctan")) == 0)
	{
		delete[] temp;
		return MATH_ARCTAN;
	}
	if (_tcscmp(temp, TEXT("sqrt")) == 0)
	{
		delete[] temp;
		return MATH_SQRT;
	}
	if (_tcscmp(temp, TEXT("ln")) == 0)
	{
		delete[] temp;
		return MATH_LN;
	}
	if (_tcscmp(temp, TEXT("log10")) == 0)
	{
		delete[] temp;
		return MATH_LOG10;
	}
	if (_tcscmp(temp, TEXT("exp")) == 0)
	{
		delete[] temp;
		return MATH_EXP;
	}
	delete[] temp;
	return MATH_NOT_AVAILIALBE;
}

void TExpressionTree::Function2Str(TExpressionTree::enumMathOperator eOperator, TCHAR *buffer)
{
	switch (eOperator)
	{
	case MATH_SIN:
		_tcscpy(buffer, TEXT("sin"));
		break;
	case MATH_COS:
		_tcscpy(buffer, TEXT("cos"));
		break;
	case MATH_TAN:
		_tcscpy(buffer, TEXT("tan"));
		break;
	case MATH_ARCSIN:
		_tcscpy(buffer, TEXT("arcsin"));
		break;
	case MATH_ARCCOS:
		_tcscpy(buffer, TEXT("arccos"));
		break;
	case MATH_ARCTAN:
		_tcscpy(buffer, TEXT("arctan"));
		break;
	case MATH_SQRT:
		_tcscpy(buffer, TEXT("sqrt"));
		break;
	case MATH_LN:
		_tcscpy(buffer, TEXT("ln"));
		break;
	case MATH_LOG10:
		_tcscpy(buffer, TEXT("log10"));
		break;
	case MATH_EXP:
		_tcscpy(buffer, TEXT("exp"));
		break;

	}
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
		return MATH_NOT_AVAILIALBE;
	}
}
/*  */
TCHAR TExpressionTree::EnumOperatorToTChar(TExpressionTree::enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_POSITIVE:
		return TEXT('+');
	case MATH_NEGATIVE:
		return TEXT('-');
	case MATH_LEFT_PARENTHESIS:
		return TEXT('(');
	case MATH_RIGHT_PARENTHESIS:
		return TEXT(')');
	case MATH_ADD:
		return TEXT('+');
	case MATH_SUBSTRACT:
		return TEXT('-');
	case MATH_MULTIPLY:
		return TEXT('*');
	case MATH_DIVIDE:
		return TEXT('/');
	case MATH_POWER:
		return TEXT('^');
	case MATH_AND:
		return TEXT('&');
	case MATH_OR:
		return TEXT('|');
	case MATH_MOD:
		return TEXT('%');
	}
}

/* 有效性检查（返回0则出现异常字符） */
bool TExpressionTree::isLegal(TCHAR c)
{
	if (isDoubleChar(c)) return true;
	if (isBaseOperator(c)) return true;
	if (TMyString::isAlphaCharOrUnderline(c)) return true;
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
enumError TExpressionTree::InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder)
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
		return eError=ERROR_PARENTHESIS_NOT_MATCH;
	}
	else
		return ERROR_NO;
}

void TExpressionTree::Node2Str(const TNode &node, TCHAR *result)
{
	switch (node.eType)
	{
	case NODE_NUMBER:
		if (abs(node.ValueOrName.value - (long long)node.ValueOrName.value) < MIN_DOUBLE)
			_stprintf(result, TEXT("%d"), (long long)node.ValueOrName.value);
		else
			_stprintf(result, TEXT("%f"), node.ValueOrName.value);
		break;
	case NODE_VARIABLE:
		_tcscpy(result, node.ValueOrName.varname);
		break;
	case NODE_FUNCTION:
		Function2Str(node.eOperator, result);
		break;
	case NODE_OPERATOR:
		result[0] = EnumOperatorToTChar(node.eOperator);
		result[1] = TEXT('\0');
		break;
	}
}

void TExpressionTree::TraverseInOrder(TNode *now, TCHAR *output, TCHAR *buffer)
{
	int has_parenthesis = 0;
	if (GetOperateNum(now->eOperator) == 1)//一元运算符：函数和取负
	{
		if (now->eType == NODE_FUNCTION)
		{
			Node2Str(*now, buffer);
			_tcscat(output, buffer);
			_tcscat(output, TEXT("("));
			has_parenthesis = 1;
		}
		else
		{
			_tcscat(output, TEXT("("));
			Node2Str(*now, buffer);
			_tcscat(output, buffer);
			has_parenthesis = 1;
		}
	}

	if (GetOperateNum(now->eOperator) != 1)//非一元运算符才输出，即一元运算符的输出顺序已改变
	{
		if (now->eType == NODE_OPERATOR)//本级为运算符
			//父运算符存在，为二元，且优先级高于本级，则添加括号
			if ((now->parent != NULL && GetOperateNum(now->parent->eOperator) == 2 &&
				(
				Rank(now->parent->eOperator) > Rank(now->eOperator)
				||//两级都是右结合且优先级相等
				(Rank(now->parent->eOperator) == Rank(now->eOperator) && (isLeft2Right(now->parent->eOperator) == false && isLeft2Right(now->eOperator) == false))
				)
				)
				||
				//父运算符存在，为除号，且本级为分母，则添加括号
				(now->parent != NULL && now->parent->eOperator == MATH_DIVIDE && now == now->parent->right))
			{
				_tcscat(output, TEXT("("));
				has_parenthesis = 1;
			}
	}

	if (now->left != NULL)//左遍历
	{
		TraverseInOrder(now->left, output, buffer);
	}

	if (GetOperateNum(now->eOperator) != 1)//非一元运算符才输出，即一元运算符的输出顺序已改变
	{
		Node2Str(*now, buffer);
		_tcscat(output, buffer);
	}


	if (now->right != NULL)//右遍历
	{
		TraverseInOrder(now->right, output, buffer);
	}

	//回到本级时补齐右括号，包住前面的东西
	if (has_parenthesis)
	{
		_tcscat(output, TEXT(")"));
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

TCHAR * TExpressionTree::OutputStr(bool bIgnoreError)
{
	if (eError == ERROR_NO || bIgnoreError)
	{
		TCHAR buffer[MAX_VAR_NAME];

		if (szOutput != NULL)
			delete[] szOutput;

		szOutput = new TCHAR[MAX_VAR_NAME * GetNodeNum(head)];
		szOutput[0] = TEXT('\0');

		if (head != NULL)
			TraverseInOrder(head, szOutput, buffer);
		return szOutput;
	}
	else
		return GetErrorInfo();
}

enumError TExpressionTree::ReadToInOrder(TCHAR *expression, std::queue<TNode *> &InOrder)
{
	if (_tcslen(expression) == 0)
		return ERROR_EMPTY_INPUT;
	TMyString::Replace(expression, TEXT(" "), TEXT(""));
	TMyString::Replace(expression, TEXT("\t"), TEXT(""));
	TMyString::Replace(expression, TEXT("\r"), TEXT(""));
	TMyString::Replace(expression, TEXT("\n"), TEXT(""));

	//过滤掉所有多余的加减
	TMyString::ReplaceLoop(expression, TEXT("--"), TEXT("+"));
	TMyString::ReplaceLoop(expression, TEXT("+-"), TEXT("-"));
	TMyString::ReplaceLoop(expression, TEXT("-+"), TEXT("-"));

	TCHAR *start = expression;
	TCHAR *end = expression + _tcslen(expression);
	TCHAR *now = start;
	TCHAR *nowstart = start;
	TCHAR *nowend = start;

	//字符合法性检查
	while (now < end)
	{
		if (isLegal(*now))
			now++;
		else
			return ERROR_ILLEGALCHAR;
	}
	now = start;

	//粗切分：利用operator切分
	struct TStrPiece
	{
		bool bBaseOperator;
		TCHAR *start, *end;
	} tempPiece;
	std::vector<TStrPiece> Data;
	bool PrevIsBaseOperator = isBaseOperator(*now);
	nowstart = now;
	now++;
	while (now < end)
	{
		if (isBaseOperator(*now) != PrevIsBaseOperator)//发生跳变
		{
			tempPiece.bBaseOperator = PrevIsBaseOperator;
			tempPiece.start = nowstart;
			tempPiece.end = now;//结尾指向下一位，最后一项结尾指向\0
			Data.push_back(tempPiece);

			PrevIsBaseOperator = isBaseOperator(*now);
			nowstart = now;
		}
		now++;
	}
	tempPiece.bBaseOperator = PrevIsBaseOperator;
	tempPiece.start = nowstart;
	tempPiece.end = end;
	Data.push_back(tempPiece);

	//二次切分：切分出4类元素
	//并送入Pre In order
	std::vector<TNode *> PreInOrder;
	TNode *tempNode;
	TCHAR *tempTChar = NULL;
	enumMathOperator tempeOperator;
	for (UINT i = 0; i < Data.size(); i++)
	{
		if (Data[i].bBaseOperator)//识别出基本运算符（括号也在其中）
		{
			now = Data[i].start;
			while (now < Data[i].end)
			{
				tempNode = new TNode;
				ZeroMemory(tempNode, sizeof(TNode));
				tempNode->eType = NODE_OPERATOR;
				tempNode->eOperator = BaseOperatorCharToEnum(*now);
				PreInOrder.push_back(tempNode);
				now++;
			}
		}
		else//
		{
			//逐位检验是否为数字
			now = Data[i].start;
			bool isDouble = true;
			while (now < Data[i].end)
			{
				if (isDoubleChar(*now) == false)
				{
					isDouble = false;
					break;
				}
				now++;
			}

			if (isDouble)//数字
			{
				tempNode = new TNode;
				ZeroMemory(tempNode, sizeof(TNode));
				tempNode->eType = NODE_NUMBER;
				tempNode->ValueOrName.value = TTransfer::TCHAR2double(Data[i].start, Data[i].end);
				PreInOrder.push_back(tempNode);
			}
			else
			{
				if ((tempeOperator = Str2Function(Data[i].start, Data[i].end)) != MATH_NOT_AVAILIALBE)//识别出函数
				{
					tempNode = new TNode;
					ZeroMemory(tempNode, sizeof(TNode));
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
						return eError=ERROR_NOT_LINK_VARIABLETABLE;
					}
					if (TMyString::isAlphaCharOrUnderline(Data[i].start[0]) == false)//变量名首字符需为下划线或字母
					{
						ReleaseVectorTNode(PreInOrder);
						return eError = ERROR_INVALID_VARNAME;
					}

					//变量名存入tempTChar
					int len = Data[i].end - Data[i].start;
					tempTChar = new TCHAR[len + 1];
					_tcsncpy(tempTChar, Data[i].start, len);
					tempTChar[len] = TEXT('\0');

					//
					TCHAR *foundvar = NULL;
					if (foundvar = pVariableTable->FindVariableTable(tempTChar))
					{
						delete[] tempTChar;
						tempTChar = foundvar;
					}
					else
					{
						delete[] tempTChar;
						ReleaseVectorTNode(PreInOrder);
						return eError=ERROR_UNDEFINED_VARIABLE;
					}

					tempNode = new TNode;
					ZeroMemory(tempNode, sizeof(TNode));
					tempNode->eType = NODE_VARIABLE;
					tempNode->ValueOrName.varname = tempTChar;
					PreInOrder.push_back(tempNode);

					tempTChar = NULL;
				}
			}
		}
	}
	//此时4大元素均已切分入PreInOrder

	//识别取正运算符与取负运算符
	bool bFirstOrParenFirst = false;
	bool bAferOneOperator = false;
	int i = 0;
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
			i++;
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

	for (i = 0; i < PreInOrder.size(); i++)
	{
		InOrder.push(PreInOrder[i]);
	}

	return ERROR_NO;
}

TCHAR * TExpressionTree::GetErrorInfo()
{
	if (ErrorInfo != NULL)
		delete[] ErrorInfo;
	ErrorInfo = new TCHAR[64];
	switch (eError)
	{
	case ERROR_NO:
		_tcscpy(ErrorInfo, TEXT("操作成功完成。"));
		break;
	case ERROR_ILLEGALCHAR:
		_tcscpy(ErrorInfo, TEXT("错误：出现非法字符。"));
		break;
	case ERROR_PARENTHESIS_NOT_MATCH:
		_tcscpy(ErrorInfo, TEXT("错误：括号不匹配。"));
		break;
	case ERROR_INVALID_VARNAME:
		_tcscpy(ErrorInfo, TEXT("错误：不正确的变量名（必须以下划线\"_\"或英文字母开头）。"));
		break;
	case ERROR_WRONG_EXPRESSION:
		_tcscpy(ErrorInfo, TEXT("错误：错误的表达式。"));
		break;
	case ERROR_EMPTY_INPUT:
		_tcscpy(ErrorInfo, TEXT("表达式为空。"));
		break;
	case ERROR_DIVIDE_ZERO:
		_tcscpy(ErrorInfo, TEXT("错误：不得除以0。"));
		break;
	case ERROR_UNDEFINED_VARIABLE:
		_tcscpy(ErrorInfo, TEXT("错误：未定义的变量。"));
		break;
	case ERROR_ZERO_POWEROF_ZERO:
		_tcscpy(ErrorInfo, TEXT("错误：0的0次方。"));
		break;
	case ERROR_SUBS_NOT_EQUAL:
		_tcscpy(ErrorInfo, TEXT("错误：替换与被替换数目不等。"));
		break;
	case ERROR_NOT_LINK_VARIABLETABLE:
		_tcscpy(ErrorInfo, TEXT("程序错误：未链接变量表。"));
		break;
	case ERROR_OUTOF_DOMAIN:
		_tcscpy(ErrorInfo, TEXT("错误：超出定义域。"));
		break;
	case ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT:
		_tcscpy(ErrorInfo, TEXT("错误：变量名与初始值数量不对等。"));
		break;
	case ERROR_I:
		_tcscpy(ErrorInfo, TEXT("暂不支持虚数。"));
		break;
	default:
		_tcscpy(ErrorInfo, TEXT("undefined error"));
		break;
	}
	return ErrorInfo;
}

bool TExpressionTree::IsIntAndEven(double n)
{
	long long i = (long long)n;
	if (abs(n - i) < MIN_DOUBLE)
		if (i % 2 == 0)
			return true;
	return false;
}

void TExpressionTree::CalcNode(TNode *Operator,const TNode *Node1,const TNode *Node2 = NULL)
{
	double value1 = Node1->ValueOrName.value;
	double value2 = (Node2 != NULL ? Node2->ValueOrName.value : 0.0);
	Operator->eType = NODE_NUMBER;
	switch (Operator->eOperator)
	{
	case MATH_SQRT:
		if (value1 < 0.0)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw eError = ERROR_I;
		}
		Operator->ValueOrName.value = sqrt(value1);
		break;
	case MATH_SIN:
		Operator->ValueOrName.value = sin(value1);
		break;
	case MATH_COS:
		Operator->ValueOrName.value = cos(value1);
		break;
	case MATH_TAN:
	{
		//x!=k*pi+pi/2 -> 2*x/pi != 2*k+1(odd)
		double value = value1*2.0 / M_PI;
		if (abs(value - (int)value) < MIN_DOUBLE && (int)value % 2 != 1)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw eError = ERROR_OUTOF_DOMAIN;
		}
		Operator->ValueOrName.value = tan(value1);
		break;
	}
	case MATH_ARCSIN:
		if (value1 < -1.0 || value1 > 1.0)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw eError = ERROR_OUTOF_DOMAIN;
		}
		Operator->ValueOrName.value = asin(value1);
		break;
	case MATH_ARCCOS:
		if (value1 < -1.0 || value1 > 1.0)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw eError = ERROR_OUTOF_DOMAIN;
		}
		Operator->ValueOrName.value = acos(value1);
		break;
	case MATH_ARCTAN:
		Operator->ValueOrName.value = atan(value1);
		break;
	case MATH_LN:
		if (value1 < MIN_DOUBLE)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw eError = ERROR_OUTOF_DOMAIN;
		}
		Operator->ValueOrName.value = log(value1);
		break;
	case MATH_LOG10:
		if (value1 < MIN_DOUBLE)
		{
			//恢复修改并抛出异常
			Operator->eType = NODE_FUNCTION;
			throw eError = ERROR_OUTOF_DOMAIN;
		}
		Operator->ValueOrName.value = log10(value1);
		break;
	case MATH_EXP:
		Operator->ValueOrName.value = exp(value1);
		break;
	case MATH_POSITIVE:
		break;
	case MATH_NEGATIVE:
		Operator->ValueOrName.value = -value1;
		break;

	case MATH_MOD://%
		Operator->ValueOrName.value = (int)value1 % (int)value2;
		break;
	case MATH_AND://&
		Operator->ValueOrName.value = (int)value1 & (int)value2;
		break;
	case MATH_OR://|
		Operator->ValueOrName.value = (int)value1 | (int)value2;
		break;

	case MATH_POWER://^
		//0^0
		if (abs(value1) < MIN_DOUBLE && abs(value2) < MIN_DOUBLE)
		{
			Operator->eType = NODE_OPERATOR;
			throw eError = ERROR_ZERO_POWEROF_ZERO;
		}

		//(-1)^0.5=i
		if (value1 < 0 && IsIntAndEven(1ll / value2))
		{
			Operator->eType = NODE_OPERATOR;
			throw eError = ERROR_I;
		}
		Operator->ValueOrName.value = pow(value1, value2);
		break;

	case MATH_MULTIPLY:
		Operator->ValueOrName.value = value1 * value2;
		break;
	case MATH_DIVIDE:
		if (abs(value2) < MIN_DOUBLE)
		{
			Operator->eType = NODE_OPERATOR;
			throw eError = ERROR_DIVIDE_ZERO;
		}
		Operator->ValueOrName.value = value1 / value2;
		break;

	case MATH_ADD:
		Operator->ValueOrName.value = value1 + value2;
		break;
	case MATH_SUBSTRACT:
		Operator->ValueOrName.value = value1 - value2;
		break;
	}
	Operator->eOperator = MATH_NOT_AVAILIALBE;
}


TExpressionTree::TNode * TExpressionTree::CopyNodeTree(TNode *oldNode)
{
	TNode *newNode = new TNode;
	ZeroMemory(newNode, sizeof(TNode));
	newNode->eType = oldNode->eType;
	newNode->eOperator = oldNode->eOperator;
	newNode->ValueOrName = oldNode->ValueOrName;

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

TExpressionTree::TNode * TExpressionTree::NewNode(enumNodeType eType, enumMathOperator eOperator)
{
	TNode *newNode = new TNode;
	ZeroMemory(newNode, sizeof(TNode));
	newNode->eType = eType;
	newNode->eOperator = eOperator;
	return newNode;
}

//未完成求导：tan,arcsin,arccos,arctan
void TExpressionTree::Diff(TNode *now, TCHAR *var)//var为指向变量的指针，一定位于变量表内
{
	switch (now->eType)
	{
	case NODE_VARIABLE:
		now->eType = NODE_NUMBER;
		if (now->ValueOrName.varname == var)
			now->ValueOrName.value = 1;
		else
			now->ValueOrName.value = 0;
		return;
	case NODE_NUMBER:
		now->ValueOrName.value = 0;
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
				return;
			}
			else
			{
				TNode *plus;
				plus = new TNode;
				ZeroMemory(plus, sizeof(TNode));
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
				return;
			}
		case MATH_DIVIDE:
			if (now->right->eType == NODE_NUMBER)// f(x)/number = f'(x)/number
			{
				Diff(now->left, var);
				return;
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
				num2->ValueOrName.value = 2;

				//连接^下面的v3和2
				power->left = v3;
				v3->parent = power;
				power->right = num2;
				num2->parent = power;

				Diff(u1, var);
				Diff(v2, var);

				return;
			}
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
				now->eOperator = MATH_NOT_AVAILIALBE;
				now->ValueOrName.value = 0.0;
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
				v->ValueOrName.value -= 1.0;

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
	case NODE_FUNCTION:
	{
		//不考虑定义域
		//函数内为数字则导为0
		if (now->left->eType == NODE_NUMBER)
		{
			now->eType = NODE_NUMBER;
			now->eOperator = MATH_NOT_AVAILIALBE;
			now->ValueOrName.value = 0;
			DeleteNode(now->left);
			now->left = NULL;
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
			num1half->ValueOrName.value = 0.5;

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
			num1->ValueOrName.value = 1.0;

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
				num10->ValueOrName.value = 10.0;

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

			return;
		}
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
			return;
		}
		case MATH_COS:

			TNode *negative;
			negative = new TNode;
			ZeroMemory(negative, sizeof(TNode));
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

		case MATH_SIN:
			TNode *multiply = new TNode;
			ZeroMemory(multiply, sizeof(TNode));
			multiply->eType = NODE_OPERATOR;
			multiply->eOperator = MATH_MULTIPLY;

			switch (function->eOperator)
			{
			case MATH_SIN:
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
				break;
			case MATH_COS:
				//连接负号和乘号
				negative->left = multiply;
				multiply->parent = negative;
				break;
			}

			//连接乘号和function
			multiply->left = function;
			function->parent = multiply;

			//变更function
			switch (function->eOperator)
			{
			case MATH_SIN:
				function->eOperator = MATH_COS;
				break;
			case MATH_COS:
				function->eOperator = MATH_SIN;
				break;
			}

			//复制u2并连接乘号
			TNode *u2 = CopyNodeTree(function->left);
			multiply->right = u2;
			u2->parent = multiply;

			Diff(u2, var);
			return;
		}
	}
	}
}

TCHAR * TExpressionTree::Diff(TCHAR *var, int n, bool bOutput)
{
	if (eError == ERROR_NO)
	{
		TCHAR *Variable = pVariableTable->FindVariableTable(var);
		//没有找到对应变量则设置为空，即无关变量导成0

		for (int i = 0; i < n; i++)
		{
			Diff(head, Variable);
		}
		//return Simplify(bOutput);
		return OutputStr();
	}

	return GetErrorInfo();
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

enumError TExpressionTree::Simplify(TNode *now)
{
	enumError nowError = ERROR_NO;
	//左遍历
	if (now->left != NULL)
		nowError = Simplify(now->left);

	if (nowError != ERROR_NO)
		return nowError;

	//右遍历
	if (now->right != NULL)
		nowError = Simplify(now->right);

	if (nowError != ERROR_NO)
		return nowError;

	//化简
	//OutputStr();
	if (GetOperateNum(now->eOperator) == 1)
	{
		bool ChildIs0 = (now->left->eType == NODE_NUMBER && abs(now->left->ValueOrName.value) < MIN_DOUBLE);
		bool ChildIs1 = (now->left->eType == NODE_NUMBER && abs(now->left->ValueOrName.value - 1) < MIN_DOUBLE);

		//sin(0)=0
		if (now->eOperator == MATH_SIN && ChildIs0)
		{
			LinkParent(now->left, now);
			now->left->ValueOrName.value = 0;
			delete now;
		}

		//cos(0)=1
		if (now->eOperator == MATH_COS && ChildIs0)
		{
			LinkParent(now->left, now);
			now->left->ValueOrName.value = 1;
			delete now;
		}

		if (now->eOperator == MATH_NEGATIVE && now->left->eType == NODE_NUMBER)
		{
			TNode *negative = now;
			TNode *num = now->left;
			LinkParent(num, negative);
			num->ValueOrName.value = -num->ValueOrName.value;
			delete negative;
		}
	}

	if (GetOperateNum(now->eOperator) == 2)
	{
		//下列每种情况必须互斥，因为每个情况都有返回值，涉及删除操作，若不返回连续执行将导致指针出错
		//不检查左右儿子是否存在，因为此处本身就是2元运算符
		bool LChildIs0 = (now->left->eType == NODE_NUMBER && abs(now->left->ValueOrName.value) < MIN_DOUBLE);
		bool RChildIs0 = (now->right->eType == NODE_NUMBER && abs(now->right->ValueOrName.value) < MIN_DOUBLE);
		bool LChildIs1 = (now->left->eType == NODE_NUMBER && abs(now->left->ValueOrName.value - 1) < MIN_DOUBLE);
		bool RChildIs1 = (now->right->eType == NODE_NUMBER && abs(now->right->ValueOrName.value - 1) < MIN_DOUBLE);

		//被除数为0
		if (now->eOperator == MATH_DIVIDE && RChildIs0)
			return nowError = ERROR_DIVIDE_ZERO;

		//0的0次方
		if (now->eOperator == MATH_POWER && LChildIs0 && RChildIs0)
			return nowError = ERROR_ZERO_POWEROF_ZERO;

		//若左右儿子都是数字，则计算出来
		if (now->left->eType == NODE_NUMBER && now->right->eType == NODE_NUMBER)
		{
			try
			{
				CalcNode(now, now->left, now->right);
			delete now->left;
			delete now->right;
			now->eOperator = MATH_NOT_AVAILIALBE;
			now->left = NULL;
			now->right = NULL;
			return ERROR_NO;
			}
			catch (enumError err)
			{
				return nowError = err;
			}
		}

		//任何节点的0次方均等于1，除了0的0次方已在前面报错
		if (now->eOperator == MATH_POWER && RChildIs0)
		{
			//替换掉当前运算符，这个1节点将在回溯时处理
			//新建一个1节点
			TNode *temp;
			temp = new TNode;
			ZeroMemory(temp, sizeof(TNode));
			temp->eType = NODE_NUMBER;
			temp->ValueOrName.value = 1;

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
			return ERROR_NO;
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
			ZeroMemory(temp, sizeof(TNode));
			temp->eType = NODE_NUMBER;
			temp->ValueOrName.value = 0;

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
			return ERROR_NO;
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
			return ERROR_NO;
		}


	}

	//OutputStr();
	return nowError;
}

TCHAR * TExpressionTree::OutputEmptyStr()
{
	if (szOutput != NULL)
		delete[] szOutput;
	szOutput = new TCHAR[1];
	_tcscpy(szOutput, TEXT(""));
	return szOutput;
}

TCHAR * TExpressionTree::Simplify(bool bOutput)
{
	if (eError == ERROR_NO)
	{
		TNode *temp = head;
		if ((eError = Simplify(temp)) == ERROR_NO)
			if (bOutput)
				return OutputStr();
			else
				return OutputEmptyStr();
	}

	return GetErrorInfo();
}

TCHAR * TExpressionTree::FindVariableTableFrom(TCHAR *varstr, std::vector<TCHAR *> newVariableTable)
{
	for (int i = 0; i < newVariableTable.size(); i++)
		if (_tcscmp(varstr, newVariableTable[i]) == 0)
			return newVariableTable[i];
	return NULL;
}

void TExpressionTree::ReplaceNodeVariable(TNode *now, std::vector<TCHAR *> &newVariableTable)
{
	if (now->left != NULL)
		ReplaceNodeVariable(now->left, newVariableTable);
	if (now->right != NULL)
		ReplaceNodeVariable(now->right, newVariableTable);
	TCHAR *temp;
	if (now->eType == NODE_VARIABLE && (temp = FindVariableTableFrom(now->ValueOrName.varname, newVariableTable)))
		now->ValueOrName.varname = temp;
}

void TExpressionTree::GetVariablePos(const TCHAR *var, std::vector<TNode *> &VarsPos)
{
	GetVariablePos(head, var, VarsPos);
}

void TExpressionTree::GetVariablePos(TNode *now, const TCHAR *var, std::vector<TNode *> &VarsPos)
{
	if (now->eType == NODE_VARIABLE && now->ValueOrName.varname == var)
		VarsPos.push_back(now);
	if (now->left != NULL)
		GetVariablePos(now->left, var, VarsPos);
	if (now->right != NULL)
		GetVariablePos(now->right, var, VarsPos);
}

void TExpressionTree::CopyVariableTable(std::vector<TCHAR *> &Dest, const std::vector<TCHAR *> source)
{
	Dest.clear();
	for (int i = 0; i < source.size(); i++)
		Dest.push_back(source[i]);
}

//替换 VarsVector变量 NumsVector数字
TCHAR * TExpressionTree::Subs(std::vector<TCHAR *> VarsVector, std::vector<double> NumsVector, bool output)
{
	if (eError == ERROR_NO)
	{
		if (VarsVector.size() == NumsVector.size())//替换与被替换元素数目相等
		{
			for (int i = 0; i < VarsVector.size(); i++)//遍历被替换变量
			{
				//查表识别被替换变量
				TCHAR *var = pVariableTable->FindVariableTable(VarsVector[i]);
				if (var != NULL)//已识别出
				{
					//构建替换节点树
					TExpressionTree Expr;
					Expr.LinkVariableTable(pVariableTable);
					Expr.Read(NumsVector[i], false);
					if (Expr.eError == ERROR_NO)
					{

						//得到所有被替换变量的位置
						std::vector<TNode *> VarsPos;
						GetVariablePos(var, VarsPos);
						for (int j = 0; j < VarsPos.size(); j++)
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
					else
						eError = Expr.eError;
				}
			}
		}
		else
			eError = ERROR_SUBS_NOT_EQUAL;

		if (eError == ERROR_NO)
		{
			if (output)
				return OutputStr();
			else
				return OutputEmptyStr();
		}
	}
	return GetErrorInfo();
}

//替换  vars变量串 nums数字串 以空格分隔，支持表达式替换
TCHAR * TExpressionTree::Subs(TCHAR *vars, TCHAR *nums, bool output)
{
	if (eError == ERROR_NO)
	{
		if (_tcslen(vars) == 0 || _tcslen(nums) == 0)
		{
			eError = ERROR_EMPTY_INPUT;
			return GetErrorInfo();
		}
		TCHAR *nVars = new TCHAR[_tcslen(vars) + 1];
		TCHAR *nNums = new TCHAR[_tcslen(nums) + 1];
		_tcscpy(nVars, vars);
		_tcscpy(nNums, nums);
		TMyString::Trim(nVars);
		TMyString::Trim(nNums);
		TMyString::ReplaceLoop(nVars, TEXT("  "), TEXT(" "));
		TMyString::ReplaceLoop(nNums, TEXT("  "), TEXT(" "));
		std::vector<TCHAR *> VarsVector;
		std::vector<TCHAR *> NumsVector;
		TMyString::Split(nVars, VarsVector, TEXT(" "));
		TMyString::Split(nNums, NumsVector, TEXT(" "));

		if (VarsVector.size() == NumsVector.size())//替换与被替换元素数目相等
		{
			for (int i = 0; i < VarsVector.size(); i++)//遍历被替换变量
			{
				//查表识别被替换变量
				TCHAR *var = pVariableTable->FindVariableTable(VarsVector[i]);
				if (var != NULL)//已识别出
				{
					//构建替换节点树
					TExpressionTree Expr;
					Expr.LinkVariableTable(pVariableTable);
					Expr.Read(NumsVector[i], false);
					if (Expr.eError == ERROR_NO)
					{

						//得到所有被替换变量的位置
						std::vector<TNode *> VarsPos;
						GetVariablePos(var, VarsPos);
						for (int j = 0; j < VarsPos.size(); j++)
						{
							TNode *newNode = CopyNodeTree(Expr.head);
							//ReplaceNodeVariable(newNode, newVariableTable);

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
					else
						eError = Expr.eError;
				}
			}
		}
		else
			eError = ERROR_SUBS_NOT_EQUAL;

		//FinishSubs:
		delete[] nVars;
		delete[] nNums;
		for (int i = 0; i < VarsVector.size(); i++)
			delete[] VarsVector[i];
		for (int i = 0; i < NumsVector.size(); i++)
			delete[] NumsVector[i];

		if (eError == ERROR_NO)
		{
			if (output)
				return OutputStr();
			else
				return OutputEmptyStr();
		}
	}
	return GetErrorInfo();
}

TCHAR * TExpressionTree::LinkVariableTable(TVariableTable *p)
{
	if (p->eError != ERROR_NO)
		eError = p->eError;

	if (eError == ERROR_NO)
	{
		pVariableTable = p;
		return OutputEmptyStr();
	}

	return GetErrorInfo();
}

TCHAR * TExpressionTree::Read(double num, bool bOutput)
{
	head = NewNode(NODE_NUMBER);
	head->ValueOrName.value = num;
	if (bOutput)
		return OutputStr();
	else
		return OutputEmptyStr();
}

TCHAR * TExpressionTree::Solve()
{
	TExpressionTree Result;

	TNode *Now, *ResultNow;
	return OutputEmptyStr();

}

TCHAR * TExpressionTree::Read(TCHAR *expression, bool bOutput)
{
	std::queue<TNode *> InOrder;
	std::vector<TNode *> PostOrder;
	TCHAR *expr = new TCHAR[_tcslen(expression) + 1];
	_tcscpy(expr, expression);
	expr[_tcslen(expression)] = TEXT('\0');

	if (eError == ERROR_NO)
		if ((eError = ReadToInOrder(expr, InOrder)) == ERROR_NO)
			if ((eError = InQueue2PostQueue(InOrder, PostOrder)) == ERROR_NO)
				if ((eError = BuildExpressionTree(PostOrder)) == ERROR_NO)
				{
					delete[] expr;
					if (bOutput)
						return OutputStr();
					else
						return OutputEmptyStr();
				}

	delete[] expr;
	return GetErrorInfo();
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

double TExpressionTree::Value(bool operateHeadNode)
{
	if (eError == ERROR_NO)
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
		double num = pNode->ValueOrName.value;
		//释放复制的树
		if (operateHeadNode == false)
			delete pNode;
		return num;
	}
}

TCHAR * TExpressionTree::Calc(double *result)
{
	if (eError == ERROR_NO)
	{
		if (CanCalc())
		{
			TNode *Duplicate = CopyNodeTree(head);
			Calc(Duplicate);

			if (result != NULL)
				*result = Duplicate->ValueOrName.value;

			Node2Str(*Duplicate, szOutput);
			delete Duplicate;

			return szOutput;
		}
		else
			return OutputStr();
	}
	return GetErrorInfo();
}

TExpressionTree& TExpressionTree::operator=(TExpressionTree &expr)
{
	Release();
	Reset();
	head = CopyNodeTree(expr.head);
	LinkVariableTable(expr.pVariableTable);
	return *this;
}

void TExpressionTree::ReleaseVectorTNode(std::vector<TNode *> vec)
{
	for (int i = 0; i < vec.size(); i++)
		delete vec[i];
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
		now->ValueOrName.value = pVariableTable->GetValueFromVarPoint(now->ValueOrName.varname);
	}
}

TCHAR * TExpressionTree::Vpa(bool bOutput)
{
	if (eError == ERROR_NO)
	{
		Vpa_inner(head);
		if (eError == ERROR_NO)
		{
			if (bOutput)
				return OutputStr();
			else
				return OutputEmptyStr();
		}
	}
	return GetErrorInfo();
}