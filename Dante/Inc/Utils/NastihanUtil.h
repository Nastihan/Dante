#pragma once
#include <string>


namespace Dante::Utils
{
	std::wstring ToWide(const std::string& narrow)
	{
		wchar_t wide[512];
		mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
		return wide;
	}

	std::string ToNarrow(const std::wstring& wide)
	{
		char narrow[512];
		wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);
		return narrow;
	}

	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}
}