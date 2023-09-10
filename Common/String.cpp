#include "String.h"

std::wstring stringToWstring(const std::string &str) {
    LPCSTR pszSrc = str.c_str();
    int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
    if (nLen == 0)
        return std::wstring(L"");

    wchar_t *pwszDst = new wchar_t[nLen];
    if (!pwszDst)
        return std::wstring(L"");

    MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
    std::wstring wstr(pwszDst);
    delete[] pwszDst;
    pwszDst = NULL;

    return wstr;
}

std::string wstringToString(const std::wstring &wstr) {
    LPCWSTR pwszSrc = wstr.c_str();
    int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
    if (nLen == 0)
        return std::string("");

    char *pszDst = new char[nLen];
    if (!pszDst)
        return std::string("");

    WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
    std::string str(pszDst);
    delete[] pszDst;
    pszDst = NULL;

    return str;
}

std::string toUTF8(const std::wstring &ws) {
    int nLen = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
    if (nLen == 0)
        return std::string("");

    std::string ret(nLen, 0);

    int nLen2 = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, const_cast<char*>(ret.data()), nLen, NULL, NULL);
    if (nLen2 != nLen)
    {
        throw std::runtime_error("WideCharToMultiByte fail");
    }

    return ret;
}

std::wstring utf8toWString(const std::string &s) {
    int nLen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    if (nLen == 0)
        return std::wstring(L"");

    std::wstring ret(nLen, 0);

    int nLen2 = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, const_cast<wchar_t*>(ret.data()), nLen);
    if (nLen2 != nLen) {
        throw std::runtime_error("MultiByteToWideChar fail");
    }

    return ret;
}

std::vector<std::string> StrSliceToVector(std::string s) {
    std::vector<std::string> ret;
    std::stringstream ss;
    ss << s;
    while (!ss.eof()) {
        std::string word;
        ss >> word;
        ret.emplace_back(word);
    }
    return ret;
}

std::vector<double> StrSliceToDoubleVector(std::string s) {
    std::vector<double> ret;
    std::stringstream ss;
    ss << s;
    while (!ss.eof()) {
        std::string word;
        ss >> word;
        if (word.empty())
            break;
        ret.emplace_back(std::stod(word));
    }
    return ret;
}