#pragma once
#include "Common\String.h"

enum enumError{
	ERROR_NO,
	ERROR_WRONG_MATH_OPERATOR,//eOperator值错误，通常是内存跑偏引起
	ERROR_ILLEGALCHAR,//出现非法字符
	ERROR_PARENTHESIS_NOT_MATCH,//括号不匹配
	ERROR_INVALID_VARNAME,//无效变量名
	ERROR_WRONG_EXPRESSION,//表达式逻辑不正确
	ERROR_EMPTY_INPUT,//表达式为空
	ERROR_DIVIDE_ZERO,//除0
	ERROR_UNDEFINED_VARIABLE,//未定义的变量
	ERROR_ZERO_POWEROF_ZERO,//0^0
	ERROR_SUBS_NOT_EQUAL,//替换与被替换数量不对等
	ERROR_NOT_LINK_VARIABLETABLE,//未链接变量表
	ERROR_OUTOF_DOMAIN,//计算超出定义域
	ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT,//定义变量时变量数量与初始值不等
	ERROR_VAR_HAS_BEEN_DEFINED,//变量重定义
	ERROR_I,//出现虚数
	ERROR_INDETERMINATE_EQUATION,//不定方程
	ERROR_SINGULAR_MATRIX,//矩阵奇异
	ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW,//A矩阵行数不等于b向量数
	ERROR_INFINITY_SOLUTIONS,//无穷多解
	ERROR_OVER_DETERMINED_EQUATIONS//方程组过定义
};

struct TError :public std::exception
{
	enumError id;
	String info;
	TError::TError(const enumError inId, const String &inInfo) :id(inId), info(inInfo){}
};

String GetErrorInfo(enumError err);