#pragma once
#include "Common\MyString.h"

#if (defined _STUDENT) || (defined _TEACHER)
extern std::String sStudentClass;
extern std::String sStudentName;
extern std::String sStudentNumber;
extern std::String sStudentScore;
#endif

extern const TCHAR szAppTitle[];