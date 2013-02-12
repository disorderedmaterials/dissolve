/*
	*** Dynamic character array
	*** src/lib/base/dnchar.cpp
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

#include "base/sysfunc.h"
#include "base/dnchar.h"
#include "math/constants.h"
#include <cstring>
using namespace std;
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

// Constructors
Dnchar::Dnchar() : ListItem<Dnchar>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;
}

Dnchar::Dnchar(int emptysize) : ListItem<Dnchar>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;

	createEmpty(emptysize);
}

Dnchar::Dnchar(const char* s) : ListItem<Dnchar>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;

	set(s);
}

Dnchar::Dnchar(int dummyparameter, const char* fmt, ...) : ListItem<Dnchar>()
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
Dnchar::Dnchar(const Dnchar &source) : ListItem<Dnchar>()
{
	// Private variables
	data_ = NULL;
	size_ = 0;
	endPosition_ = 0;

	if (source.data_ == NULL) clear();
	else set(source.data_);
}

// Conversion operators
Dnchar::operator const char*()
{
	return get();
}

// Destructor
Dnchar::~Dnchar()
{
	if (data_ != NULL) delete[] data_;
}

// Print
void Dnchar::info() const
{
	std::printf("DnChar len = %i, end = %i : '%s'\n",size_,endPosition_,data_);
}

// Clear
void Dnchar::clear()
{
	if (data_ == NULL) return;
	endPosition_ = 0;
	data_[0] = '\0';
}

// Set from C-style string
void Dnchar::set(const char* s)
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
	else strcpy(data_,s);
}

// Get
const char *Dnchar::get() const
{
	return (data_ != NULL ? data_ : "");
}

// Get length
int Dnchar::length() const
{
	return (endPosition_ < size_ ? endPosition_ : size_);
}

// Create empty array
void Dnchar::createEmpty(int newsize)
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
void Dnchar::createEmpty(Dnchar &s)
{
	createEmpty(s.size_);
}

// Empty?
bool Dnchar::isEmpty() const
{
	return (endPosition_ <= 0 ? TRUE : FALSE);
}

// Return last character of string (before '\0')
char Dnchar::lastChar() const
{
	return (endPosition_ == 0 ? '\0' : data_[endPosition_-1]);
}

/*
// Erase / Cut
*/

// Erase range
void Dnchar::erase(int start, int end)
{
	// Retain original memory length of string, but move '\0' and decrease 'size_'
	// Check range given
	if (start >= endPosition_) return;
	if (end >= endPosition_) end = endPosition_ - 1;
	int count = endPosition_ - end;
	//printf("Range to erase is %i to %i.\n",start,end);
	//printf("Characters after endpoint = %i\n",count);
	// Copy the character in position 'n' to position 'start + (n-last-1)'
	//printf("   DNCHAR - Before erase(%i,%i) = '%s', After = ",start,end,data_);
	for (int n=0; n<count; n++) data_[start+n] = data_[end+n+1];
	size_ -= (1 + end - start);
	endPosition_ -= (1 + end - start);
	//printf("'%s'\n",data_);
}

// Erase from start
void Dnchar::eraseStart(int n)
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
void Dnchar::eraseEnd(int n)
{
	if ((n - 1) >= endPosition_) n = endPosition_;
	if (n > 0) erase(endPosition_-n,endPosition_-1);
}

// Erase from specified position to end
void Dnchar::eraseFrom(int n)
{
	if (n >= (endPosition_-1)) n = endPosition_-1;
	if (n > 0) erase(n,endPosition_-1);
}

// Cut characters from start
void Dnchar::cutStart(int len, Dnchar &target)
{
	// Set new size_ of target string
	target.createEmpty(len+1);
	for (int n=0; n<len; n++) target += data_[n];
	erase(0,len-1);
}

// Replace characters
void Dnchar::replace(char from, char to)
{
	for (int n=0; n<endPosition_; ++n) if (data_[n] == from) data_[n] = to;
}

/*
// Operators
*/

// Assignment operator (const char*)
void Dnchar::operator=(const char* s)
{
	set(s);
}

// Assignment operator (const Dnchar&)
void Dnchar::operator=(const Dnchar &source)
{
	if (source.data_ == NULL) clear();
	else set(source.data_);
}

// Equality Operator (const char*)
bool Dnchar::operator==(const char* s) const
{
	if (data_ == NULL) return (s[0] == '\0');
	return (strcmp(data_,s) == 0);
}

// Inequality Operator (const char*)
bool Dnchar::operator!=(const char* s) const
{
	if (data_ == NULL) return (s[0] != '\0');
	return (strcmp(data_,s) != 0);
}

// Equality Operator
bool Dnchar::operator==(const Dnchar &s) const
{
	if (data_ == NULL)
	{
		if ((s.data_ == NULL) || (s.data_[0] == '\0')) return TRUE;
		else return FALSE;
	}
	else if (s.data_ == NULL) return (data_[0] == '\0');
	return (strcmp(data_,s.data_) == 0);
}

// Inequality Operator
bool Dnchar::operator!=(const Dnchar &s) const
{
	if (data_ == NULL)
	{
		if ((s.data_ == NULL) || (s.data_[0] == '\0')) return FALSE;
		else return TRUE;
	}
	else if (s.data_ == NULL) return (data_[0] != '\0');
	return (strcmp(data_,s.data_) != 0);
}

// Subscript operator
char Dnchar::operator[](int n) const
{
	if ((n < 0) || (n >= size_))
	{
		std::printf("Dnchar::operator[] <<<< Array subscript %i out of range (0-%i) >>>>\n",n,size_-1);
		return 0;
	}
	return data_[n];
}

// Character addition
void Dnchar::operator+=(char c)
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

/*
// Conversion
*/

// Return as double
double Dnchar::asDouble() const
{
	return (data_ != NULL ? atof(data_) : 0.0);
}

// Return as integer
int Dnchar::asInteger() const
{
	return (data_ != NULL ? atoi(data_) : 0);
}

// Return as bool
bool Dnchar::asBool() const
{
	// Convert string to boolean
	bool result = FALSE;
	Dnchar lcase(lowerCase(data_));
	if (lcase == "off") result = FALSE;
	else if (lcase == "on") result = TRUE;
	else if (lcase == "no") result = FALSE;
	else if (lcase == "yes") result = TRUE;
	else if (lcase == "false") result = FALSE;
	else if (lcase == "true") result = TRUE;
	else
	{
		std::printf("Character constant '%s' doesn't translate directly to a boolean value - FALSE assumed.\n", lcase.get());
		result = FALSE;
	}
	return result;
}

// Is Number?
bool Dnchar::isNumeric() const
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
				return FALSE;
		}
	}
	return (nSymbols != nChars);
}

// Return the lowercase conversion of the string
const char *Dnchar::lower() const
{
	if (data_ == NULL) return "\0";
	return lowerCase(data_);
}

// Return the uppercase conversion of the string
const char *Dnchar::upper() const
{
	if (data_ == NULL) return "\0";
	return upperCase(data_);
}

/*
// Search
*/

// Find character
int Dnchar::find(char search) const
{
	int count = 0;
	char* c;
	for (c = data_; *c != '\0'; c++)
	{
// 	printf("Dnchar %c %c\n",*c,search);
		if (*c == search) return count;
		++count;
	}
	return -1;
}

// Reverse find character
int Dnchar::rFind(char search, char stopat1, char stopat2) const
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
// C-String Functions
*/

// String addition
void Dnchar::strcat(const char* s, int charcount)
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
			printf("Dnchar::cat <<<< Buffer overflow - blame shoddy programming >>>>\n");
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
void Dnchar::strcatf(const char* fmt ...)
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
void Dnchar::sprintf(const char* fmt ...)
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
char *Dnchar::strchr(char c) const
{
	return std::strchr(data_, c);
}

// Copy substring of supplied string into this string
void Dnchar::substr(const char* source, int pos, int nchars)
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
const char* Dnchar::string(const char* fmt, ...)
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
