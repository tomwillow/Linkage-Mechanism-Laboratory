#include <string>
#include <fstream>

#ifdef _UNICODE

using std::wstring;
#define String wstring

using std::wofstream;
#define Ofstream wofstream

using std::wifstream;
#define Ifstream wifstream

#else

using std::string;
#define String string

using std::ofstream;
#define Ofstream ofstream

using std::ifstream;
#define Ifstream ifstream

#endif

//String & operator<<(String &s,TCHAR szStr[])
//{
//	s += szStr;
//	return s;
//}