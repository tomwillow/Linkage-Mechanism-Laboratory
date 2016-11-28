#pragma once
#include "TExpressionTree.h"
#include <Windows.h>
#include "TTransfer.h"

TExpressionTree::TExpressionTree()
{
	head = NULL;
	ErrorInfo = NULL;
	szPostOrder = NULL;
	szInOrder = NULL;
}


TExpressionTree::~TExpressionTree()
{
	if (ErrorInfo != NULL)
		delete[] ErrorInfo;

	if (szPostOrder != NULL)
		delete[] szPostOrder;

	if (szInOrder != NULL)
		delete[] szInOrder;

	for (UINT i = 0; i < VariableTable.size(); i++)
	{
		delete[] VariableTable[i];
	}
}

TExpressionTree::enumError TExpressionTree::BuildExpressionTree()
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
				PostOrder[i]->right = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();
				PostOrder[i]->left = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();

				tempStack.push(PostOrder[i]);
			}
			else
			{
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
		//case MATH_SQRT:
		//case MATH_SIN:
		//case MATH_COS:
		//case MATH_TAN:
		//case MATH_ARCSIN:
		//case MATH_ARCCOS:
		//case MATH_ARCTAN:
		//case MATH_LEFT_PARENTHESIS:
		//case MATH_RIGHT_PARENTHESIS:
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
	if (isAlphaCharOrUnderline(c)) return true;
	return false;
}

/* 字符是a-zA-z或_ */
bool TExpressionTree::isAlphaCharOrUnderline(TCHAR c)
{
	if ((c >= TEXT('a') && c <= TEXT('z')) || (c >= TEXT('A') && c <= TEXT('Z'))
		|| c == TEXT('_'))
		return true;
	else
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
TExpressionTree::enumError TExpressionTree::InQueue2PostQueue()//返回0:正常 1:括号不配对
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
					//pop至左括号
					while (temp.size() > 0)
					{
						if (temp.top()->eOperator == MATH_LEFT_PARENTHESIS)//(
						{
							temp.pop();//扔掉左括号
							parenthesis_num--;
							break;
						}
						else
						{
							PostOrder.push_back(temp.top());//入队
							temp.pop();
						}
					}

					//取出函数
					if (temp.top()->eType == NODE_FUNCTION)
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
	while (temp.size() > 0)
	{
		PostOrder.push_back(temp.top());
		temp.pop();
	}
	if (parenthesis_num > 0)
		return ERROR_PARENTHESIS_NOT_MATCH;
	else
		return ERROR_NO;
}

void TExpressionTree::Node2Str(TNode &node, TCHAR *result)
{
	switch (node.eType)
	{
	case NODE_NUMBER:
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
	if (GetOperateNum(now->eOperator) == 1)//一元运算符
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

	if (now->left != NULL)//左遍历
	{
		TraverseInOrder(now->left, output, buffer);
	}

	if (GetOperateNum(now->eOperator) != 1)//非一元运算符才输出，即一元运算符的输出顺序已改变
	{
		if (now->eType == NODE_OPERATOR)//本级为运算符
			if (
				(now->left != NULL && now->left->eType == NODE_OPERATOR && Rank(now->eOperator) > Rank(now->left->eOperator))
				||
				(now->right != NULL && now->right->eType == NODE_OPERATOR && Rank(now->eOperator) > Rank(now->right->eOperator))
				)//左子树或右子树运算符存在且优先级低于本级
			{
				_tcscat(output, TEXT("("));
				has_parenthesis = 1;
			}
		Node2Str(*now, buffer);
		_tcscat(output, buffer);
	}

	if (now->right != NULL)//右遍历
	{
		TraverseInOrder(now->right, output, buffer);
	}

	//回到本级时补齐右括号
	if (has_parenthesis)
	{
		_tcscat(output, TEXT(")"));
	}
}

TCHAR * TExpressionTree::OutputStr()
{
	if (eError == ERROR_NO)
	{
		TCHAR buffer[MAX_VAR_NAME];
		if (szInOrder != NULL)
			delete[] szInOrder;
		szInOrder = new TCHAR[MAX_VAR_NAME * PostOrder.size()];
		szInOrder[0] = TEXT('\0');
		if (head != NULL)
			TraverseInOrder(head, szInOrder, buffer);
		return szInOrder;
	}
	else
		return GetErrorInfo(eError);
}

TCHAR * TExpressionTree::OutputPostOrderStr()
{
	if (eError == ERROR_NO)
	{
		TCHAR buffer[MAX_VAR_NAME];
		if (szPostOrder != NULL)
			delete[] szPostOrder;
		szPostOrder = new TCHAR[MAX_VAR_NAME * PostOrder.size()];
		szPostOrder[0] = TEXT('\0');
		for (int i = 0; i < PostOrder.size(); i++)
		{
			Node2Str(*PostOrder[i], buffer);
			_tcscat(szPostOrder, buffer);
		}
		return szPostOrder;
	}
	else
		return GetErrorInfo(eError);
}

TExpressionTree::enumError TExpressionTree::ReadToInOrder(TCHAR *expression)
{
	if (_tcslen(expression) == 0)
		return ERROR_EMPTY_INPUT;
	Replace(expression, TEXT(" "), TEXT(""));
	Replace(expression, TEXT("\t"), TEXT(""));
	Replace(expression, TEXT("\n"), TEXT(""));

	//过滤掉所有多余的加减
	while (1)
	{
		if (Replace(expression, TEXT("--"), TEXT("+")) == 0 &&
			Replace(expression, TEXT("+-"), TEXT("-")) == 0 &&
			Replace(expression, TEXT("-+"), TEXT("-")) == 0)
			break;
	}

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
					if (isAlphaCharOrUnderline(Data[i].start[0]) == false)//变量名首字符需为下划线或字母
						return ERROR_INVALID_VARNAME;

					//变量名存入tempTChar
					int len = Data[i].end - Data[i].start;
					tempTChar = new TCHAR[len + 1];
					_tcsncpy(tempTChar, Data[i].start, len);
					tempTChar[len] = TEXT('\0');

					VariableTable.push_back(tempTChar);

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

	//二元运算符不可连续出现，二元运算符不可出现在字符串首尾或左括号后第一位或右括号前一位，函数后必须带一个括号

	for (i = 0; i < PreInOrder.size(); i++)
	{
		InOrder.push(PreInOrder[i]);
	}

	return ERROR_NO;
}

TCHAR * TExpressionTree::GetErrorInfo(TExpressionTree::enumError eError)
{
	if (ErrorInfo != NULL)
		delete[] ErrorInfo;
	ErrorInfo = new TCHAR[64];
	switch (eError)
	{
	case ERROR_NO:
		_tcscpy(ErrorInfo, TEXT("没有错误。"));
		break;
	case ERROR_ILLEGALCHAR:
		_tcscpy(ErrorInfo, TEXT("出现非法字符。"));
		break;
	case ERROR_PARENTHESIS_NOT_MATCH:
		_tcscpy(ErrorInfo, TEXT("括号不匹配。"));
		break;
	case ERROR_INVALID_VARNAME:
		_tcscpy(ErrorInfo, TEXT("不正确的变量名（必须以下划线\"_\"或英文字母开头）。"));
		break;
	case ERROR_EMPTY_INPUT:
		_tcscpy(ErrorInfo, TEXT("表达式为空。"));
		break;
	}
	return ErrorInfo;
}

void TExpressionTree::CalcNode(TNode *Operator, TNode *Node1, TNode *Node2 = NULL)
{
	Operator->eType = NODE_NUMBER;
	switch (Operator->eOperator)
	{
	case MATH_SQRT:
		Operator->ValueOrName.value = sqrt(Node1->ValueOrName.value);
		break;
	case MATH_SIN:
		Operator->ValueOrName.value = sin(Node1->ValueOrName.value);
		break;
	case MATH_COS:
		Operator->ValueOrName.value = cos(Node1->ValueOrName.value);
		break;
	case MATH_TAN:
		Operator->ValueOrName.value = tan(Node1->ValueOrName.value);
		break;
	case MATH_ARCSIN:
		Operator->ValueOrName.value = asin(Node1->ValueOrName.value);
		break;
	case MATH_ARCCOS:
		Operator->ValueOrName.value = acos(Node1->ValueOrName.value);
		break;
	case MATH_ARCTAN:
		Operator->ValueOrName.value = atan(Node1->ValueOrName.value);
		break;
	case MATH_POSITIVE:
		break;
	case MATH_NEGATIVE:
		Operator->ValueOrName.value = -Node1->ValueOrName.value;
		break;

	case MATH_MOD://%
		Operator->ValueOrName.value = (int)Node1->ValueOrName.value % (int)Node2->ValueOrName.value;
		break;
	case MATH_AND://&
		Operator->ValueOrName.value = (int)Node1->ValueOrName.value & (int)Node2->ValueOrName.value;
		break;
	case MATH_OR://|
		Operator->ValueOrName.value = (int)Node1->ValueOrName.value | (int)Node2->ValueOrName.value;
		break;

	case MATH_POWER://^
		Operator->ValueOrName.value = pow(Node1->ValueOrName.value, Node2->ValueOrName.value);
		break;

	case MATH_MULTIPLY:
		Operator->ValueOrName.value = Node1->ValueOrName.value * Node2->ValueOrName.value;
		break;
	case MATH_DIVIDE:
		Operator->ValueOrName.value = Node1->ValueOrName.value / Node2->ValueOrName.value;
		break;

	case MATH_ADD:
		Operator->ValueOrName.value = Node1->ValueOrName.value + Node2->ValueOrName.value;
		break;
	case MATH_SUBSTRACT:
		Operator->ValueOrName.value = Node1->ValueOrName.value - Node2->ValueOrName.value;
		break;
	}
}

void TExpressionTree::Simplify(TNode *now)
{
	if (now->left != NULL)
		Simplify(now->left);


	if (now->right != NULL)
		Simplify(now->right);

	if (GetOperateNum(now->eOperator) == 2)
	{
		if (now->left->eType == NODE_NUMBER && now->right->eType == NODE_NUMBER)
		{
			CalcNode(now, now->left, now->right);
			delete now->left;
			delete now->right;
			now->eOperator = MATH_NOT_AVAILIALBE;
			now->left = NULL;
			now->right = NULL;
		}
	}
}

TCHAR * TExpressionTree::Simplify()
{
	if (eError == ERROR_NO)
	{
		TNode *temp = head;
		Simplify(head);

		return OutputStr();
	}
	else
		return GetErrorInfo(eError);
}

TCHAR * TExpressionTree::Read(TCHAR *expression)
{
	if ((eError = ReadToInOrder(expression)) == ERROR_NO)
		if ((eError = InQueue2PostQueue()) == ERROR_NO)
			if ((eError = BuildExpressionTree()) == ERROR_NO)
				return OutputStr();

	return GetErrorInfo(eError);
}

int Replace(TCHAR *src, TCHAR *sub, TCHAR *dest)
{
	int srclen = _tcslen(src);
	if (srclen == 0) return 0;
	int sublen = _tcslen(sub);
	if (sublen == 0) return 0;
	int destlen = _tcslen(dest);
	std::vector<TCHAR *> pos;//保存寻找到的位置

	//得到所有位置
	TCHAR *temp = src;
	TCHAR *now = NULL;
	while ((now = _tcsstr(temp, sub)) != NULL)
	{
		pos.push_back(now);
		temp = now + sublen;
	}

	if (pos.size() == 0) return 0;
	int newsize = srclen + pos.size()*(destlen - sublen) + 1;//新串的大小，加上末尾的\0
	TCHAR *newsrc = (TCHAR *)malloc(newsize*sizeof(TCHAR));
	newsrc[0] = TEXT('');
	TCHAR *prevpos = src;
	for (UINT i = 0; i < pos.size(); i++)
	{
		_tcsncat(newsrc, prevpos, pos[i] - prevpos);
		_tcsncat(newsrc, dest, destlen);
		prevpos = pos[i] + sublen;
	}
	_tcsncat(newsrc, prevpos, srclen - (prevpos - src));
	//此处newsrc已全部拷完

	_tcscpy(src, newsrc);
	free(newsrc);
	return pos.size();
}