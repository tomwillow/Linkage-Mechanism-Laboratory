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
		MATH_NOT_AVAILIALBE,
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

	enum enumNodeType{ NODE_NUMBER, NODE_OPERATOR, NODE_VARIABLE, NODE_FUNCTION };

	/* ����Ԫ�� */
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

	//��������ʺ���
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
	void TExpressionTree::CalcNode(TNode *Operator, const TNode *Node1, const TNode *Node2);//����ڵ�ֵ��֧������������ͺ�����������Operator
	void TExpressionTree::Simplify(TNode *now);
	void TExpressionTree::GetNodeNum(TNode *now, int &n);
	int TExpressionTree::GetNodeNum(TNode *head);
	void TExpressionTree::DeleteNode(TNode *node);
	void TExpressionTree::DeleteNodeTraversal(TNode *node);
	void TExpressionTree::Diff(TNode *now, String var);
	TExpressionTree::TNode * TExpressionTree::CopyNodeTree(TNode *oldNode);
	//String  TExpressionTree::FindVariableTableFrom(const String varstr, std::vector<String > newVariableTable);//���ұ����Ƿ��ڱ������У�û���򷵻�NULL
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
	void  TExpressionTree::LinkVariableTable(TVariableTable *p);//���ӱ�����
	void  TExpressionTree::Read(const String expression, bool bOutput);
	void  TExpressionTree::Read(double num, bool bOutput);//����ֻ��1�����ֵı��ʽ

	//���в�����δ���
	String  TExpressionTree::Solve(String &var, double &value);//��ⵥ�������� ����֤������ԣ�����ǰ����HasOnlyOneVarȷ�� ���Ķ����ʽ����
	String  TExpressionTree::OutputStr();
	void  TExpressionTree::Simplify(bool bOutput);//����
	String  TExpressionTree::Diff(String var, int n, bool bOutput);//�Ա�����
	void  TExpressionTree::Subs(String ptVar, double value, bool output);
	void  TExpressionTree::Subs(const String vars,const String nums, bool output);//varsΪ���滻������numsΪ�滻���ʽ���Կո�ָ�
	void  TExpressionTree::Subs(std::vector<String > VarsVector, std::vector<double> NumsVector, bool output);
	bool TExpressionTree::CanCalc();//����Ƿ��б������ڣ����Լ����򷵻�true
	bool TExpressionTree::IsSingleVar();//����Ƿ�ΪһԪ(not used)
	bool TExpressionTree::HasOnlyOneVar();//ֻ��һ��������ֻ�и�read����Ч��
	bool TExpressionTree::CheckOnlyOneVar();//ֻ��һ��������ʵʱ��֤��
	double TExpressionTree::Value(bool operateHeadNode);//����֤�ɼ����ԣ�������CanCalc����
	String  TExpressionTree::Calc(double *result = NULL);//������ʽ��ֵ����������result��ѽ�����롣����ֵΪ����ַ�������ʽ����

	TExpressionTree& TExpressionTree::operator=(const TExpressionTree &expr);
	TExpressionTree& TExpressionTree::operator+(const TExpressionTree &expr);
	TExpressionTree & TExpressionTree::operator*(double value);
	TExpressionTree & TExpressionTree::operator+(double value);
	TExpressionTree & TExpressionTree::operator-(double value);

	TExpressionTree();
	~TExpressionTree();
};
