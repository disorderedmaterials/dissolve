/*
	*** 4-component vector class
	*** src/templates/vector4.h
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_VECTOR4_H
#define DISSOLVE_VECTOR4_H

#include "templates/vector3.h"
#include <stdio.h>
#include <math.h>

// 4D vector
template <class T> class Vec4
{
	public:
	// Constructor
	Vec4<T>(T xx = 0, T yy = 0, T zz = 0, T ww = 0)
	{
		x = xx;
		y = yy;
		z = zz;
		w = ww;
	}
	// Components of vector
	T x, y, z, w;

	/*
	 * Set / adjust / retrieve
	 */
	public:
	// Set the vector to 0,0,0,0
	void zero()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	// Set all four components simultaneously
	void set(T newX, T newY, T newZ, T newW)
	{
		x = newX;
		y = newY;
		z = newZ;
		w = newW;
	}
	// Set single component
	void set(int el, T value)
	{
		if (el == 0) x = value;
		else if (el == 1) y = value;
		else if (el == 2) z = value;
		else if (el == 3) w = value;
	}
	// Adjust all four components simultaneously
	void add(T a, T b, T c, T d)
	{
		x += a;
		y += b;
		z += c;
		w += d;
	}
	// Set all four components simultaneously
	void set(Vec3<T> v, T newW)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = newW;
	}
	// Retrieve single element
	T get(int index) const
	{
		if (index == 0) return x;
		else if (index == 1) return y;
		else if (index == 2) return z;
		else if (index == 3) return w;
		printf("Vec4 - retrieve index %i is out of range.\n", index);
		return T();
	}

	
	/*
	 * Operators
	 */
	public:
	// Operators + and +=
	Vec4<T>& operator+=(Vec4<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return* this;
	}
	Vec4<T>& operator+=(Vec3<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return* this;
	}
	Vec4<T> operator+(Vec4<T>& v)
	{
		return Vec4<T>(x+v.x, y+v.y, z+v.z, w+v.w);
	}
	Vec4<T> operator+(Vec3<T>& v)
	{
		return Vec4<T>(x+v.x, y+v.y, z+v.z, w);
	}
	// Operators - and -=
	Vec4<T>& operator-=(Vec4<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return* this;
	}
	Vec4<T>& operator-=(Vec3<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return* this;
	}
	Vec4<T> operator-(Vec4<T>& v)
	{
		return Vec4<T>(x-v.x, y-v.y, z-v.z, w-v.w);
	}
	Vec4<T> operator-(Vec3<T>& v)
	{
		return Vec4<T>(x-v.x, y-v.y, z-v.z, w);
	}
	// Operators * and *=
	Vec4<T> operator*(T value) const
	{
		return Vec4<T>(x*value, y*value, z*value, w*value);
	}
	void operator*=(T value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
	}
	// Element access operator
	T operator[](int index)
	{
		if (index == 0) return x;
		else if (index == 1) return y;
		else if (index == 2) return z;
		else if (index == 3) return w;
		printf("Vec4 - array access failed - index %i is out of bounds.\n", index);
		return 0;
	}

	/*
	 * Methods
	 */

	// Normalise to unity
	void normalise()
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
	void print() const
	{
		printf("vec(xyzw) = %8.4f %8.4f %8.4f %8.4f\n", (double)x, (double)y, (double)z, (double)w);
	}
};

#endif
