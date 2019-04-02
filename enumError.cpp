#include "enumError.h"

String GetErrorInfo(enumError err)
{
	switch (err)
	{
	case ERROR_NO:
		return TEXT("操作成功完成。");
		break;
	case ERROR_ILLEGALCHAR:
		return TEXT("错误：出现非法字符。");
		break;
	case ERROR_PARENTHESIS_NOT_MATCH:
		return TEXT("错误：括号不匹配。");
		break;
	case ERROR_INVALID_VARNAME:
		return TEXT("错误：不正确的变量名（必须以下划线\"_\"或英文字母开头）。");
		break;
	case ERROR_WRONG_EXPRESSION:
		return TEXT("错误：错误的表达式。");
		break;
	case ERROR_EMPTY_INPUT:
		return TEXT("表达式为空。");
		break;
	case ERROR_DIVIDE_ZERO:
		return TEXT("错误：不能除以0。");
		break;
	case ERROR_UNDEFINED_VARIABLE:
		return TEXT("错误：未定义的变量。");
		break;
	case ERROR_ZERO_POWEROF_ZERO:
		return TEXT("错误：0的0次方。");
		break;
	case ERROR_SUBS_NOT_EQUAL:
		return TEXT("错误：替换与被替换数目不等。");
		break;
	case ERROR_NOT_LINK_VARIABLETABLE:
		return TEXT("程序错误：未链接变量表。");
		break;
	case ERROR_OUTOF_DOMAIN:
		return TEXT("错误：超出定义域。");
		break;
	case ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT:
		return TEXT("错误：变量名与初始值数量不对等。");
		break;
	case ERROR_I:
		return TEXT("暂不支持虚数。");
		break;
	default:
		return TEXT("undefined error");
		break;
	}
}