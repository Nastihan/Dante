#pragma once

#include <iostream>
#include <assert.h>
#include <exception>
#include <format>
#include <wtypes.h>
#include <string>
#include <comdef.h>
#include <optional>

// WIN32
#define NOMINMAX
#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

// DX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "../ThirdParty/d3dx12.h"


#include <Utils/Exception.h>
#include <Utils/NastihanUtil.h>


// Macros
#define DISABLE_COPY_AND_MOVE(Class) \
Class(const Class&) = delete; \
Class(Class&&) = delete; \
Class& operator=(const Class&) = delete; \
Class& operator=(Class&&) = delete;

#define ID(pType) __uuidof(**(&pType)), IID_PPV_ARGS_Helper(&pType)

// 
using Microsoft::WRL::ComPtr;

//
