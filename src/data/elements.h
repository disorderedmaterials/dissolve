/*
	*** Element Data
	*** src/data/elements.h
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

#ifndef DISSOLVE_ELEMENTS_H
#define DISSOLVE_ELEMENTS_H

// Basic Element Definition (Z, name, symbol)
class Element
{
	public:
	// Constructor
	Element(int Z, const char* name, const char* symbol);

	private:
	// Atomic number (Z)
	int Z_;
	// Element name
	const char* name_;
	// Element symbol
	const char* symbol_;

	public:
	// Return atomic number (Z)
	int Z() const;
	// Return name of element
	const char* name() const;
	// Return symbol of element
	const char* symbol() const;
};

// Basic Elements Base Class
class Elements
{
	/*
	 * Element Data
	 */
	private:
	// Instantiate / return array of element data
	static Element* elements();

	public:
	// Return Element with corresponding Z 
	static Element& element(int Z);
	// Return Element with corresponding symbol
	static Element& element(const char* symbol);
	// Return pointer to Element with corresponding Z
	static Element* elementPointer(int Z);
	// Return pointer to Element with corresponding symbol
	static Element* elementPointer(const char* symbol);
	// Return total number of defined elements
	static int nElements();
	// Return name of element with specified Z
	static const char* name(int Z);
	// Return symbol of element with specified Z
	static const char* symbol(int Z);
};

// Reference to Element, for use in constructing derived/associated data classes
class ElementReference
{
	public:
	// Constructor
	ElementReference(int Z, const char* symbol);

	private:
	// Referenced Element
	const Element& element_;

	public:
	// Return referenced element
	const Element& element() const;
	// Return atomic number (Z)
	int Z() const;
	// Return name of element
	const char* name() const;
	// Return symbol of element
	const char* symbol() const;
};

#endif
