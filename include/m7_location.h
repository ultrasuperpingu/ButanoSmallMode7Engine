#pragma once
#include "vector3.h"


class M7Location
{
	public:
	constexpr M7Location()
	{
		_update();
	}
	constexpr M7Location(const Vector3& p, int16_t phi, int16_t theta) : pos(p), _phi(phi), _theta(theta)
	{
		_update();
	}
	constexpr M7Location(const M7Location& l) : pos(l.pos), _phi(l._phi), _theta(l._theta), _right(l._right), _up(l._up), _backward(l._backward)
	{
	}
	constexpr M7Location& operator=(const M7Location& l)
	{
		pos=l.pos;
		_phi=l._phi;
		_theta=l._theta;
		_right=l._right;
		_up=l._up;
		_backward=l._backward;
		return *this;
	}
	constexpr void set_location(const M7Location& l)
	{
		pos=l.pos;
		_phi=l._phi;
		_theta=l._theta;
		_right=l._right;
		_up=l._up;
		_backward=l._backward;
	}
	constexpr int16_t phi() const
	{
		return _phi;
	}
	constexpr int16_t theta() const
	{
		return _theta;
	}
	constexpr void set_orientation(int16_t phi, int16_t theta)
	{
		_phi = phi;
		_theta = theta;
		_update();
	}
	constexpr void translate(const Vector3& delta)
	{
		pos += _right * delta.x + _up * delta.y - _backward * delta.z;
	}
	constexpr void translate_floor_plane(bn::fixed dir_x, bn::fixed dir_z)
	{
		pos.x += (dir_x * cf()) - (dir_z * sf());
		pos.z += (dir_x * sf()) + (dir_z * cf());
	}
	constexpr void set_phi(int16_t phi)
	{
		_phi = phi;
		bn::fixed _cf = bn::lut_cos(_phi);
		bn::fixed _sf = bn::lut_sin(_phi);
		bn::fixed _ct = ct();
		bn::fixed _st = st();

		_right.x = _cf;
		_right.z = _sf;
		_up.x = _sf.safe_multiplication(_st);
		_up.z = -_cf.safe_multiplication(_st);
		_backward.x = -_sf.safe_multiplication(_ct);
		_backward.z = _cf.safe_multiplication(_ct);
	}
	constexpr void set_theta(int16_t theta)
	{
		_theta = theta;
		bn::fixed _cf = cf();
		bn::fixed _sf = sf();
		bn::fixed _ct = bn::lut_cos(_theta);
		bn::fixed _st = bn::lut_sin(_theta);

		_up.x = _sf.safe_multiplication(_st);
		_up.y = _ct;
		_up.z = -_cf.safe_multiplication(_st);

		_backward.x = -_sf.safe_multiplication(_ct);
		_backward.y = _st;
		_backward.z = _cf.safe_multiplication(_ct);
	}
	constexpr const bn::fixed& sf() const
	{
		return _right.z;
	}
	constexpr const bn::fixed& cf() const
	{
		return _right.x;
	}
	constexpr const bn::fixed& st() const
	{
		return _backward.y;
	}
	constexpr const bn::fixed& ct() const
	{
		return _up.y;
	}
	
	constexpr const Vector3& right() const
	{
		return _right;
	}
	constexpr Vector3 left() const
	{
		return -_right;
	}
	constexpr const Vector3& up() const
	{
		return _up;
	}
	constexpr Vector3 down() const
	{
		return -_up;
	}
	constexpr Vector3 forward() const
	{
		return -_backward;
	}
	constexpr const Vector3& backward() const
	{
		return _backward;
	}
	constexpr Vector3 global_to_local(const Vector3& global) const
	{
		Vector3 local;
		Vector3 vr=global-pos;
		local.x=vr.dot(right());
		local.y=-vr.dot(up());
		local.z=-vr.dot(backward());
		return local;
	}
	inline void look_at(const Vector3& position)
	{
		Vector3 diff = position - pos;
		int phi = (bn::atan2(diff.z.round_integer(), diff.x.round_integer()) * 2048).round_integer()+512;
		if(phi < 0)
			phi+=2048;
		if(phi >= 2048)
			phi-=2048;
		
		bn::fixed diffY=position.y - pos.y;
		diff.y=0;
		int theta = (bn::atan2(-diffY.round_integer(), diff.norm().round_integer()) * 2048).round_integer();
		if(theta < 0)
			theta+=2048;
		set_orientation(phi, theta);
	}
	
	Vector3 pos;
	protected:
	constexpr void _update()
	{
		bn::fixed cf = bn::lut_cos(_phi);
		bn::fixed sf = bn::lut_sin(_phi);
		bn::fixed ct = bn::lut_cos(_theta);
		bn::fixed st = bn::lut_sin(_theta);

		_right.x = cf;
		_right.y = 0;
		_right.z = sf;

		_up.x = sf.safe_multiplication(st);
		_up.y = ct;
		_up.z = -cf.safe_multiplication(st);

		_backward.x = -sf.safe_multiplication(ct);
		_backward.y = st;
		_backward.z = cf.safe_multiplication(ct);
	}
	int16_t _phi = 0;//!< Azimuth angle.
	int16_t _theta = 0;//!< Polar angle.
	Vector3 _right;		//!< local x-axis (right)
	Vector3 _up;		//!< local y-axis (up)
	Vector3 _backward;		//!< local z-axis (back)
	friend constexpr M7Location lerp(const M7Location& from, const M7Location& to, const bn::fixed_t<18>& factor);
	friend constexpr M7Location lerp_no_update(const M7Location& from, const M7Location& to, const bn::fixed_t<18>& factor);
};

constexpr M7Location lerp(const M7Location& from, const M7Location& to, const bn::fixed_t<18>& factor)
{
	M7Location res=lerp_no_update(from, to, factor);
	res._update();
	return res;
}
constexpr M7Location lerp_no_update(const M7Location& from, const M7Location& to, const bn::fixed_t<18>& factor)
{
	M7Location res;
	res.pos = lerp(from.pos, to.pos, factor);
	if(from._phi > 1024 && to._phi < 1024 && from._phi-to._phi > 1024)
	{
		int from_phi=from._phi-2048;
		res._phi = (from_phi * (1 - factor) + to._phi * factor).round_integer();
		if(res._phi < 0)
			res._phi += 2048;
	}
	else if(from._phi < 1024 && to._phi > 1024 && to._phi-from._phi > 1024)
	{
		int to_phi=to._phi-2048;
		res._phi = (from._phi * (1 - factor) + to_phi * factor).round_integer();
		if(res._phi < 0)
			res._phi += 2048;
	}
	else
	{
		res._phi = (from._phi * (1 - factor) + to._phi * factor).round_integer();
	}
	res._theta = (from._theta * (1 - factor) + to._theta * factor).round_integer();
	return res;
}

inline bn::ostringstream& operator<<(bn::ostringstream& stream, const M7Location& l)
{
	stream << "(" << l.pos << ", " << l.phi() << ", " << l.theta() << ")";
	return stream;
}

constexpr int normalize_angle(int angle)
{
	while(angle < 0)
	{
		angle+=2048;
	}
	while(angle > 2047)
	{
		angle-=2048;
	}
	return angle;
}