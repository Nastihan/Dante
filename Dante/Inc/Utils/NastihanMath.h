#pragma once
#include "Pch.h"

namespace Dante::Utils
{
	class NastihanMath
	{
	public:
		static DirectX::XMFLOAT4X4 Identity4x4()
		{
			static DirectX::XMFLOAT4X4 I(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return I;
		}

		static constexpr float PI = 3.14159265f;
		static constexpr double PI_D = 3.1415926535897932;

		template <typename T>
		static constexpr auto sq(const T& x) noexcept
		{
			return x * x;
		}

		template<typename T>
		static T wrap_angle(T theta) noexcept
		{
			constexpr T twoPi = (T)2 * (T)PI_D;
			const T mod = (T)fmod(theta, twoPi);
			if (mod > (T)PI_D)
			{
				return mod - twoPi;
			}
			else if (mod < -(T)PI_D)
			{
				return mod + twoPi;
			}
			return mod;
		}
	};
}