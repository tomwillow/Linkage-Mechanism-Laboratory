#ifdef _UNICODE
#define String wstring
using std::wstring;
#else
#define String string
using std::string;
#endif