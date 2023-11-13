#pragma once

#include <iostream>
#include <assert.h>
#include <exception>
#include <format>
#include <wtypes.h>
#include <string>
#include <comdef.h>
#include <optional>
#include <array>
#include <unordered_map>
#include <utility>
#include <memory>
#include <algorithm>

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
#include <DirectXColors.h>
#include <d3dcompiler.h>

#include "../ThirdParty/DirectX/d3dx12.h"
//#include "../ThirdParty/DirectX/DDSTextureLoader.h"


#include <Utils/Exception.h>
#include <Utils/NastihanUtil.h>


// Macros
#define DISABLE_COPY_AND_MOVE(Class) \
Class(const Class&) = delete; \
Class(Class&&) = delete; \
Class& operator=(const Class&) = delete; \
Class& operator=(Class&&) = delete;


// DX Macros
#define NAME_D3D12_OBJECT(obj, name) obj->SetName(name);
#define ID(pType) __uuidof(**(&pType)), IID_PPV_ARGS_Helper(&pType)

// 
using Microsoft::WRL::ComPtr;

//
//#define STB_IMAGE_IMPLEMENTATION
//#include "../ThirdParty/tinygltf/stb_image.h"
//
//#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_USE_CPP20
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STBI_MSC_SECURE_CRT
//#include "../ThirdParty/tinygltf/tiny_gltf.h"

//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif // !STB_IMAGE_IMPLEMENTATION
//
//#include "../ThirdParty/tinygltf/stb_image.h"






