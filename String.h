#ifdef _UNICODE
#include <string>

#define String wstring
using std::wstring;
#else
#define String string
using std::string;
#endif