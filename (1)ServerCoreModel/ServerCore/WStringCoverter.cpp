#include "pch.h"
#include "WStringCoverter.h"

std::wstring WStringCoverter::StringToWString(const std::string& st)
{
	if (st.empty())
	{
		return std::wstring();
	}

	int wideLen = MultiByteToWideChar(CP_UTF8, 0, st.data(), (int)st.size(), nullptr, 0);
	if (wideLen == 0) return std::wstring();

	std::wstring wideStr(wideLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, st.data(), (int)st.size(), &wideStr[0], wideLen);

	return wideStr;
}

std::string WStringCoverter::WStringToString(const std::wstring& wst)
{
	if (wst.empty())
	{
		return std::string();
	}

	int stLen = WideCharToMultiByte(CP_UTF8, 0, wst.data(), (int)wst.size(), nullptr, 0, nullptr, 
		nullptr);
	if (stLen == 0)
	{
		return std::string();
	}

	string st(stLen, 0);
	WideCharToMultiByte(CP_UTF8, 0, wst.data(), (int)wst.size(), &st[0], stLen, nullptr, nullptr);

	return st;
}
