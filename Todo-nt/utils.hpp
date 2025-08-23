#pragma once
#include <wx/colour.h>
#include <wx/scrolwin.h>
#include <algorithm>
#include <iostream>



namespace utils
{
	struct RGBi
	{
		int r, g, b;

		RGBi() = default;

		constexpr RGBi(int rr, int gg, int bb)
			: r(rr), g(gg), b(bb)
		{}

		explicit RGBi(const wxColour& other)
			: r(other.Red()), g(other.Green()), b(other.Blue())
		{}

		RGBi& operator+=(const RGBi& other)
		{
			r += other.r;
			g += other.g;
			b += other.b;
			return *this;
		}

		RGBi& operator-=(const RGBi& other)
		{
			r -= other.r;
			g -= other.g;
			b -= other.b;
			return *this;
		}

		RGBi operator+(const RGBi& other) const
		{
			return { r + other.r, g + other.g, b + other.b };
		}

		RGBi operator-(const RGBi& other) const
		{
			return { r - other.r, g - other.g, b - other.b };
		}

		RGBi operator*(const float scalar) const
		{
			float rr = r, gg = g, bb = b;
			rr *= scalar;
			gg *= scalar;
			bb *= scalar;
			return { int(rr), int(gg), int(bb) };
		}

		RGBi& operator=(const wxColour& other) 
		{
			r = other.Red();
			g = other.Green();
			b = other.Blue();
			return *this;
		}


		static inline int clamp255(int v) { return std::min(255, std::max(0, v)); }
		static inline RGBi clampRGBi(const RGBi& rgb)
		{
			return { clamp255(rgb.r), clamp255(rgb.g), clamp255(rgb.b) };
		}

		operator wxColour() const
		{
			RGBi a = clampRGBi(*this);
			return wxColour(a.r, a.g, a.b);
		}
		

	};

	template<typename T>
	inline T lerp(const T& a, const T& b, float alpha)
	{
		return a + (b - a) * alpha;
	}

	inline wxColour wxColourLerp(const wxColour& a, const wxColour& b, float alpha)
	{
		return lerp<RGBi>(RGBi(a), RGBi(b), alpha);
	}


}