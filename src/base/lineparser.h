/*
	*** Line Parsing Routines
	*** src/base/lineparser.h
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

#ifndef DISSOLVE_LINEPARSER_H
#define DISSOLVE_LINEPARSER_H

#include "base/processpool.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/vector3.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

#define MAXLINELENGTH 8092

// Forward Declarations
/* none */

// Line Parser
class LineParser
{
	public:
	// Constructor
	LineParser(ProcessPool* procPool = NULL);
	// Destructor
	~LineParser();
	// Parse Options Enum
	enum ParseOption
	{
		Defaults = 0,			/* Default parsing behaviour */
		KeepComments = 1,		/* Automatically strip comments (beginning # or //) from files */
		IgnoreQuotes = 2,		/* Quoted text in files will not be retained as a single argument */
		KeepBlanks = 4,			/* Don't automatically skip blank lines (those containing nothing or only whitespace) */
		StripBrackets = 8,		/* Remove parentheses during parsing */
		NoEscapes = 16,			/* Don't convert excaped characters */
		UseBraces = 32,			/* Text inside curly brackets is retaind as a single argument */
		CommasAreDelimiters = 64,	/* Treat commas as delimiting marks in files */
		SemiColonLineBreaks = 128,	/* Treat semicolons as line-breaks */
		nParseOptions = 9
	};
	// Parse Return Value
	enum ParseReturnValue
	{
		EndOfFile		= -1,	/* Operation encountered the end of the current file */
		Success			= 0,	/* Operation succeeded */
		Fail			= 1	/* Operation failed */
	};


	/*
	 * Source line/file and read options
	 */
	private:
	// Associated process pool (if any)
	ProcessPool* processPool_;
	// Current input filename (if any)
	CharString inputFilename_;
	// Current output filename (if any)
	CharString outputFilename_;
	// Line to parse
	char line_[MAXLINELENGTH];
	// Length of line_
	int lineLength_;
	// Current reading position in line
	int linePos_;
	// Integer line number of last read line
	int lastLineNo_;
	// Source stream for reading
	ifstream* inputFile_;
	// Source string stream for reading
	stringstream* inputStrings_;
	// Target stream for writing
	ofstream* outputFile_;
	// Target stream for cached writing
	stringstream* cachedFile_;

	private:
	// Return current stream for input
	istream* inputStream() const;

	public:
	// Reset data
	void reset();
	// Return associated process pool (if any)
	ProcessPool* processPool() const;
	// Return filename of current inputFile (if any)
	const char* inputFilename() const;
	// Return filename of current outputFile (if any)
	const char* outputFilename() const;
	// Return pointer to start of current line
	const char* line() const;
	// Set line target
	void setLine(const char* s);
	// Return integer line number of last read line
	int lastLineNo() const;
	// Return read-only status of file
	bool isFileReadOnly() const;
	// Open new file for reading
	bool openInput(const char* filename);
	// Open input string for reading
	bool openInputString(const char* string);
	// Open new stream for writing
	bool openOutput(const char* filename, bool directOutput = true);
	// Open existing stream for writing
	bool appendOutput(const char* filename);
	// Close file(s)
	void closeFiles();
	// Return whether current file source is good for reading
	bool isFileGoodForReading() const;
	// Return whether current file source is good for writing
	bool isFileGoodForWriting() const;
	// Tell current position of input stream
	streampos tellg() const;
	// Peek next character in input stream
	char peek() const;
	// Seek position in input stream
	void seekg(streampos pos);
	// Seek n bytes in specified direction in input stream
	void seekg(streamoff off, ios_base::seekdir dir);
	// Rewind input stream to start
	void rewind();
	// Return whether the end of the input stream has been reached (or only whitespace remains)
	bool eofOrBlank() const;


	/*
	 * Read/Write Routines
	 */
	private:
	// Whether input is from file or strings
	bool fileInput_;
	// Whether output is cached or direct
	bool directOutput_;
	// Gets all delimited args from internal line
	void getAllArgsDelim(int optionMask);
	// Working storage for text to write
	char workingText_[8096];

	public:
	// Gets next delimited arg from internal line
	bool getNextArg(int optionMask, CharString* destarg);
	// Gets next n chars from internal line
	bool getNextN(int optionMask, int length, CharString* destarg = NULL);
	// Read line from file and do delimited parse
	ParseReturnValue getArgsDelim(int optionMask = LineParser::Defaults);
	// Get rest of line starting at next delimited part
	bool getRestDelim(CharString* destarg = NULL);
	// Set line and parse using delimiters
	void getArgsDelim(int optionMask, const char* s);
	// Get next delimited chunk from file (not line)
	bool getCharsDelim(CharString* destarg = NULL);
	// Get next delimited chunk from string, removing grabbed part
	bool getCharsDelim(int optionMask, CharString* source, CharString* destarg);
	// Read next line from internal source file, setting as parsing source
	ParseReturnValue readNextLine(int optionMask);
	// Read next line from internal source file, setting as parsing source and storing in specified CharString
	ParseReturnValue readNextLine(int optionMask, CharString& dest);
	// Skip 'n' lines from internal file
	ParseReturnValue skipLines(int nskip);
	// Get next delimited argument from internal line
	bool getArgDelim(int optionMask, CharString* destarg);
	// Return a number of characters from the input stream
	const char* getChars(int nchars, bool skipeol = true);
	// Write line to file
	bool writeLine(const char* s) const;
	// Write formatted line to file
	bool writeLineF(const char* fmt, ...) const;
	// Print banner comment of fixed width
	bool writeBannerComment(const char* fmt, ...);
	// Write long int argument as single line
	bool writeArg(long int i) const;
	// Write long long int argument as single line
	bool writeArg(long long int i) const;
	// Read long int argument as single line
	bool readArg(long int& i);
	// Read long long int argument as single line
	bool readArg(long long int& i);
	// Commit cached output stream to actual output file
	bool commitCache();


	/*
	 * Argument Data
	 */
	private:
	// Temporary string variable
	char tempArg_[MAXLINELENGTH];
	// Parsed arguments
	List<CharString> arguments_;
	// Whether the end of the string has been found in get_next_arg()
	bool endOfLine_;

	public:
	// Returns number of arguments grabbed from last parse
	int nArgs() const;
	// Returns the specified argument as a character string
	const char* argc(int i);
	// Returns the specified argument as an integer
	int argi(int i);
	// Returns the specified argument as a long integer
	long int argli(int i);
	// Returns the specified argument as a double
	double argd(int i);
	// Returns the specified argument as a bool
	bool argb(int i);
	// Returns the specified argument as a float
	float argf(int i);
	// Return the specified and next two arguments as a Vec3<int>
	Vec3<int> arg3i(int i);
	// Return the specified and next two arguments as a Vec3<double>
	Vec3<double> arg3d(int i);
	// Returns whether the specified argument exists
	bool hasArg(int i) const;
	// Return specified argument as int, placing in variable provided
	bool argAs(int i, int& destination);
	// Return specified argument as double, placing in variable provided
	bool argAs(int i, double& destination);
	// Return specified argument as bool, placing in variable provided
	bool argAs(int i, bool& destination);
	// Return specified argument as Vec3<double>, placing in variable provided
	bool argAs(int i, Vec3<int>& destination);
	// Return specified argument as Vec3<double>, placing in variable provided
	bool argAs(int i, Vec3<double>& destination);
};

#endif
