
/*
	*** Dynamic character string
	*** src/base/charstring.cpp
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

#include "base/sysfunc.h"
#include "base/charstring.h"
#include "math/constants.h"
#include <cstring>
using namespace std;
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

// Constructors
CharString::CharString() : ListItem<CharString>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;
}

CharString::CharString(int emptysize) : ListItem<CharString>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;

	createEmpty(emptysize);
}

CharString::CharString(const char* fmt, ...) : ListItem<CharString>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;

	va_list arguments;
	static char s[8096];
	s[0] = '\0';

	// Parse the argument list (...) and internally write the output string into s[]
	va_start(arguments,fmt);
	vsprintf(s,fmt,arguments);
	va_end(arguments);
	set(s);
}

// Copy constructor
CharString::CharString(const CharString &source) : ListItem<CharString>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;

	if (source.data_ == NULL) clear();
	else set(source.data_);
}

// Destructor
CharString::~CharString()
{
	if (data_ != NULL) delete[] data_;
}

// Print
void CharString::info() const
{
	std::printf("CharString len = %i, end = %i : '%s'\n",size_,endPosition_,data_);
}

// Clear
void CharString::clear()
{
	if (data_ == NULL) return;
	endPosition_ = 0;
	data_[0] = '\0';
}

// Set from C-style string
void CharString::set(const char* s)
{
	// If new size is less than or equal to old size, don't reallocate
	int newsize = (s == NULL ? 1 : strlen(s) + 1);
	if (newsize > size_)
	{
		if (data_ != NULL) delete[] data_;
		data_ = new char[newsize];
	}
	size_ = newsize;
	endPosition_ = size_-1;
	if (s == NULL) data_[0] = '\0';
	else strcpy(data_, s);
}

// Get
const char* CharString::get() const
{
	return (data_ != NULL ? data_ : "");
}

// Get length
int CharString::length() const
{
	return (endPosition_ < size_ ? endPosition_ : size_);
}

// Create empty array
void CharString::createEmpty(int newsize)
{
	// Check if array has already been initialised
	if (data_ != NULL) delete[] data_;
	// Create new, empty array
	size_ = newsize;
	data_ = new char[newsize];
	endPosition_ = 0;
	data_[0] = '\0';
}

// Create empty array
void CharString::createEmpty(CharString &s)
{
	createEmpty(s.size_);
}

// Fill current string with specified character
void CharString::fill(char c)
{
	for (int n=0; n<size_-1; ++n) data_[n] = c;
	data_[size_-1] = '\0';
}

// Returns the length of the current string
bool CharString::isEmpty() const
{
	return (endPosition_ <= 0 ? true : false);
}

// Return last character of string (before '\0')
char CharString::lastChar() const
{
	return (endPosition_ == 0 ? '\0' : data_[endPosition_-1]);
}

// Return reference to char at specified position
char CharString::at(int n) const
{
	if ((n < 0) || (n >= endPosition_))
	{
		printf("Character %i is out of range.\n", n);
		return 0;
	}
	return data_[n];
}

/*
 * Erase / Cut
 */

// Erase range
void CharString::erase(int start, int end)
{
	// Retain original memory length of string, but move '\0' and decrease 'size_'
	// Check range given
	if (start >= endPosition_) return;
	if (end >= endPosition_) end = endPosition_ - 1;
	int count = endPosition_ - end;
	//printf("Range to erase is %i to %i.\n",start,end);
	//printf("Characters after endpoint = %i\n",count);
	// Copy the character in position 'n' to position 'start + (n-last-1)'
	//printf("   CharString - Before erase(%i,%i) = '%s', After = ",start,end,data_);
	for (int n=0; n<count; n++) data_[start+n] = data_[end+n+1];
	size_ -= (1 + end - start);
	endPosition_ -= (1 + end - start);
	//printf("'%s'\n",data_);
}

// Erase from start
void CharString::eraseStart(int n)
{
	//printf("erasestart - n = %i, endPosition_ = %i\n",n,endPosition_);
	if ((n - 1) > endPosition_)
	{
// 		printf("new (old) n = (%i) %i\n",n,endPosition_);
		n = endPosition_; 
	}
	if (n > 0) erase(0,n-1);
}

// Erase from end
void CharString::eraseEnd(int n)
{
	if ((n - 1) >= endPosition_) n = endPosition_;
	if (n > 0) erase(endPosition_-n,endPosition_-1);
}

// Erase from specified position to end
void CharString::eraseFrom(int n)
{
	if (n >= (endPosition_-1)) n = endPosition_-1;
	if (n > 0) erase(n,endPosition_-1);
}

// Cut characters from start
void CharString::cutStart(int len, CharString &target)
{
	// Set new size_ of target string
	target.createEmpty(len+1);
	for (int n=0; n<len; n++) target += data_[n];
	erase(0,len-1);
}

// Replace characters
void CharString::replace(char fromChar, char toChar)
{
	for (int n=0; n<endPosition_; ++n) if (data_[n] == fromChar) data_[n] = toChar;
}

// Replace multiple characters
void CharString::replace(const char* fromChars, char toChar)
{
	for (int n=0; n<endPosition_; ++n) if (std::strchr(fromChars, data_[n])) data_[n] = toChar;
}

/*
 * Operators
 */

// Assignment operator (const char*)
void CharString::operator=(const char* s)
{
	set(s);
}

// Assignment operator (const CharString&)
void CharString::operator=(const CharString& source)
{
	if (source.data_ == NULL) clear();
	else set(source.data_);
}

// Equality Operator (const char*)
bool CharString::operator==(const char* s) const
{
	if (data_ == NULL) return (s[0] == '\0');
	return (strcmp(data_,s) == 0);
}

// Inequality Operator (const char*)
bool CharString::operator!=(const char* s) const
{
	if (data_ == NULL) return (s[0] != '\0');
	return (strcmp(data_,s) != 0);
}

// Equality Operator
bool CharString::operator==(const CharString &s) const
{
	if (data_ == NULL)
	{
		if ((s.data_ == NULL) || (s.data_[0] == '\0')) return true;
		else return false;
	}
	else if (s.data_ == NULL) return (data_[0] == '\0');
	return (strcmp(data_,s.data_) == 0);
}

// Inequality Operator
bool CharString::operator!=(const CharString &s) const
{
	if (data_ == NULL)
	{
		if ((s.data_ == NULL) || (s.data_[0] == '\0')) return false;
		else return true;
	}
	else if (s.data_ == NULL) return (data_[0] != '\0');
	return (strcmp(data_,s.data_) != 0);
}

// Subscript operator
char& CharString::operator[](int n)
{
	if ((n < 0) || (n >= size_))
	{
		std::printf("CharString::operator[] <<<< Array subscript %i out of range (0-%i) >>>>\n",n,size_-1);
		static char dummy;
		return dummy;
	}
	return data_[n];
}

// Character addition
void CharString::operator+=(char c)
{
	// Check whether we need to reallocate
	if ((endPosition_+1) > (size_-1))
	{
		// We'll extend the size by 100 characters, rather than just 1 since we're likely to be adding more....
		size_ = endPosition_+100;
		char* newdata = new char[size_];
		if (data_ != NULL)
		{
			strcpy(newdata, data_);
			delete[] data_;
		}
		data_ = newdata;
	}
	data_[endPosition_] = c;
	++endPosition_;
	data_[endPosition_] = '\0';
}

// String addition operator
void CharString::operator+=(const char* s)
{
	strcat(s);
}

// Conversion operators
CharString::operator const char*()
{
	return get();
}

/*
 * Conversion
 */

// Return as double
double CharString::asDouble() const
{
	return (data_ != NULL ? atof(data_) : 0.0);
}

// Return as integer
int CharString::asInteger() const
{
	return (data_ != NULL ? atoi(data_) : 0);
}

// Returns contents as long integer
long int CharString::asLongInteger() const
{
	return (data_ != NULL ? atol(data_) : 0);
}

// Return as bool
bool CharString::asBool() const
{
	// Convert string to boolean
	bool result = false;
	CharString lcase(DissolveSys::lowerCase(data_));
	if (lcase == "off") result = false;
	else if (lcase == "on") result = true;
	else if (lcase == "no") result = false;
	else if (lcase == "yes") result = true;
	else if (lcase == "false") result = false;
	else if (lcase == "true") result = true;
	else
	{
		std::printf("Character constant '%s' doesn't translate directly to a boolean value - false assumed.\n", lcase.get());
		result = false;
	}
	return result;
}

// Is Number?
bool CharString::isNumeric() const
{
	// Go through string - if we find a 'non-number' character, return false
	int nSymbols = 0, nChars = 0;
	for (char* c = data_; *c != '\0'; ++c)
	{
		++nChars;
		switch (*c)
		{
			case (' '): case ('e'): case ('E'):
			case ('+'): case ('-'): case ('.'):
				++nSymbols;
				break;
			case ('0'): case ('1'): case ('2'): case ('3'): case ('4'): 
			case ('5'): case ('6'): case ('7'): case ('8'): case ('9'): 
				break;
			default:
				return false;
		}
	}
	return (nSymbols != nChars);
}

// Return the lowercase conversion of the string
const char* CharString::lower() const
{
	if (data_ == NULL) return "";
	return DissolveSys::lowerCase(data_);
}

// Return the uppercase conversion of the string
const char* CharString::upper() const
{
	if (data_ == NULL) return "";
	return DissolveSys::upperCase(data_);
}

/*
 * Search
 */

// Find character
int CharString::find(char search) const
{
	int count = 0;
	char* c;
	for (c = data_; *c != '\0'; c++)
	{
// 	printf("CharString %c %c\n",*c,search);
		if (*c == search) return count;
		++count;
	}
	return -1;
}

// Reverse find character
int CharString::rFind(char search, char stopat1, char stopat2) const
{
	int result;
	for (result = endPosition_; result >= 0; --result)
	{
		if (data_[result] == stopat1) return -1;
		if (data_[result] == stopat2) return -1;
		if (data_[result] == search) break;
	}
	return result;
}

/*
 * C-String Functions
 */

// String addition
void CharString::strcat(const char* s, int charcount)
{
	if (charcount == 0) return;
	// Check whether we need to reallocate
	int slen = strlen(s);
	if ((charcount != -1) && (charcount <= slen)) slen = charcount;
	if ((slen+endPosition_) > (size_-1))
	{
		size_ = slen+endPosition_+1;
		char* newdata = new char[size_];
		if (data_ != NULL)
		{
			strcpy(newdata, data_);
			delete[] data_;
		}
		data_ = newdata;
	}
	for (const char* c = s; *c != '\0'; ++c)
	{
		// If we're passed \0, ignore it (since we already have one)
		// Check size_ of array
		if (endPosition_ == (size_ - 1))
		{
			printf("CharString::cat <<<< Buffer overflow - blame shoddy programming >>>>\n");
			return;
		}
		data_[endPosition_] = *c;
		++endPosition_;
		--slen;
		if (slen == 0) break;
	}
	data_[endPosition_] = '\0';
}

// Append formatted string
void CharString::strcatf(const char* fmt ...)
{
	va_list arguments;
	static char s[8096];
	s[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments,fmt);
	vsprintf(s,fmt,arguments);
	va_end(arguments);
	// Now, append to existing string
	strcat(s);
}

// Create formatted string
void CharString::sprintf(const char* fmt ...)
{
	va_list arguments;
	static char s[8096];
	s[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments,fmt);
	vsprintf(s,fmt,arguments);
	set(s);
	va_end(arguments);
}

// Search for character in string
char *CharString::strchr(char c) const
{
	return std::strchr(data_, c);
}

// Copy substring of supplied string into this string
void CharString::substr(const char* source, int pos, int nchars)
{
	clear();
	// Check start position
	int len = strlen(source);
	if ((pos < 0) || (pos >= len)) return;
	// Copy characters
	const char* c = &source[pos];
	for (int n=0; n<nchars; ++n)
	{
		if (*c == '\0') break;
		(*this) += *c;
		c = c+1;
	}
}

// Static sprintf
const char* CharString::string(const char* fmt, ...)
{
	va_list arguments;
	static char s[8096];
	s[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments,fmt);
	vsprintf(s, fmt, arguments);
	va_end(arguments);
	return s;
}
