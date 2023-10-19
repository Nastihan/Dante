#pragma once
#include "Pch.h"

namespace Dante::Utils
{
    class NastihanException 
    {
    public:
        NastihanException() = default;
        NastihanException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
            :
            ErrorCode(hr),
            FunctionName(functionName),
            Filename(filename),
            LineNumber(lineNumber)
        {

        }

        std::wstring ToString() const
        {
            _com_error err(ErrorCode);
            std::wstring msg = err.ErrorMessage();

            return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
        }

        HRESULT ErrorCode = S_OK;
        std::wstring FunctionName;
        std::wstring Filename;
        int LineNumber = -1;
    };
}
#ifndef Chk
#define Chk(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = Dante::Utils::AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw Dante::Utils::NastihanException(hr__, L#x, wfn, __LINE__); } \
}
#endif