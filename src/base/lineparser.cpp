/*
	*** Line Parsing Routines
	*** src/base/lineparser.cpp
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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include "base/processpool.h"
#include <string.h>
#include <stdarg.h>

// Constructor
LineParser::LineParser()
{
	arguments_.clear();
	reset();
}

// Destructor
LineParser::~LineParser()
{
	if (inputFile_ != NULL) delete inputFile_;
	if (outputFile_ != NULL) delete outputFile_;
	if (cachedFile_ != NULL) delete cachedFile_;
}

// Reset data
void LineParser::reset()
{
	inputFilename_.clear();
	outputFilename_.clear();
	endOfLine_ = false;
	lineLength_ = 0;
	linePos_ = 0;
	lastLineNo_ = 0;
	inputFile_ = NULL;
	outputFile_ = NULL;
	cachedFile_ = NULL;
	directOutput_ = false;
	arguments_.clear();
}

/*
 * Source line/file and read options
 */

// Return filename of current input file (if any)
const char* LineParser::inputFilename() const
{
	return inputFilename_.get();
}

// Return filename of current output file (if any)
const char* LineParser::outputFilename() const
{
	return outputFilename_.get();
}

// Return pointer to current line
const char* LineParser::line() const
{
	return line_;
}

// Set line target
void LineParser::setLine(const char* s)
{
	strncpy(line_, s, MAXLINELENGTH);
	lineLength_ = strlen(line_);
	linePos_ = 0;
}

// Return integer line number of last read line
int LineParser::lastLineNo() const
{
	return lastLineNo_;
}

// Open new file for reading
bool LineParser::openInput(const char* filename)
{
	// Check existing input file
	if (inputFile_ != NULL)
	{
		printf("Warning - LineParser already appears to have an open file...\n");
		inputFile_->close();
		delete inputFile_;
		inputFile_ = NULL;
	}
	// Open new file
	inputFile_ = new ifstream(filename, ios::in | ios::binary);
	if (!inputFile_->is_open())
	{
		closeFiles();
		Messenger::error("Failed to open file '%s' for reading.\n", filename);
		return false;
	}
	// Reset variables
	lastLineNo_ = 0;
	inputFilename_ = filename;
	return true;
}

// Open new stream for writing
bool LineParser::openOutput(const char* filename, bool directOutput)
{
	// Check existing input file
	if ((outputFile_ != NULL) || (cachedFile_ != NULL))
	{
		printf("Warning - LineParser already appears to have an open file/cache...\n");
		if (outputFile_ != NULL)
		{
			outputFile_->close();
			delete outputFile_;
			outputFile_ = NULL;
		}
		if (cachedFile_ != NULL)
		{
			delete cachedFile_;
			cachedFile_ = NULL;
		}
	}
	// Open new file
	directOutput_ = directOutput;
	if (directOutput_)
	{
		outputFile_ = new ofstream(filename, ios::out);
		if (!outputFile_->is_open())
		{
			closeFiles();
			Messenger::error("Failed to open file '%s' for writing.\n", filename);
			return false;
		}
	}
	else cachedFile_ = new stringstream;

	outputFilename_ = filename;
	return true;
}

// Open existing stream for writing
bool LineParser::appendOutput(const char* filename)
{
	// Check existing input file
	if ((outputFile_ != NULL) || (cachedFile_ != NULL))
	{
		printf("Warning - LineParser already appears to have an open file/cache...\n");
		if (outputFile_ != NULL)
		{
			outputFile_->close();
			delete outputFile_;
			outputFile_ = NULL;
		}
		if (cachedFile_ != NULL)
		{
			delete cachedFile_;
			cachedFile_ = NULL;
		}
	}

	// Open new file
	directOutput_ = true;
	outputFile_ = new ofstream(filename, ios::app);
	if (!outputFile_->is_open())
	{
		closeFiles();
		Messenger::error("Failed to open file '%s' for writing.\n", filename);
		return false;
	}

	outputFilename_ = filename;
	return true;
}

// Close file 
void LineParser::closeFiles()
{
	if (inputFile_ != NULL)
	{
		inputFile_->close();
		delete inputFile_;
	}
	if (outputFile_ != NULL)
	{
		outputFile_->close();
		delete outputFile_;
	}
	reset();
}

// Return whether current file source is good for reading
bool LineParser::isFileGoodForReading() const
{
	if (inputFile_ == NULL) return false;
	else if (!inputFile_->is_open()) return false;
	return true;
}

// Return whether current file source is good for writing
bool LineParser::isFileGoodForWriting() const
{
	if (directOutput_)
	{
		if (outputFile_ == NULL) return false;
		else if (!outputFile_->is_open()) return false;
		return true;
	}
	else return true;
}

// Peek next character in input stream
char LineParser::peek() const
{
	if (inputFile_ == NULL) return '\0';
	return inputFile_->peek();
}

// Tell current position of input stream
streampos LineParser::tellg() const
{
	streampos result = 0;
	if (inputFile_ != NULL) result = inputFile_->tellg();
	else printf("Warning: LineParser tried to tellg() on a non-existent input file.\n");
	return result;
}

// Seek position in input stream
void LineParser::seekg(streampos pos)
{
	if (inputFile_ != NULL)
	{
		if (inputFile_->eof()) inputFile_->clear();
		inputFile_->seekg(pos);
	}
	else printf("Warning: LineParser tried to seekg() on a non-existent input file.\n");
}

// Seek n bytes in specified direction in input stream
void LineParser::seekg(streamoff off, ios_base::seekdir dir)
{
	if (inputFile_ != NULL) inputFile_->seekg(off, dir);
	else printf("Warning: LineParser tried to seekg() on a non-existent input file.\n");
}

// Rewind input stream to start
void LineParser::rewind()
{
	if (inputFile_ != NULL) inputFile_->seekg(0, ios::beg);
	else Messenger::print("No file currently open to rewind.\n");
}

// Return whether the end of the input stream has been reached (or only whitespace remains)
bool LineParser::eofOrBlank() const
{
	if (inputFile_ == NULL) return true;
	// Simple check first - is this the end of the file?
	if (inputFile_->eof()) return true;
	// Otherwise, store the current file position and search for a non-whitespace character (or end of file)
	streampos pos = inputFile_->tellg();
	
	// Skip through whitespace, searching for 'hard' character
	char c;
	bool result = true;
	do
	{
		inputFile_->get(c);
		if (inputFile_->eof()) break;
		// If a whitespace character then skip it....
		if ((c == ' ') || (c == '\r') || ( c == '\n') || (c == '\t') || (c == '\0'))
		{
			if (inputFile_->eof()) break;
			else continue;
		}
		result = false;
		break;
	} while (1);
	inputFile_->seekg(pos);
	
	return result;
}

/*
 * Read/Write Routines
 */

// Read single line from internal file source
int LineParser::readNextLine(int optionMask)
{
	// Returns : 0=ok, 1=error, -1=eof
	if (inputFile_ == NULL)
	{
		printf("Error: No input file open for LineParser::readNextLine.\n");
		return 1;
	}
	if (inputFile_->eof())
	{
		return -1;
	}
	
	// Loop until we get 'suitable' line from file
	int nchars, nspaces, result = 0;
	do
	{
		char chr;
		lineLength_ = 0;
		while(inputFile_->get(chr).good())
		{
			if (chr == '\r')
			{
				if (inputFile_->peek() == '\n') inputFile_->ignore();
				break;
			}
			else if (chr == '\n') break;
			line_[lineLength_++] = chr;
			// Check here for overfilling the line_ buffer - perhaps it's a binary file?
			if (lineLength_ == MAXLINELENGTH)
			{
				return -1;
			}
		}
		line_[lineLength_] = '\0';
//		Messenger::print(Messenger::Parse, "Line from file is: [%s]\n", line_);

		// Remove comments from line
		if (optionMask&LineParser::StripComments) DUQSys::removeComments(line_);
		
		// If we are skipping blank lines, check for a blank line here
		if (optionMask&LineParser::SkipBlanks)
		{
			// Now, see if our line contains only blanks
			nchars = 0;
			nspaces = 0;
			for (char* c = line_; *c != '\0'; c++)
			{
				nchars++;
				if (isspace(*c)) nspaces++;
			}
			if (nchars == nspaces) result = -1;
			else result = 0;
		}
		else result = 0;
		
		// If result is 0, everything went okay, but if not we got a blank line. EOF or failed perhaps?
		if (result == -1)
		{
			if (inputFile_->eof())
			{
				return -1;
			}
			if (inputFile_->fail())
			{
				return 1;
			}
		}
		lineLength_ = strlen(line_);
		linePos_ = 0;
		lastLineNo_ ++;
	} while (result != 0);
	// printf("LineParser Returned line = [%s], length = %i\n",line_,lineLength_);

	return result;
}

// Gets next delimited arg from internal line
bool LineParser::getNextArg(int optionMask, CharString* destarg)
{
	// Get the next input chunk from the internal string and put into argument specified.
	int arglen;
	bool done, hadquotes, failed;
	char c, quotechar;
	failed = false;
	done = false;
	hadquotes = false;
	quotechar = '\0';
	endOfLine_ = false;
	arglen = 0;
	if (endOfLine_)
	{
		destarg->clear();
		// printf("Lineparser is at end of line - returning...\n");
		return true;
	}
	while (linePos_ < lineLength_)
	{
		c = line_[linePos_];
		switch (c)
		{
			// End of line markers
			case (10):	// Line feed (\n)
			case (13):	// Carriage Return
				done = true;
				endOfLine_ = true;
				break;
			// Delimiters
			// If we encounter one and arg length != 0 this signals the end of the argument.
			case (','):	// Comma
				if (optionMask&LineParser::NormalCommas)
				{
					tempArg_[arglen++] = c;
					break;
				}
			case (9):	// Horizontal Tab
			case (' '):	// Space
				if (quotechar != '\0') tempArg_[arglen++] = c;
				else if (arglen != 0) done = true;
				break;
			// Quote marks
			// If LineParser::UseQuotes, keep delimiters and other quote marks inside the quoted text.
			case (34):	// Double quotes
			case (39):	// Single quotes
				if (!(optionMask&LineParser::UseQuotes)) break;
				if (quotechar == '\0') quotechar = c;
				else if (quotechar == c)
				{
					quotechar = '\0';
					hadquotes = true;
					done = true;
				}
				else tempArg_[arglen++] = c;
				break;
			// Curly brackets - treat in the same way as quotes
			case ('{'):
			case ('}'):
				// If explicitly not useing braces, add as normal character
				if (!(optionMask&LineParser::UseBraces)) tempArg_[arglen++] = c;
				else
				{
					// If the quotechar is a left brace and we have a right brace, stop quoting
					if ((quotechar == '{') && (c == '}'))
					{
						quotechar = '\0';
						break;
					}
					// If we are already quoting by some other means, add character and exit
					if (quotechar != '\0') tempArg_[arglen++] = c;
					// No previous quoting, so begin quoting if '{'
					if (c == '{') quotechar = '{';
				}
				break;
			// Parentheses
			case ('('):	// Left parenthesis
			case (')'):	// Right parenthesis
				if (optionMask&LineParser::StripBrackets) break;
				tempArg_[arglen++] = c;
				break;
			// Comment markers
			case ('#'):	// "#" Rest/all of line is a comment
				endOfLine_ = true;
				done = true;
				break;
			// Normal character
			default: 
				tempArg_[arglen++] = c;
				break;
		}
		// Increment line position
		++linePos_;
		if (done || failed) break;
	}
	// Finalise argument
	tempArg_[arglen] = '\0';
	if (linePos_ == lineLength_) endOfLine_ = true;
	// Store the result in the desired destination
	if (destarg != NULL) *destarg = tempArg_;
	if (failed) return false;
	return (arglen == 0 ? (hadquotes ? true : false) : true);
}

// Rip next n characters
bool LineParser::getNextN(int optionMask, int length, CharString* destarg)
{
	// Put the next 'length' characters from line_ into temparg (and put into supplied arg if supplied)
	// A negative length may be supplied, which we interpret as 'strip trailing spaces'
	int arglen = 0;
	char c;
	if (lineLength_ == 0) return false;

	int n, charsleft = lineLength_ - linePos_;
	bool striptrailing = (length < 0);
	length = abs(length);
	if (length > charsleft) length = charsleft;
	//if (length > lineLength_) length = lineLength_;
	for (n=0; n<length; n++)
	{
		c = line_[linePos_];
		switch (c)
		{
			// Brackets
			case ('('):	// Left parenthesis
			case (')'):	// Right parenthesis
				if (optionMask&LineParser::StripBrackets) break;
				tempArg_[arglen] = c;
				arglen ++;
				break;
			default:
				tempArg_[arglen] = c;
				arglen ++;
				break;
		}
		linePos_ ++;
	}
	// Add terminating character to temparg
	tempArg_[arglen] = '\0';
	if (striptrailing) for (n = arglen-1; (tempArg_[n] == ' ') || (tempArg_[n] == '\t'); --n) tempArg_[n] = '\0'; 
	if (destarg != NULL) destarg->set(tempArg_);
	//printf("getNextN found [%s], length = %i\n", tempArg_, arglen);
	//line_.eraseStart(length);
	return true;
}

// Get all arguments (delimited) from LineParser::line_
void LineParser::getAllArgsDelim(int optionMask)
{
	// Parse the string in 'line_' into arguments in 'args'
	arguments_.clear();
	endOfLine_ = false;
	CharString* arg;
	while (!endOfLine_)
	{
		// Create new, empty CharString
		arg = new CharString;
		// We must pass on the current optionMask, else it will be reset by the default value in getNextArg()
		if (getNextArg(optionMask, arg))
		{
			// printf("getAllArgsDelim arg=%i [%s]\n", arguments_.nItems(), arg->get());
			// Add this char to the list
			arguments_.own(arg);
		}
		else delete arg;
	}
}

// Parse delimited (from file)
int LineParser::getArgsDelim(int optionMask)
{
	bool done = false;
	int result;
	// Returns : 0=ok, 1=error, -1=eof
	do
	{
		// Read line from file and parse it
		result = readNextLine(optionMask);
		if (result != 0) return result;
		
		// Assume that we will finish after parsing the line we just read in
		done = true;
		// To check for blank lines, do the parsing and then check nargs()
		getAllArgsDelim(optionMask);
		if ((optionMask&LineParser::SkipBlanks) && (nArgs() == 0)) done = false;
	} while (!done);
	return 0;
}

// Get rest of current line starting at next delimited part (and put into destination argument if supplied)
bool LineParser::getRestDelim(CharString* destarg)
{
	int arglen = 0, n, length;
	char c;
	if (lineLength_ == 0) return false;
	length = lineLength_ - linePos_;
	for (n=0; n<length; n++)
	{
		c = line_[linePos_];
		switch (c)
		{
			// Ignore whitespace occuring before first proper character
			case (' '):
			case ('\0'):
				if (arglen != 0) tempArg_[arglen++] = c;
				break;
			default:
				tempArg_[arglen++] = c;
				break;
		}
		linePos_ ++;
	}
	// Add terminating character to temparg - strip whitespace at end if there is any...
	tempArg_[arglen] = '\0';
	for (n=arglen-1; n>0; --n)
	{
		if ((tempArg_[n] != ' ') && (tempArg_[n] != '\t')) break;
		tempArg_[n] = '\0';
	}
	if (destarg != NULL) destarg->set(tempArg_);
	return true;
}

// Get next argument (delimited) from file stream
bool LineParser::getArgDelim(int optionMask, CharString* destarg)
{
	bool result = getNextArg(optionMask, destarg);
	printf("getArgDelim = %s [%s]\n", result ? "true" : "false", destarg->get());
	return result;
}

// Parse all arguments (delimited) from string
void LineParser::getArgsDelim(int optionMask, const char* s)
{
	strcpy(line_,s);
	lineLength_ = strlen(line_);
	linePos_ = 0;
	getAllArgsDelim(optionMask);
}

// Get next delimited chunk from input stream (not line)
bool LineParser::getCharsDelim(CharString* destarg)
{
	int length = 0;
	bool result = true;
	char c;
	while (!inputFile_->eof())
	{
		inputFile_->get(c);
		if ((c == '\n') || (c == '\t') || (c == '\r') || (c == ' '))
		{
			// Eat DOS-style line terminator
			if ((c == '\r') && (inputFile_->peek() == '\n')) inputFile_->get(c);
			if (length != 0) break;
			else continue;
		}
		if (c == '\0')
		{
			if (length == 0) result = false;
			break;
		}
		tempArg_[length] = c;
		++length;
	}
	tempArg_[length] = '\0';
	if (destarg != NULL) destarg->set(tempArg_);
	return result;
}

// Get next delimited chunk from string, removing grabbed part
bool LineParser::getCharsDelim(int optionMask, CharString* source, CharString* destarg)
{
	// Get the next input chunk from the internal string and put into argument specified.
	int arglen, pos = 0, length = source->length();
	bool done, hadquotes, failed;
	char c, quotechar;
	failed = false;
	done = false;
	hadquotes = false;
	quotechar = '\0';
	arglen = 0;
	while (pos < length)
	{
		c = (*source)[pos];
		switch (c)
		{
			// End of line markers
			case (10):	// Line feed (\n)
			case (13):	// Carriage Return
				done = true;
				break;
			// Delimiters
			// If we encounter one and arg length != 0 this signals the end of the argument.
			case (9):	// Horizontal Tab
			case (' '):	// Space
			case (','):	// Comma
				if (quotechar != '\0')
				{
					tempArg_[arglen] = c;
					arglen ++;
				}
				else if (arglen != 0) done = true;
				break;
			// Quote marks
			// If LineParser::UseQuotes, keep delimiters and other quote marks inside the quoted text.
			case (34):	// Double quotes
			case (39):	// Single quotes
				if (!(optionMask&LineParser::UseQuotes)) break;
				if (quotechar == '\0') quotechar = c;
				else if (quotechar == c)
				{
					quotechar = '\0';
					hadquotes = true;
					done = true;
				}
				else
				{
					tempArg_[arglen] = c;
					arglen ++;
				}
				break;
			// Curly brackets - treat in the same way as quotes
			case ('{'):
			case ('}'):
				if (!(optionMask&LineParser::UseBraces))
				{
					// Just add as normal character
					tempArg_[arglen] = c;
					arglen ++;
				}
				else
				{
					// If the quotechar is a left brace and we have a right brace, stop quoting
					if ((quotechar == '{') && (c == '}'))
					{
						quotechar = '\0';
						break;
					}
					// If we are already quoting by some other means, add character and exit
					if (quotechar != '\0')
					{
						tempArg_[arglen] = c;
						arglen ++;
					}
					// No previous quoting, so begin quoting if '{'
					if (c == '{') quotechar = '{';
				}
				break;
			// Parentheses
			case ('('):	// Left parenthesis
			case (')'):	// Right parenthesis
				if (optionMask&LineParser::StripBrackets) break;
				tempArg_[arglen] = c;
				arglen ++;
				break;
			// Comment markers
			case ('#'):	// "#" Rest/all of line is a comment
				endOfLine_ = true;
				done = true;
				break;
			// Normal character
			default: 
				tempArg_[arglen] = c;
				arglen ++;
				break;
		}
		// Increment line position
		pos ++;
		if (done || failed) break;
	}
	// Finalise argument
	tempArg_[arglen] = '\0';
	// Store the result in the desired destination
	if (destarg != NULL) *destarg = tempArg_;
	// Trim characters from source string
	source->eraseStart(pos);
	if (failed) return false;
	return (arglen == 0 ? (hadquotes ? true : false) : true);
}

// Return a number of characters from the input stream
const char* LineParser::getChars(int nchars, bool skipeol)
{
	char c;
	// Check number of characters requested
	int i=0;
	if (nchars == 0) return NULL;
	else if (nchars > MAXLINELENGTH)
	{
		Messenger::error("The maximum number of characters read at once from a file is currently %i.\n", MAXLINELENGTH);
		return NULL;
	}
	else if (nchars < 0)
	{
		tempArg_[0] = '\0';
		for (int i=nchars; i<0; i++) inputFile_->unget();
	}
	else for (i=0; i < nchars; ++i)
	{
		inputFile_->get(c);
		if (skipeol) while ((c == '\n') || (c == '\r')) { if (inputFile_->eof()) break; inputFile_->get(c); }
		tempArg_[i] = c;
		if (inputFile_->eof()) break;
	}
	tempArg_[i] = '\0';
	if (inputFile_->eof())
	{
	//	closeFile();
		return NULL;
	}
	if (inputFile_->fail())
	{
	//	closeFile();
		return NULL;
	}
	return tempArg_;
}

// Write line to file
bool LineParser::writeLine(const char* s) const
{
	if (!directOutput_)
	{
		if (cachedFile_ == NULL)
		{
			Messenger::print("Unable to delayed-writeLine - destination cache is not open.\n");
			return false;
		}
		else* cachedFile_ << s;
	}
	else if (outputFile_ == NULL)
	{
		Messenger::print("Unable to direct-writeLine - destination file is not open.\n");
		return false;
	}
	else* outputFile_ << s;
	return true;
}

// Write formatted line to file
bool LineParser::writeLineF(const char* fmt, ...) const
{
	if (!directOutput_)
	{
		if (cachedFile_ == NULL)
		{
			Messenger::print("Unable to delayed-writeLineF - destination cache is not open.\n");
			return false;
		}
	}
	else if (outputFile_ == NULL)
	{
		Messenger::print("Unable to direct-writeLineF - destination file is not open.\n");
		return false;
	}
	
	// Construct line
	va_list arguments;
	static char s[8096];
	s[0] = '\0';

	// Parse the argument list (...) and internally write the output string into s[]
	va_start(arguments,fmt);
	vsprintf(s,fmt,arguments);
	va_end(arguments);
	if (directOutput_) *outputFile_ << s;
	else* cachedFile_ << s;
	return true;
}

// Commit cached output stream to actual output file
bool LineParser::commitCache()
{
	// Were we using cached writing?
	if (directOutput_)
	{
		printf("Internal Error: Tried to commit cached writes when direct output was enabled.\n");
		return false;
	}
	ofstream outputFile(outputFilename_);
	if (outputFile.is_open())
	{
		outputFile << cachedFile_->str();
		outputFile.close();
	}
	else
	{
		Messenger::error("Couldn't open output file '%s' for writing.\n", outputFilename_.get());
		return false;
	}
	return true;
}

// Skip lines from file
int LineParser::skipLines(int nlines)
{
	int result;
	for (int n=0; n<nlines; n++)
	{
		result = readNextLine(0);
		if (result != 0) return result;
	}
	return 0;
}

/*
 * Parallel Read/Write
 */

// Return whether the end of the input stream has been reached (or only whitespace remains)
bool LineParser::eofOrBlank(ProcessPool& procPool) const
{
	// Run command on master and broadcast result
	bool result;
	if (procPool.isMaster()) result = eofOrBlank();
	if (!procPool.broadcast(result)) return false;

	return result;
}

// Read line from file and do delimited parse
int LineParser::getArgsDelim(ProcessPool& procPool, int optionMask)
{
	// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
	int result;
	if (procPool.isMaster()) result = getArgsDelim(optionMask);
	if (!procPool.broadcast(&result, 1)) return -1;

	// Everybody now has the result of the read/parse, so transfer line
	if (!procPool.broadcast(line_)) return false;
	if (procPool.isSlave())
	{
		lineLength_ = strlen(line_);
		linePos_ = 0;

		// Parse the line before returning the result of the initial line read
		getAllArgsDelim(optionMask);
	}

	return result;
}

/*
 * Argument Data
 */

// Returns number of arguments grabbed from last parse
int LineParser::nArgs() const
{
	return arguments_.nItems();
}

// Returns the specified argument as a character string
const char* LineParser::argc(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning \"NULL\"...\n", i);
		return "NULL";
	}
	return arguments_[i]->get();
}

// Returns the specified argument as an integer
int LineParser::argi(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning 0...\n", i);
		return 0;
	}
	return arguments_[i]->asInteger();
}

// Returns the specified argument as a double
double LineParser::argd(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning 0.0...\n", i);
		return 0.0;
	}
	return arguments_[i]->asDouble();
}

// Returns the specified argument as a bool
bool LineParser::argb(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning false...\n", i);
		return false;
	}
	return arguments_[i]->asBool();
}

// Returns the specified argument as a float
float LineParser::argf(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning 0.0f...\n", i);
		return 0.0f;
	}
	return (float) argd(i);
}

// Returns whether the specified argument exists (has been provided)
bool LineParser::hasArg(int i) const
{
	if ((i < 0) || (i >= nArgs())) return false;
	return true;
}
