#pragma once
#include "bn_fixed.h"
#include "bn_math.h"
class Vector3
{
	public:
	bn::fixed x;
	bn::fixed y;
	bn::fixed z;
	constexpr Vector3() = default;
	constexpr Vector3(const bn::fixed& x_, const bn::fixed& y_, const bn::fixed& z_) :
		x(x_),y(y_),z(z_)
	{
	}
	constexpr Vector3(const Vector3& v) :
		x(v.x),y(v.y),z(v.z)
	{
	}
	constexpr Vector3& set(const bn::fixed& x_, const bn::fixed& y_, const bn::fixed& z_)
	{
		this->x = x_;
		this->y = y_;
		this->z = z_;
		return *this;
	}
	constexpr Vector3& operator=(const Vector3& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		return *this;
	}
	constexpr Vector3& operator+=(const Vector3& vb)
	{
		this->x += vb.x;
		this->y += vb.y;
		this->z += vb.z;
		return *this;
	}
	constexpr Vector3 operator+(const Vector3& vb) const
	{
		Vector3 res = *this;
		res+=vb;
		return res;
	}
	constexpr Vector3& operator-=(const Vector3& vb)
	{
		this->x -= vb.x;
		this->y -= vb.y;
		this->z -= vb.z;
		return *this;
	}
	constexpr Vector3 operator-(const Vector3& vb) const
	{
		Vector3 res = *this;
		res-=vb;
		return res;
	}
	constexpr Vector3 operator-() const
	{
		Vector3 res;
		res-=*this;
		return res;
	}
	constexpr Vector3& operator*=(const Vector3& vb)
	{
		this->x = this->x.safe_multiplication(vb.x);
		this->y = this->y.safe_multiplication(vb.y);
		this->z = this->z.safe_multiplication(vb.z);
		return *this;
	}
	constexpr Vector3 operator*(const Vector3& vb) const
	{
		Vector3 res = *this;
		res*=vb;
		return res;
	}
	constexpr Vector3& operator*=(const bn::fixed& c)
	{
		this->x = c.safe_multiplication(this->x);
		this->y = c.safe_multiplication(this->y);
		this->z = c.safe_multiplication(this->z);
		return *this;
	}
	constexpr Vector3 operator*(const bn::fixed& c) const
	{
		Vector3 res = *this;
		res*=c;
		return res;
	}
	constexpr bn::fixed dot(const Vector3& vb) const
	{	
		bn::fixed dot;
		dot  = this->x.safe_multiplication(vb.x);
		dot += this->y.safe_multiplication(vb.y);
		dot += this->z.safe_multiplication(vb.z);
		return dot;
	}
	constexpr Vector3& operator^=(const Vector3& vb)
	{
		bn::fixed _x = this->y.safe_multiplication(vb.z) - this->z.safe_multiplication(vb.y);
		bn::fixed _y = this->z.safe_multiplication(vb.x) - this->x.safe_multiplication(vb.z);
		bn::fixed _z = this->x.safe_multiplication(vb.y) - this->y.safe_multiplication(vb.x);
		set(_x,_y,_z);
		return *this;
	}
	constexpr Vector3 operator^(const Vector3& c) const
	{
		Vector3 res = *this;
		res^=c;
		return res;
	}
	constexpr bn::fixed square_norm() const
	{
		return x*x + y*y + z*z;
	}
	constexpr bn::fixed norm() const
	{
		return bn::sqrt(square_norm());
	}
	constexpr Vector3& normalize()
	{
		auto n=norm();
		if(n != 0)
			return *this*=1/n;
		// can't normalize an empty vector, just returning it;
		return *this;
	}
	constexpr Vector3 normalized() const
	{
		Vector3 res(*this);
		return res.normalize();
	}
};

constexpr Vector3 operator*(const bn::fixed& c, const Vector3& v)
{
	Vector3 res=v;
	res*=c;
	return res;
}

inline bn::ostringstream& operator<<(bn::ostringstream& stream, const Vector3& v)
{
	stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return stream;
}
constexpr Vector3 lerp(const Vector3& from, const Vector3& to, const bn::fixed_t<18>& factor)
{
	return from * (1 - factor) + to * factor;
}