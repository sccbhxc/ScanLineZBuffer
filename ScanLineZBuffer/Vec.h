#ifndef __VEC_H
#define __VEC_H

#include <cmath>
#include "utils.h"


class Vec3f;
typedef Vec3f Color3f;
typedef Vec3f Point3f;

class Vec3f
{
public:
	union {
		struct{ float x, y, z; };
		struct{ float r, g, b; };
	};

	Vec3f() :x(0.0f), y(0.0f), z(0.0f){};
	Vec3f(float x, float y, float z) 
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline Vec3f& operator = (const Vec3f &v) { x = v.x; y = v.y; z = v.z; return *this; }

	inline Vec3f& operator += (float num) { x += num; y += num; z += num; return *this; }
	inline Vec3f& operator += (const Vec3f &v) { x += v.x; y += v.y; z += v.z; return *this; }

	inline Vec3f& operator -= (float num) { x -= num; y -= num; z -= num; return *this; }
	inline Vec3f& operator -= (const Vec3f &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

	inline Vec3f& operator *= (float num) { x *= num; y *= num; z *= num; return *this; }
	inline Vec3f& operator *= (const Vec3f &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }

	inline Vec3f& operator /= (float num) { x /= num; y /= num; z /= num; return *this; }
	inline Vec3f& operator /= (const Vec3f &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }


	friend inline Vec3f operator + (const Vec3f &u, double num) { return Vec3f(u.x + num, u.y + num, u.z + num); }
	friend inline Vec3f operator + (double num, const Vec3f &u) { return Vec3f(num + u.x, num + u.y, num + u.z); }
	friend inline Vec3f operator + (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x + v.x, u.y + v.y, u.z + v.z); }
	friend inline Vec3f operator - (const Vec3f &u, double num) { return Vec3f(u.x - num, u.y - num, u.z - num); }
	friend inline Vec3f operator - (double num, const Vec3f &u) { return Vec3f(num - u.x, num - u.y, num - u.z); }
	friend inline Vec3f operator - (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x - v.x, u.y - v.y, u.z - v.z); }
	friend inline Vec3f operator * (const Vec3f &u, double num) { return Vec3f(u.x * num, u.y * num, u.z * num); }
	friend inline Vec3f operator * (double num, const Vec3f &u) { return Vec3f(num * u.x, num * u.y, num * u.z); }
	friend inline Vec3f operator * (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x * v.x, u.y * v.y, u.z * v.z); }
	friend inline Vec3f operator / (const Vec3f &u, double num) { return Vec3f(u.x / num, u.y / num, u.z / num); }
	friend inline Vec3f operator / (double num, const Vec3f &u) { return Vec3f(num / u.x, num / u.y, num / u.z); }
	friend inline Vec3f operator / (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x / v.x, u.y / v.y, u.z / v.z); }

	inline bool operator == (const Vec3f &u) { return isEqualf(x, u.x) && isEqualf(y, u.y) && isEqualf(z, u.z); }
	inline bool operator != (const Vec3f &u) { return !(isEqualf(x, u.x) && isEqualf(y, u.y) && isEqualf(z, u.z)); }

};


inline Vec3f cross(const Vec3f &u, const Vec3f &v)
{
	return Vec3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline float dot(const Vec3f &u, const Vec3f &v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline float length(const Vec3f &u)
{
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

inline Vec3f normalize(const Vec3f &u)
{
	return u * (1.0f / sqrt(u.x * u.x + u.y * u.y + u.z * u.z));
}












#endif