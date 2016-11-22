/*
	*** Element definitions
	*** src/base/elements.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_ELEMENTMAP_H
#define DUQ_ELEMENTMAP_H

#include <QtOpenGL/QtOpenGL>

// Forward Declarations
class Atom;

// Element
class Element
{
	public:
	// Z
	int z;
	// Mass of element
	double atomicMass;
	// Element name
	const char* name;
	// Uppercase element name
	const char* ucName;
	// Element symbol
	const char* symbol;
	// Uppercase Element symbol
	const char* ucSymbol;
	// Group position in periodic table
	int group;
	// Rough elemental radius (for bond calculation etc.)
	double atomicRadius;
	// Element colour
	double colour[4];
	// Numeric measure of 'penalties' for total bond orders 0 - 8
	int bondOrderPenalty[9];
	// Formal charges for bond orders 0 - 8
	int formalCharges[9];


	/*
	// Data by Z
	 */
	public:
	// Copy the ambient colour of the element into the array provided
	void copyColour(GLfloat* v);
	// Set ambient colour component of element
	void setColour(int rgb, double value);
	void setColour(double r, double g, double b, double a);
};

// Element map
class ElementMap
{
	public:
	// Constructor / Destructor
	ElementMap();
	~ElementMap();
	// Name->Z mapping methods
	enum ZMapType { AlphaZMap, FirstAlphaZMap, SingleAlphaZMap, NameZMap, NumericZMap, ForcefieldZMap, AutoZMap, nZMapTypes };
	static ElementMap::ZMapType zMapType(const char* s, bool reportError = 0);
	static const char* zMapType(ElementMap::ZMapType zm);

	private:
	// Convert string from Z to element number
	int numberToZ(const char*) const;
	// Convert string from alpha to element number
	int alphaToZ(const char*) const;
	// Convert string from alpha (up to non-AZ inc 09) to element number
	int firstAlphaToZ(const char*) const;
	// Convert string from first alpha (up to non-AZ inc 09) to element number
	int singleAlphaToZ(const char*) const;
	// Convert string from name to element number
	int nameToZ(const char*) const;
	// Convert string from fftype to element number
	int ffToZ(const char*) const;
	// Number of defined elements
	int nElements_;
	// Storage for copy of element data
	Element* backupEl_;
	
	public:
	// Element data array
	static Element el[];
	// Default element data
	Element* defaultEl;
	// Backup current data
	void backupData();
	// Restore default element values
	void restoreData();
	// Return atomic number of element in string using supplied method (if specified)
	int find(const char* query, ElementMap::ZMapType = ElementMap::nZMapTypes) const;
	// Return number of defined elements
	int nElements() const;


	/*
	// Data by Z
	 */
	public:
	// Return periodic table group number
	int group(int i) const;
	// Return atomic mass of atomic number 'i'
	double atomicMass(int i) const;
	// Return name of atomic number 'i'
	const char* name(int i) const;
	// Return symbol of atomic number 'i'
	const char* symbol(int i) const;
	// Set radius of atomic number 'i'
	void setAtomicRadius(int i, double r);
	// Return effective radius of atomic number 'i'
	double atomicRadius(int i) const;
	// Return bond order penalty for TBO 'bo' of atomic number 'i'
	int bondOrderPenalty(int i, int bo) const;
	// Return the colour of the element
	double* colour(int i);
	// Copy the colour of the element into the array provided
	void copyColour(int i, GLfloat* v) const;
	// Set colour component of element
	void setColour(int i, int rgba, double value);
	void setColour(int i, double r, double g, double b, double a);


	/*
	// Data by atom*
	 */
	public:
	// Return periodic table group number
	int group(Atom* i);
	// Return atomic mass of atomic number 'i'
	double atomicMass(Atom* i);
	// Return name of atomic number 'i'
	const char* name(Atom* i);
	// Return symbol of atomic number 'i'
	const char* symbol(Atom* i);
	// Return effective radius of atomic number 'i'
	double atomicRadius(Atom* i);
	// Return bond order penalty for TBO 'bo' of atomic number 'i'
	int bondOrderPenalty(Atom* i, int bo);
	// Return the colour of the element
	double* colour(Atom* i);
};

extern ElementMap &elements();
extern ElementMap* elementsAsPointer();

#endif
