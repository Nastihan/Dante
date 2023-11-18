#pragma once
#include <string>
#include <filesystem>


namespace Dante::Utils
{
	inline std::wstring ToWide(const std::string& narrow)
	{
		wchar_t wide[512];
		mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
		return wide;
	}

	inline std::string ToNarrow(const std::wstring& wide)
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

	inline std::string FolderFromFilePath(std::string& filePath)
	{
		std::filesystem::path fullPath(filePath);

		if (!std::filesystem::exists(fullPath))
		{
			throw std::runtime_error("File not found: " + filePath);
		}

		std::filesystem::path folderPath = fullPath.parent_path();

		return folderPath.string();
	}
}