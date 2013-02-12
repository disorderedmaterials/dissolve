/*
	*** Box Definition
	*** src/lib/classes/box.h
	Copyright T. Youngs 2012-2013

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

#ifndef DUQ_BOX_H
#define DUQ_BOX_H

#include "math/matrix3.h"

// Forward Declarations
class Atom;
class Cell;

/*!
 * \brief Basic Box Definition
 * \details XXX TODO
 */
class Box
{
	public:
	// Constructor
	Box();
	// Virtual Destructor
	virtual ~Box();
	/// Box Type Enum
	enum BoxType {
		CubicBox,		/**> Cubic box with equivalent cell lengths, and right-angles */
		OrthorhombicBox,	/**> Orthorhombic box with inequivalent cell lengths, and right-angles */
		MonoclinicBox,		/**> Monoclinic box with cell angles a != 90, and b == c == 90 */
		TriclinicBox,		/**> Triclinic box with cell angles a != b != c != 90 */
		nBoxTypes		/**> Number of Box types */
	};
	// Convert text string to BoxType
	static BoxType boxType(const char* s);
	// Convert BoxType to text string
	static const char* boxType(BoxType id);


	/*!
	 * \name Axis Definition
	 */
	///@{
	protected:
	// Box type
	BoxType type_;
	// Axes
	Matrix3 axes_;
	// Inverse Axes
	Matrix3 inverseAxes_;

	public:
	// Return Box type
	BoxType type();
	// Set box volume (in cubic Angstroms)
	void setVolume(double volume);
	// Return axis length specified
	double axisLength(int n);
	// Return axis angle specified
	double axisAngle(int n);
	// Return axis matrix
	Matrix3 axes();
	///@}


	/*!
	 * \name Cell Partitioning
	 */
	///@{
	protected:
	// Cell divisions along each axis
	Vec3<int> divisions_;
	// Fractional Cell size
	Vec3<double> cellSize_;
	// Real Cell size
	Vec3<double> realCellSize_;
	// Cell array
	Cell*** cells_;
	// Cell index array
	Cell** cellIndex_;
	
	private:
	// Clear Cell arrays
	void clearCells();

	public:
	// Generate Cells for Box
	bool generateCells(double ppRange);
	// Retrieve Cell with (wrapped) grid reference specified
	Cell* cell(int x, int y, int z);
	// Retrieve Cell with id specified
	Cell* cell(int id);
	///@}


	/*!
	 * \name Minimum Image Routines (Pure Virtual)
	*/
	///@{
	public:
	// Return minimum image coordinates of 'i' with respect to 'ref'
	virtual Vec3<double> minimumImage(const Atom* i, const Atom* ref) const = 0;
	// Return minimum image coordinates of 'i' with respect to 'ref'
	virtual Vec3<double> minimumImage(const Atom* i, const Vec3<double> ref) const = 0;
	// Return minimum image vector from 'i' to 'j'
	virtual Vec3<double> minimumVector(const Atom* i, const Atom* j) const = 0;
	// Return minimum image vector from 'i' to 'j'
	virtual Vec3<double> minimumVector(const Atom* i, const Vec3<double> j) const = 0;
	///@}


	/*!
	 * \name Utility Routines (Pure Virtual)
	 */
	///@{
	public:
	// Return random coordinate inside Box
	virtual Vec3<double> randomCoordinate() const = 0;
	// Return Cell containing specified coordinate
	virtual Cell* cell(const Vec3<double> r) const = 0;
	///@}
};

/*!
 * \brief Cubic Box Definition
 */
class CubicBox : public Box
{
	public:
	// Constructor
	CubicBox(double volume, double A);
	// Destructor
	~CubicBox();


	/*!
	 * \name Definition
	 */
	///@{
	private:
	// Box length
	double a_;
	///@}


	/*!
	 * \name Minimum Image Routines (Virtual Implementations)
	*/
	///@{
	public:
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Atom* ref) const;
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Vec3<double> ref) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Atom* j) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Vec3<double> j) const;
	///@}


	/*!
	 * \name Utility Routines (Virtual Implementations)
	 */
	///@{
	public:
	// Return random coordinate inside Box
	Vec3<double> randomCoordinate() const;
	// Return Cell containing specified coordinate
	Cell* cell(const Vec3<double> r) const;
	///@}
};

/*!
 * \brief Orthorhombic Box Definition
 */
class OrthorhombicBox : public Box
{
	public:
	// Constructor
	OrthorhombicBox(double volume, const Vec3<double> relativeLengths);
	// Destructor
	~OrthorhombicBox();


	/*!
	 * \name Definition
	 */
	///@{
	private:
	// Box lengths
	double a_, b_, c_;
	///@}


	/*!
	 * \name Minimum Image Routines (Virtual Implementations)
	*/
	///@{
	public:
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Atom* ref) const;
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Vec3<double> ref) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Atom* j) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Vec3<double> j) const;
	///@}


	/*!
	 * \name Utility Routines (Virtual Implementations)
	 */
	///@{
	public:
	// Return random coordinate inside Box
	Vec3<double> randomCoordinate() const;
	// Return Cell containing specified coordinate
	Cell* cell(const Vec3<double> r) const;

	///@}
};

/*!
 * \brief Monoclinic Box Definition
 */
class MonoclinicBox : public Box
{
	public:
	// Constructor
	MonoclinicBox(double volume, const Vec3<double> relativeLengths, double alpha);
	// Destructor
	~MonoclinicBox();


	/*!
	 * \name Definition
	 */
	///@{
	private:
	// Box lengths
	double a_, b_, c_;
	// Box angle
	double alpha_;
	///@}


	/*!
	 * \name Minimum Image Routines (Virtual Implementations)
	*/
	///@{
	public:
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Atom* ref) const;
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Vec3<double> ref) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Atom* j) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Vec3<double> j) const;
	///@}


	/*!
	 * \name Utility Routines (Virtual Implementations)
	 */
	///@{
	public:
	// Return random coordinate inside Box
	Vec3<double> randomCoordinate() const;
	// Return Cell containing specified coordinate
	Cell* cell(const Vec3<double> r) const;
	///@}
};

/*!
 * \brief Triclinic Box Definition
 */
class TriclinicBox : public Box
{
	public:
	// Constructor
	TriclinicBox(double volume, const Vec3<double> relativeLengths, const Vec3<double> cellAngles);
	// Destructor
	~TriclinicBox();


	/*!
	 * \name Definition
	 */
	///@{
	private:
	// Box lengths
	double a_, b_, c_;
	// Box angles
	double alpha_, beta_, gamma_;
	///@}


	/*!
	 * \name Minimum Image Routines (Virtual Implementations)
	*/
	///@{
	public:
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Atom* ref) const;
	// Return minimum image coordinates of 'i' with respect to 'ref'
	Vec3<double> minimumImage(const Atom* i, const Vec3<double> ref) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Atom* j) const;
	// Return minimum image vector from 'i' to 'j'
	Vec3<double> minimumVector(const Atom* i, const Vec3<double> j) const;
	///@}


	/*!
	 * \name Utility Routines (Virtual Implementations)
	 */
	///@{
	public:
	// Return random coordinate inside Box
	Vec3<double> randomCoordinate() const;
	// Return Cell containing specified coordinate
	Cell* cell(const Vec3<double> r) const;
	///@}
};

#endif
