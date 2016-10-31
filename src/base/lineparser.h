/*
	*** Line Parsing Routines
	*** src/base/lineparser.h
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

#ifndef DUQ_LINEPARSER_H
#define DUQ_LINEPARSER_H

#include "base/dnchar.h"
#include "math/constants.h"
#include "templates/list.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

#define MAXLINELENGTH 1024

// Forward Declarations
class Format;
class ProcessPool;

// Line Parser
class LineParser
{
	public:
	// Constructor
	LineParser();
	// Destructor
	~LineParser();
	// Parse Options Enum
	enum ParseOption
	{
		Defaults = 0,		/* Default parsing behaviour */
		StripComments = 1,	/* Automatically strip comments (beginning # or //) from files */
		UseQuotes = 2,		/* Quoted text in files is retained as a single delimited argument */
		SkipBlanks = 4,		/* Skip blank lines (those containing nothing or only whitespace) */
		StripBrackets = 8,	/* Remove parentheses during parsing */
		NoEscapes = 16,		/* Don't convert excaped characters */
		UseBraces = 32,		/* Text inside curly brackets is retaind as a single argument */
		NormalCommas = 64,	/* Don't treat commas as delimiting marks in files */
		DummyOption = 128,
		nParseOptions = 9
	};


	/*
	 * Source line/file and read options
	 */
	private:
	// Current input filename (if any)
	Dnchar inputFilename_;
	// CUrrent output filename (if any)
	Dnchar outputFilename_;
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
	// Target stream for writing
	ofstream* outputFile_;
	// Target stream for cached writing
	stringstream* cachedFile_;

	public:
	// Reset data
	void reset();
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
	// Open new stream for writing
	bool openOutput(const char* filename, bool directOutput);
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
	// Whether output is cached or direct
	bool directOutput_;
	// Gets all delimited args from internal line
	void getAllArgsDelim(int optionMask);

	public:
	// Gets next delimited arg from internal line
	bool getNextArg(int optionMask, Dnchar* destarg);
	// Gets next n chars from internal line
	bool getNextN(int optionMask, int length, Dnchar* destarg = NULL);
	// Read line from file and do delimited parse
	int getArgsDelim(int optionMask);
	// Get rest of line starting at next delimited part
	bool getRestDelim(Dnchar* destarg = NULL);
	// Set line and parse using delimiters
	void getArgsDelim(int optionMask, const char* s);
	// Get next delimited chunk from file (not line)
	bool getCharsDelim(Dnchar* destarg = NULL);
	// Get next delimited chunk from string, removing grabbed part
	bool getCharsDelim(int optionMask, Dnchar* source, Dnchar* destarg);
	// Read next line from internal source file, setting as parsing source
	int readNextLine(int optionMask);
	// Skip 'n' lines from internal file
	int skipLines(int nskip);
	// Get next delimited argument from internal line
	bool getArgDelim(int optionMask, Dnchar* destarg);
	// Return a number of characters from the input stream
	const char* getChars(int nchars, bool skipeol = true);
	// Skip a number of characters from the input stream
	void skipChars(int nchars);
	// Return an integer value from reading 'n' chars of an (unformatted) input file
	int getInteger(int nbytes = 0);
	// Fill an array of integer values from reading of an (unformatted) input file
	int getIntegerArray(int* array, int count);
	// Return a double value from reading 'n' chars of an (unformatted) input file
	double getDouble(int nbytes = 0);
	// Fill an array of double values from reading of an (unformatted) input file
	int getDoubleArray(double* array, int count);
	// Write line to file
	bool writeLine(const char* s) const;
	// Write formatted line to file
	bool writeLineF(const char* fmt, ...) const;
	// Commit cached output stream to actual output file
	bool commitCache();


	/*
	 * Parallel Read/Write Routines
	 */
	public:
	// Return whether the end of the input stream has been reached (or only whitespace remains)
	bool eofOrBlank(ProcessPool& worldPool) const;
	// Read line from file and do delimited parse
	int getArgsDelim(ProcessPool& worldPool, int optionMask);


	/*
	 * Argument Data
	 */
	private:
	// Temporary string variable
	char tempArg_[MAXLINELENGTH];
	// Parsed arguments
	List<Dnchar> arguments_;
	// Whether the end of the string has been found in get_next_arg()
	bool endOfLine_;

	public:
	// Returns number of arguments grabbed from last parse
	int nArgs() const;
	// Returns the specified argument as a character string
	const char* argc(int i);
	// Returns the specified argument as an integer
	int argi(int i);
	// Returns the specified argument as a double
	double argd(int i);
	// Returns the specified argument as a bool
	bool argb(int i);
	// Returns the specified argument as a float
	float argf(int i);
	// Returns whether the specified argument exists
	bool hasArg(int i) const;
};

#endif
