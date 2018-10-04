/*
	*** Site
	*** src/classes/site.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_SITE_H
#define DISSOLVE_SITE_H

#include "math/matrix3.h"

// Forward Declarations
class Molecule;

// Site Definition
class Site
{
	public:
	// Constructor
	Site(const Molecule* molecule = NULL, Vec3<double> origin = Vec3<double>());
	// Destructor
	virtual ~Site();


	/*
	 * Site Definition
	 */
	protected:
	// Site origin
	Vec3<double> origin_;
	// Molecule to which site is related (if relevant)
	const Molecule* molecule_;

	public:
	// Return site origin
	const Vec3<double>& origin() const;
	// Return Molecule to which site is related (if relevant)
	const Molecule* molecule() const;
	// Return whether local axes are present
	virtual bool hasAxes() const;
	// Return local axes
	virtual const Matrix3& axes() const;
};

// Site with Axes Definition
class OrientedSite : public Site
{
	public:
	// Constructor
	OrientedSite(const Molecule* molecule = NULL, Vec3<double> origin = Vec3<double>(), Vec3<double> xAxis = Vec3<double>(), Vec3<double> yAxis = Vec3<double>(), Vec3<double> zAxis = Vec3<double>());
	// Destructor
	~OrientedSite();


	/*
	 * Site Definition
	 */
	private:
	// Local axes
	Matrix3 axes_;

	public:
	// Return whether local axes are present
	bool hasAxes() const;
	// Return local axes
	const Matrix3& axes() const;
};

#endif

