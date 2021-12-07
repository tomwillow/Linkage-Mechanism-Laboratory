#pragma once

#include <queue>
#include <stack>
#include "..\Solver\TVariableTable.h"
#include "..\enumError.h"

class TExpressionTree
{
private:
#define MAX_VAR_NAME 32//ͬʱҲ�Ǹ�����ת�ַ�������󳤶�
#define MIN_DOUBLE  1e-6
	enum enumMathOperator{
		MATH_NULL,
		//һԪ
		MATH_POSITIVE, MATH_NEGATIVE,

		//����
		MATH_SIN, MATH_COS, MATH_TAN,
		MATH_ARCSIN, MATH_ARCCOS, MATH_ARCTAN, MATH_SQRT,
		MATH_LN, MATH_LOG10, MATH_EXP,

		//��Ԫ
		MATH_ADD, MATH_SUBSTRACT,
		MATH_MULTIPLY, MATH_DIVIDE,
		MATH_POWER, MATH_AND, MATH_OR, MATH_MOD,

		MATH_LEFT_PARENTHESIS, MATH_RIGHT_PARENTHESIS
	};

	enum enumNodeType{ NODE_NULL, NODE_NUMBER, NODE_OPERATOR, NODE_VARIABLE, NODE_FUNCTION };

	/* ����Ԫ�� */
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

	//��������ʺ���
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
	void CalcNode(TNode *Operator, const TNode *Node1, const TNode *Node2);//����ڵ�ֵ��֧������������ͺ�����������Operator
	void Simplify(TNode *now);
	void GetNodeNum(TNode *now, int &n);
	int GetNodeNum(TNode *head);
	void DeleteNode(TNode *node);
	void DeleteNodeTraversal(TNode *node);
	void Diff(TNode *now, String var);
	TNode * CopyNodeTree(TNode *oldNode);
	//String  FindVariableTableFrom(const String varstr, std::vector<String > newVariableTable);//���ұ����Ƿ��ڱ������У�û���򷵻�NULL
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
	void  LinkVariableTable(TVariableTable *p);//���ӱ�����
	void  Read(const String expression, bool bOutput);
	void  Read(double num, bool bOutput);//����ֻ��1�����ֵı��ʽ

	//���в�����δ���
	String  Solve(String &var, double &value);//��ⵥ�������� ����֤������ԣ�����ǰ����HasOnlyOneVarȷ�� ���Ķ����ʽ����
	String  OutputStr();
	void  Simplify(bool bOutput);//����
	String  Diff(String var, int n, bool bOutput);//�Ա�����
	void  Subs(String ptVar, double value, bool output);
	void  Subs(const String vars,const String nums, bool output);//varsΪ���滻������numsΪ�滻���ʽ���Կո�ָ�
	void  Subs(std::vector<String > VarsVector, std::vector<double> NumsVector, bool output);
	bool CanCalc();//����Ƿ��б������ڣ����Լ����򷵻�true
	bool IsSingleVar();//����Ƿ�ΪһԪ(not used)
	bool HasOnlyOneVar();//ֻ��һ��������ֻ�и�read����Ч��
	bool CheckOnlyOneVar();//ֻ��һ��������ʵʱ��֤��
	double Value(bool operateHeadNode);//����֤�ɼ����ԣ�������CanCalc����
	String  Calc(double *result = NULL);//������ʽ��ֵ����������result��ѽ�����롣����ֵΪ����ַ�������ʽ����

	TExpressionTree& operator=(const TExpressionTree &expr);
	TExpressionTree& operator+(const TExpressionTree &expr);
	TExpressionTree & operator*(double value);
	TExpressionTree & operator+(double value);
	TExpressionTree & operator-(double value);

	TExpressionTree();
	~TExpressionTree();
};
