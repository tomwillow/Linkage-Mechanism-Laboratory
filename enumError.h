#pragma once
#include "Common\String.h"

enum enumError{
	ERROR_NO,
	ERROR_WRONG_MATH_OPERATOR,//eOperatorֵ����ͨ�����ڴ���ƫ����
	ERROR_ILLEGALCHAR,//���ַǷ��ַ�
	ERROR_PARENTHESIS_NOT_MATCH,//���Ų�ƥ��
	ERROR_INVALID_VARNAME,//��Ч������
	ERROR_WRONG_EXPRESSION,//���ʽ�߼�����ȷ
	ERROR_EMPTY_INPUT,//���ʽΪ��
	ERROR_DIVIDE_ZERO,//��0
	ERROR_UNDEFINED_VARIABLE,//δ����ı���
	ERROR_ZERO_POWEROF_ZERO,//0^0
	ERROR_SUBS_NOT_EQUAL,//�滻�뱻�滻�������Ե�
	ERROR_NOT_LINK_VARIABLETABLE,//δ���ӱ�����
	ERROR_OUTOF_DOMAIN,//���㳬��������
	ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT,//�������ʱ�����������ʼֵ����
	ERROR_VAR_HAS_BEEN_DEFINED,//�����ض���
	ERROR_I,//��������
	ERROR_INDETERMINATE_EQUATION,//��������
	ERROR_SINGULAR_MATRIX,//��������
	ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW,//A��������������b������
	ERROR_INFINITY_SOLUTIONS,//������
	ERROR_OVER_DETERMINED_EQUATIONS//�����������
};

struct TError :public std::exception
{
	enumError id;
	String info;
	TError(const enumError inId, const String &inInfo) :id(inId), info(inInfo){}
};

String GetErrorInfo(enumError err);