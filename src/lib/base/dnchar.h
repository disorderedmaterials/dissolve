/*
	*** Dynamic character array
	*** src/lib/base/dnchar.h
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

#ifndef DUQ_DNCHAR_H
#define DUQ_DNCHAR_H

#include "templates/list.h"

// Dynamic character array
class Dnchar : public ListItem<Dnchar>
{
	public:
	// Constructor / Destructor
	Dnchar();
	Dnchar(int size);
	Dnchar(const char* s);
	Dnchar(int dummyparameter, const char* fmt, ...);
	~Dnchar();
	// Copy constructor
	Dnchar(const Dnchar&);
	// Conversion operators
	operator const char*();


	/*
	// Data / Access
	*/
	private:
	// Current size
	int size_;
	// Position of '\0' character
	int endPosition_;
	// Pointer to character array
	char* data_;

	public:
	// Print string info
	void info() const;
	// Clear string but don't free
	void clear();
	// Set value
	void set(const char* s);
	// Get value
	const char* get() const;
	// Resize data
	void createEmpty(int);
	// Resize data (to be same length as source Dnchar)
	void createEmpty(Dnchar&);
	// Returns the length of the current string
	int length() const;
	// Returns TRUE if current length is 1 or less.
	bool isEmpty() const;
	// Return last character of string (before '\0')
	char lastChar() const;

	
	/*
	// Erase / Cut
	*/
	public:
	// Erase range of characters from the string
	void erase(int, int);
	// Erase 'n' characters from start of string
	void eraseStart(int);
	// Erase 'n' characters from end of string
	void eraseEnd(int);
	// Erase from nth character to the end of string
	void eraseFrom(int);
	// Cut n characters from start of string and place in other
	void cutStart(int, Dnchar&);
	// Replace characters
	void replace(char from, char to);


	/*
	// Operators
	*/
	public:
	// Assignment operator
	void operator=(const char*);
	// Assignment operator
	void operator=(const Dnchar&);
	// Equality operator (const char*)
	bool operator==(const char*) const;
	// Inequality operator (const char*)
	bool operator!=(const char*) const;
	// Equality operator
	bool operator==(const Dnchar&) const;
	// Inequality operator
	bool operator!=(const Dnchar&) const;
	// Array subscript operator
	char operator[](int) const;
	// Character addition operator
	void operator+=(char);


	/*
	// Conversion
	*/
	public:
	// Returns contents as double
	double asDouble() const;
	// Returns contents as integer
	int asInteger() const;
	// Returns contents as bool
	bool asBool() const;
	// Returns true if the string contains a number
	bool isNumeric() const;
	// Return the lowercase conversion of the string
	const char* lower() const;
	// Return the uppercase conversion of the string
	const char* upper() const;


	/*
	// Search (Returning integer index)
	*/
	public:
	// Find position of first occurrence of character 'c'
	int find(char c) const;
	// Find position of last occurrence of character 'c'
	int rFind(char c, char stopat1 = '\0', char stopat2 = '\0') const;


	/*
	// C-String Routines
	*/
	public:
	// Concnotepsrate supplied string on to end of this string
	void strcat(const char* s, int charcount = -1);
	// Append formatted string (not actually a C string function, but would be useful!)
	void strcatf(const char* fmt, ...);
	// Create formatted string
	void sprintf(const char* fmt, ...);
	// Search for character in string
	char* strchr(char c) const;
	// Copy substring of supplied string into this string
	void substr(const char* source, int pos, int nchars);
	// Static sprintf
	static const char* string(const char* fmt, ...);
};

#endif
