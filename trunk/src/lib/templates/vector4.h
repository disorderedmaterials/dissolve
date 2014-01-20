/*
	*** 4-component vector class
	*** src/lib/templates/vector4.h
	Copyright T. Youngs 2012-2014

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_VECTOR4_H
#define DUQ_VECTOR4_H

#include "templates/vector3.h"
#include <stdio.h>
#include <math.h>

// 4D vector
template <class T> class Vec4
{
	public:
	// Constructor
	Vec4<T>(T xx = 0, T yy = 0, T zz = 0, T ww = 0);
	// Components of vector
	T x,y,z,w;

	/*
	// Set / adjust / retrieve
	*/
	public:
	// Set the vector to 0,0,0
	void zero();
	// Set all four components simultaneously
	void set(T, T, T, T);
	// Set single component
	void set(int el, T value);
	// Adjust all four components simultaneously
	void add(T, T, T, T);
	// Set all four components simultaneously
	void set(Vec3<T>, T);
	// Retrieve single element
	T get(int index) const;

	/*
	// Operators
	*/
	public:
	// Operators + and +=
	Vec4<T>& operator+=(Vec4<T>);
	Vec4<T>& operator+=(Vec3<T>);
	Vec4<T> operator+(Vec4<T>);
	Vec4<T> operator+(Vec3<T>);
	// Operators - and -=
	Vec4<T>& operator-=(Vec4<T>);
	Vec4<T>& operator-=(Vec3<T>);
	Vec4<T> operator-(Vec4<T>);
	Vec4<T> operator-(Vec3<T>);
	// Operators * and *=
	Vec4<T> operator*(T) const;
	void operator*=(T);
	// Element access operator
	T operator[](int);

	/*
	// Methods
	*/
	public:
	// Normalise to unity
	void normalise();
	// Print data
	void print() const;
};

// Constructur
template <class T> Vec4<T>::Vec4(T xx, T yy, T zz, T ww)
{
	x = xx;
	y = yy;
	z = zz;
	w = ww;
}

// Zero
template <class T> void Vec4<T>::zero()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

// Set
template <class T> void Vec4<T>::set(T a, T b, T c, T d)
{
	x = a;
	y = b;
	z = c;
	w = d;
}

// Set single component
template <class T> void Vec4<T>::set(int el, T value)
{
	if (el == 0) x = value;
	else if (el == 1) y = value;
	else if (el == 2) z = value;
	else if (el == 3) w = value;
}

// Add
template <class T> void Vec4<T>::add(T a, T b, T c, T d)
{
	x += a;
	y += b;
	z += c;
	w += d;
}

// Set (vec3, T)
template <class T> void Vec4<T>::set(Vec3<T> v, T a)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = a;
}

// Retrieve single element
template <class T> T Vec4<T>::get(int index) const
{
	if (index == 0) return x;
	else if (index == 1) return y;
	else if (index == 2) return z;
	else if (index == 3) return w;
	printf("Vec4 - retrieve index %i is out of range.\n", index);
	return T();
}

/*
// Operators
*/

// Operator += (Vec4)
template <class T> Vec4<T>& Vec4<T>::operator+=(Vec4<T> v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return* this;
}

// Operator += (vec3)
template <class T> Vec4<T>& Vec4<T>::operator+=(Vec3<T> v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return* this;
}

// Operator + (Vec4)
template <class T> Vec4<T> Vec4<T>::operator+(Vec4<T> v)
{
	Vec4<T> result;
	result.x = x+v.x;
	result.y = y+v.y;
	result.z = z+v.z;
	result.w = w+v.w;
	return result;
}

// Operator + (vec3)
template <class T> Vec4<T> Vec4<T>::operator+(Vec3<T> v)
{
	Vec4<T> result;
	result.x = x+v.x;
	result.y = y+v.y;
	result.z = z+v.z;
	result.w = w+v.w;
	return result;
}

// Operator -= (Vec4)
template <class T> Vec4<T>& Vec4<T>::operator-=(Vec4<T> v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return* this;
}

// Operator -= (vec3)
template <class T> Vec4<T>& Vec4<T>::operator-=(Vec3<T> v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return* this;
}

// Operator - (Vec4)
template <class T> Vec4<T> Vec4<T>::operator-(Vec4<T> v)
{
	Vec4<T> result;
	result.x = x-v.x;
	result.y = y-v.y;
	result.z = z-v.z;
	result.w = w-v.w;
	return result;
}

// Operator - (vec3)
template <class T> Vec4<T> Vec4<T>::operator-(Vec3<T> v)
{
	Vec4<T> result;
	result.x = x-v.x;
	result.y = y-v.y;
	result.z = z-v.z;
	result.w = w-v.w;
	return result;
}

// Operator *= (T)
template <class T> void Vec4<T>::operator*=(T v)
{
	x *= v;
	y *= v;
	z *= v;
	w *= v;
}

// Element access operator
template <class T> T Vec4<T>::operator[](int index)
{
	if (index == 0) return x;
	else if (index == 1) return y;
	else if (index == 2) return z;
	else if (index == 3) return w;
	printf("Vec4 - array access failed - index %i is out of bounds.\n", index);
	return 0;
}

/*
// Methods
*/

// Normalise
template <class T> void Vec4<T>::normalise()
{
	double mag = sqrt(w*w + x*x + y*y + z*z);
	if (mag < 1.0E-8) zero();
	else
	{
		x /= mag;
		y /= mag;
		z /= mag;
		w /= mag;
	}
}

// Print
template <class T> void Vec4<T>::print() const
{
	printf("vec(xyzw) = %8.4f %8.4f %8.4f %8.4f\n", (double)x, (double)y, (double)z, (double)w);
}

#endif
