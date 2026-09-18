#pragma once

#include <cmath>
#include <cassert>
#include <PxPhysics.h>

class Vector3D
{
public:
	Vector3D() noexcept : _x(0), _y(0), _z(0) {}
	Vector3D(float x, float y, float z) noexcept : _x(x), _y(y), _z(z) {}
	Vector3D(float v) noexcept : _x(v), _y(v), _z(v) {}
	Vector3D(const physx::PxVec3& vec) noexcept : _x(vec.x), _y(vec.y), _z(vec.z){}
	static Vector3D zero() noexcept { return Vector3D(0); }
	static Vector3D one() noexcept { return Vector3D(1); }

	void setX(float x) noexcept { _x = x;}
	void setY(float y) noexcept { _y = y;}
	void setZ(float z) noexcept { _z = z;}
	float getX() const noexcept { return _x;}
	float getY() const noexcept { return _y;}
	float getZ() const noexcept { return _z;}

	float magnitude()		const noexcept{ return std::sqrt(magnitudeSqr()); }
	float magnitudeSqr()	const noexcept { return _x * _x + _y * _y + _z * _z; }
	void normalize() {

		float mod = magnitude();
		assert(mod != 0, "Can't normalize vector with module = 0");

		_x /= mod; _y /= mod; _z /= mod;
	}
	Vector3D normalized() {

		float mod = magnitude();
		assert(mod != 0, "Can't normalize vector with module = 0");

		return Vector3D(_x / mod, _y / mod, _z / mod);
	}

	float dot(const Vector3D& other) const noexcept
	{
		return _x * other._x + _y * other._y + _z * other._z;
	}

	Vector3D cross(const Vector3D& other) const {
		return Vector3D(
			_y*other._z - _z*other._y,
			_z*other._x - _x*other._z,
			_x*other._y - _y*other._x
		);
	}

	Vector3D& operator= (const Vector3D& other) noexcept
	{
		if (&other == this) return *this;

		_x = other._x;
		_y = other._y;
		_z = other._z;

		return *this;
	}
	Vector3D& operator= (const physx::PxVec3& other) noexcept
	{
		_x = other.x;
		_y = other.y;
		_z = other.z;

		return *this;
	}
	Vector3D& operator+ (const Vector3D& other) const noexcept
	{
		return Vector3D(_x + other._x, _y + other._y, _z + other._z);
	}
	Vector3D& operator- (const Vector3D& other) const noexcept
	{
		return Vector3D(_x - other._x, _y - other._y, _z - other._z);
	}
	float operator* (const Vector3D& other) const noexcept
	{
		return dot(other);
	}
	Vector3D& operator* (float scalar) const noexcept
	{
		return Vector3D(_x * scalar, _y * scalar, _z * scalar);
	}

	operator physx::PxVec3() const{
		return physx::PxVec3(_x, _y, _z);
	}

private:
	float _x, _y, _z;
};