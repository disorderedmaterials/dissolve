/*
    *** Line Parsing Routines
    *** src/base/lineparser.cpp
    Copyright T. Youngs 2012-2020

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

#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"
#include <stdarg.h>
#include <string.h>

LineParser::LineParser(ProcessPool *procPool)
{
    arguments_.clear();
    reset();

    processPool_ = procPool;
}

LineParser::~LineParser()
{
    closeFiles();

    if (inputFile_ != NULL)
        delete inputFile_;
    if (outputFile_ != NULL)
        delete outputFile_;
    if (cachedFile_ != NULL)
        delete cachedFile_;
    if (inputStrings_ != NULL)
        delete inputStrings_;
}

// Return current stream for input
std::istream *LineParser::inputStream() const
{
    if (fileInput_)
        return inputFile_;
    return inputStrings_;
}

/*
 * Source line/file and read options
 */

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
    inputStrings_ = NULL;
    fileInput_ = true;
    directOutput_ = false;
    arguments_.clear();
}

// Return associated process pool (if any)
ProcessPool *LineParser::processPool() const { return processPool_; }

// Return filename of current input file (if any)
const char *LineParser::inputFilename() const { return inputFilename_.get(); }

// Return filename of current output file (if any)
const char *LineParser::outputFilename() const { return outputFilename_.get(); }

// Return pointer to current line
const char *LineParser::line() const { return line_; }

// Set line target
void LineParser::setLine(const char *s)
{
    strncpy(line_, s, MAXLINELENGTH);
    lineLength_ = strlen(line_);
    linePos_ = 0;
}

// Return integer line number of last read line
int LineParser::lastLineNo() const { return lastLineNo_; }

// Open new file for reading
bool LineParser::openInput(const char *filename)
{
    // Master needs to check for an existing input file
    if ((!processPool_) || processPool_->isMaster())
    {
        if (inputFile_ != NULL)
        {
            printf("Warning - LineParser already appears to have an open file...\n");
            inputFile_->close();
            delete inputFile_;
            inputFile_ = NULL;
        }
    }

    fileInput_ = true;

    // Master will open the file
    auto result = true;
    if ((!processPool_) || processPool_->isMaster())
    {
        inputFile_ = new std::ifstream(filename, std::ios::in | std::ios::binary);
        if (!inputFile_->is_open())
        {
            closeFiles();
            Messenger::warn("Failed to open file '%s' for reading.\n", filename);
            result = false;
        }
    }

    // Broadcast result of open
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    lastLineNo_ = 0;
    inputFilename_ = filename;

    return result;
}

// Open input string for reading
bool LineParser::openInputString(const char *string)
{
    if (inputFile_ != NULL)
    {
        printf("Warning - LineParser already appears to have an open file...\n");
        inputFile_->close();
        delete inputFile_;
        inputFile_ = NULL;
    }
    if (inputStrings_ != NULL)
    {
        printf("Warning - LineParser already appears to have open strings...\n");
        delete inputStrings_;
        inputStrings_ = NULL;
    }

    fileInput_ = false;

    // Create a new stringstream and copy the input string to it
    inputStrings_ = new std::stringstream;
    (*inputStrings_) << string;
    inputStrings_->seekg(std::ios_base::beg);

    lastLineNo_ = 0;

    return true;
}

// Open new stream for writing
bool LineParser::openOutput(const char *filename, bool directOutput)
{
    auto result = true;

    // Master handles the opening of the output file
    if ((!processPool_) || processPool_->isMaster())
    {
        // Check for existing output file
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
            outputFile_ = new std::ofstream(filename, std::ios::out);
            if (!outputFile_->is_open())
            {
                closeFiles();
                Messenger::error("Failed to open file '%s' for writing.\n", filename);
                result = false;
            }
        }
        else
            cachedFile_ = new std::stringstream;
    }

    // Broadcast result of open
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    outputFilename_ = filename;

    return result;
}

// Open existing stream for writing
bool LineParser::appendOutput(const char *filename)
{
    auto result = true;

    // Master handles the opening of the output file
    if ((!processPool_) || processPool_->isMaster())
    {
        // Check for existing output file
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

        // Open file for appending
        directOutput_ = true;
        outputFile_ = new std::ofstream(filename, std::ios::app);
        if (!outputFile_->is_open())
        {
            closeFiles();
            Messenger::error("Failed to open file '%s' for writing.\n", filename);
            result = false;
        }
    }

    // Broadcast result of open
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    outputFilename_ = filename;

    return result;
}

// Close file
void LineParser::closeFiles()
{
    if ((!processPool_) || processPool_->isMaster())
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
    }

    if (inputStrings_ != NULL)
        delete inputStrings_;

    reset();
}

// Return whether current file source is good for reading
bool LineParser::isFileGoodForReading() const
{
    // Master performs the checks
    auto result = true;
    if ((!processPool_) || processPool_->isMaster())
    {
        if (fileInput_ && (inputFile_ == NULL))
            result = false;
        else if (fileInput_ && (!inputFile_->is_open()))
            result = false;
        else if ((!fileInput_) && (!inputStrings_))
            result = false;
    }

    // Broadcast result of open
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Return whether current file source is good for writing
bool LineParser::isFileGoodForWriting() const
{
    // Master performs the checks
    auto result = true;
    if ((!processPool_) || processPool_->isMaster())
    {
        if (directOutput_)
        {
            if (outputFile_ == NULL)
                result = false;
            else if (!outputFile_->is_open())
                result = false;
        }
    }

    // Broadcast result of open
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Peek next character in input stream
char LineParser::peek() const
{
    if (inputStream() == NULL)
        return '\0';
    return inputStream()->peek();
}

// Tell current position of input stream
std::streampos LineParser::tellg() const
{
    std::streampos result = 0;
    if (inputStream() != NULL)
        result = inputStream()->tellg();
    else
        printf("Warning: LineParser tried to tellg() on a non-existent input file.\n");
    return result;
}

// Seek position in input stream
void LineParser::seekg(std::streampos pos)
{
    if (inputFile_ != NULL)
    {
        if (inputStream()->eof())
            inputStream()->clear();
        inputStream()->seekg(pos);
    }
    else
        printf("Warning: LineParser tried to seekg() on a non-existent input file.\n");
}

// Seek n bytes in specified direction in input stream
void LineParser::seekg(std::streamoff off, std::ios_base::seekdir dir)
{
    if (inputStream() != NULL)
        inputFile_->seekg(off, dir);
    else
        printf("Warning: LineParser tried to seekg() on a non-existent input file.\n");
}

// Rewind input stream to start
void LineParser::rewind()
{
    if (inputStream() != NULL)
        inputFile_->seekg(0, std::ios::beg);
    else
        Messenger::print("No file currently open to rewind.\n");
}

// Return whether the end of the input stream has been reached (or only whitespace remains)
bool LineParser::eofOrBlank() const
{
    // If no process pool is defined, or we are the master, do the check
    auto result = false;
    if ((!processPool_) || processPool_->isMaster())
    {
        // Do we have a valid input stream?
        if (inputStream() == NULL)
        {
            result = true;
            if (processPool_ && (!processPool_->broadcast(result)))
                return false;
            return true;
        }

        // Simple check first - is this the end of the file?
        if (inputStream()->eof())
        {
            result = true;
            if (processPool_ && (!processPool_->broadcast(result)))
                return false;
            return true;
        }

        // Otherwise, store the current file position and search for a non-whitespace character (or end of file)
        std::streampos pos = inputStream()->tellg();

        // Skip through whitespace, searching for 'hard' character
        char c;
        result = true;
        do
        {
            inputStream()->get(c);
            if (inputStream()->eof())
                break;
            // If a whitespace character then skip it....
            if ((c == ' ') || (c == '\r') || (c == '\n') || (c == '\t') || (c == '\0'))
            {
                if (inputStream()->eof())
                    break;
                else
                    continue;
            }
            result = false;
            break;
        } while (1);
        inputStream()->seekg(pos);
    }

    // Broadcast result to pool if it is defined
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

/*
 * Read/Write Routines
 */

// Read single line from internal file source
LineParser::ParseReturnValue LineParser::readNextLine(int optionMask)
{
    // Master will check the file and broadcast the result
    LineParser::ParseReturnValue result = LineParser::Success;
    if ((!processPool_) || processPool_->isMaster())
    {
        // Returns : 0=ok, 1=error, -1=eof
        if (fileInput_ && (inputFile_ == NULL))
        {
            Messenger::error("No input file open for LineParser::readNextLine.\n");
            result = LineParser::Fail;
        }
        else if (inputStream()->eof())
            result = LineParser::EndOfFile;
    }

    // Broadcast result of file check
    if (processPool_)
    {
        if (!processPool_->broadcast(EnumCast<LineParser::ParseReturnValue>(result)))
            return LineParser::Fail;
        if (result != LineParser::Success)
            return result;
    }

    // Master (if appropriate) will read the line and broadcast the result of the read
    if ((!processPool_) || processPool_->isMaster())
    {
        // Loop until we get 'suitable' line from file
        int nchars, nspaces;
        result = LineParser::Fail;
        while (result != LineParser::Success)
        {
            char chr;
            lineLength_ = 0;
            result = LineParser::Fail;
            while (inputStream()->get(chr).good())
            {
                if (chr == '\r')
                {
                    if (inputStream()->peek() == '\n')
                        inputStream()->ignore();
                    break;
                }
                else if (chr == '\n')
                    break;
                else if ((chr == ';') && (optionMask & LineParser::SemiColonLineBreaks))
                    break;

                line_[lineLength_++] = chr;

                // Check here for overfilling the line_ buffer - perhaps it's a binary file?
                if (lineLength_ == MAXLINELENGTH)
                    result = LineParser::Fail;
            }
            line_[lineLength_] = '\0';
            ++lastLineNo_;
            Messenger::printVerbose("Line from file is: [%s]\n", line_);

            // Remove comments from line
            if (!(optionMask & LineParser::KeepComments))
                DissolveSys::removeComments(line_);

            // If we are skipping blank lines, check for a blank line here
            if (!(optionMask & LineParser::KeepBlanks))
            {
                // Now, see if our line contains only blanks
                nchars = 0;
                nspaces = 0;
                for (auto *c = line_; *c != '\0'; c++)
                {
                    nchars++;
                    if (isspace(*c))
                        nspaces++;
                }

                if (nchars == nspaces)
                {
                    // Blank line - if we're at the end of the file, return EOF.
                    // Otherwise, read in another line.
                    if (inputStream()->eof())
                        result = LineParser::EndOfFile;
                    else if (inputStream()->fail())
                        result = LineParser::Fail;
                    else
                        continue;
                }
                else
                    result = LineParser::Success;
            }
            else
                result = LineParser::Success;

            lineLength_ = strlen(line_);
            linePos_ = 0;

            // Exit on EOF or error
            if (result != LineParser::Success)
                break;
        };
        // 		printf("LineParser Returned line = [%s], length = %i\n",line_,lineLength_);
    }

    // Broadcast result
    if (processPool_)
    {
        if (!processPool_->broadcast(EnumCast<LineParser::ParseReturnValue>(result)))
            return LineParser::Fail;
        if (result != LineParser::Success)
            return result;
    }

    // Broadcast line
    if (processPool_)
    {
        if (!processPool_->broadcast(line_))
            return LineParser::Fail;

        if (processPool_->isSlave())
        {
            lineLength_ = strlen(line_);
            linePos_ = 0;
        }
    }

    return LineParser::Success;
}

// Read next line from internal source file, setting as parsing source and storing in specified CharString
LineParser::ParseReturnValue LineParser::readNextLine(int optionMask, CharString &dest)
{
    LineParser::ParseReturnValue result = readNextLine(optionMask);
    if (result != LineParser::Success)
        return result;
    dest = line_;
    return result;
}

// Gets next delimited arg from internal line
bool LineParser::getNextArg(int optionMask, CharString *destarg)
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
            case (10): // Line feed (\n)
            case (13): // Carriage Return
                done = true;
                endOfLine_ = true;
                break;
            // Delimiters
            // If we encounter one and arg length != 0 this signals the end of the argument.
            case (','): // Comma
                if (!(optionMask & LineParser::CommasAreDelimiters))
                {
                    tempArg_[arglen++] = c;
                    break;
                }
            case (9):   // Horizontal Tab
            case (' '): // Space
                if (quotechar != '\0')
                    tempArg_[arglen++] = c;
                else if (arglen != 0)
                    done = true;
                break;
            // Quote marks
            // If LineParser::IgnoreQuotes, don't keep delimiters and other quote marks inside the quoted text.
            case (34): // Double quotes
            case (39): // Single quotes
                if (optionMask & LineParser::IgnoreQuotes)
                    break;
                if (quotechar == '\0')
                    quotechar = c;
                else if (quotechar == c)
                {
                    quotechar = '\0';
                    hadquotes = true;
                    done = true;
                }
                else
                    tempArg_[arglen++] = c;
                break;
            // Curly brackets - treat in the same way as quotes
            case ('{'):
            case ('}'):
                // If explicitly not useing braces, add as normal character
                if (!(optionMask & LineParser::UseBraces))
                    tempArg_[arglen++] = c;
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
                        tempArg_[arglen++] = c;
                    // No previous quoting, so begin quoting if '{'
                    if (c == '{')
                        quotechar = '{';
                }
                break;
            // Parentheses
            case ('('): // Left parenthesis
            case (')'): // Right parenthesis
                if (optionMask & LineParser::StripBrackets)
                    break;
                tempArg_[arglen++] = c;
                break;
            // Comment markers
            case ('#'): // "#" Rest/all of line is a comment
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
        if (done || failed)
            break;
    }

    // Finalise argument
    tempArg_[arglen] = '\0';
    if (linePos_ == lineLength_)
        endOfLine_ = true;

    // Store the result in the desired destination
    if (destarg != NULL)
        *destarg = tempArg_;
    if (failed)
        return false;
    return (arglen == 0 ? (hadquotes ? true : false) : true);
}

// Rip next n characters
bool LineParser::getNextN(int optionMask, int length, CharString *destarg)
{
    // Put the next 'length' characters from line_ into temparg (and put into supplied arg if supplied)
    // A negative length may be supplied, which we interpret as 'strip trailing spaces'
    auto arglen = 0;
    char c;
    if (lineLength_ == 0)
        return false;

    int n, charsleft = lineLength_ - linePos_;
    auto striptrailing = (length < 0);
    length = abs(length);
    if (length > charsleft)
        length = charsleft;
    // if (length > lineLength_) length = lineLength_;
    for (n = 0; n < length; n++)
    {
        c = line_[linePos_];
        switch (c)
        {
            // Brackets
            case ('('): // Left parenthesis
            case (')'): // Right parenthesis
                if (optionMask & LineParser::StripBrackets)
                    break;
                tempArg_[arglen] = c;
                arglen++;
                break;
            default:
                tempArg_[arglen] = c;
                arglen++;
                break;
        }
        linePos_++;
    }

    // Add terminating character to temparg
    tempArg_[arglen] = '\0';
    if (striptrailing)
        for (n = arglen - 1; (tempArg_[n] == ' ') || (tempArg_[n] == '\t'); --n)
            tempArg_[n] = '\0';
    if (destarg != NULL)
        destarg->set(tempArg_);
    // printf("getNextN found [%s], length = %i\n", tempArg_, arglen);
    // line_.eraseStart(length);
    return true;
}

// Get all arguments (delimited) from LineParser::line_
void LineParser::getAllArgsDelim(int optionMask)
{
    // Parse the string in 'line_' into arguments in 'args'
    arguments_.clear();
    endOfLine_ = false;
    CharString *arg;
    while (!endOfLine_)
    {
        // Create new, empty CharString
        CharString arg;

        // We must pass on the current optionMask, else it will be reset by the default value in getNextArg()
        if (getNextArg(optionMask, &arg))
        {
            // printf("getAllArgsDelim arg=%i [%s]\n", arguments_.nItems(), arg->get());
            // Add this char to the list
            arguments_.push_back(arg);
        }
    }
}

// Parse delimited (from file)
LineParser::ParseReturnValue LineParser::getArgsDelim(int optionMask)
{
    LineParser::ParseReturnValue result = readNextLine(optionMask);

    // Parse the line before returning the result of the initial line read
    getAllArgsDelim(optionMask);

    return result;
}

// Get rest of current line starting at next delimited part (and put into destination argument if supplied)
bool LineParser::getRestDelim(CharString *destarg)
{
    auto arglen = 0;
    char c;
    if (lineLength_ == 0)
        return false;
    const auto length = lineLength_ - linePos_;
    for (int n = 0; n < length; ++n)
    {
        c = line_[linePos_];
        switch (c)
        {
            // Ignore whitespace occuring before first proper character
            case (' '):
            case ('\0'):
                if (arglen != 0)
                    tempArg_[arglen++] = c;
                break;
            default:
                tempArg_[arglen++] = c;
                break;
        }
        linePos_++;
    }
    // Add terminating character to temparg - strip whitespace at end if there is any...
    tempArg_[arglen] = '\0';
    for (int n = arglen - 1; n > 0; --n)
    {
        if ((tempArg_[n] != ' ') && (tempArg_[n] != '\t'))
            break;
        tempArg_[n] = '\0';
    }
    if (destarg != NULL)
        destarg->set(tempArg_);
    return true;
}

// Get next argument (delimited) from file stream
bool LineParser::getArgDelim(int optionMask, CharString *destarg)
{
    auto result = getNextArg(optionMask, destarg);
    // 	printf("getArgDelim = %s [%s]\n", result ? "true" : "false", destarg->get());
    return result;
}

// Parse all arguments (delimited) from string
void LineParser::getArgsDelim(int optionMask, const char *s)
{
    strcpy(line_, s);
    lineLength_ = strlen(line_);
    linePos_ = 0;
    getAllArgsDelim(optionMask);
}

// Get next delimited chunk from input stream (not line)
bool LineParser::getCharsDelim(CharString *destarg)
{
    auto length = 0;
    auto result = true;
    char c;
    while (!inputStream()->eof())
    {
        inputStream()->get(c);
        if ((c == '\n') || (c == '\t') || (c == '\r') || (c == ' '))
        {
            // Eat DOS-style line terminator
            if ((c == '\r') && (inputStream()->peek() == '\n'))
                inputStream()->get(c);
            if (length != 0)
                break;
            else
                continue;
        }
        if (c == '\0')
        {
            if (length == 0)
                result = false;
            break;
        }
        tempArg_[length] = c;
        ++length;
    }
    tempArg_[length] = '\0';
    if (destarg != NULL)
        destarg->set(tempArg_);
    return result;
}

// Get next delimited chunk from string, removing grabbed part
bool LineParser::getCharsDelim(int optionMask, CharString *source, CharString *destarg)
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
            case (10): // Line feed (\n)
            case (13): // Carriage Return
                done = true;
                break;
            // Delimiters
            // If we encounter one and arg length != 0 this signals the end of the argument.
            case (9):   // Horizontal Tab
            case (' '): // Space
            case (','): // Comma
                if (quotechar != '\0')
                {
                    tempArg_[arglen] = c;
                    arglen++;
                }
                else if (arglen != 0)
                    done = true;
                break;
            // Quote marks
            // If LineParser::IgnoreQuotes, don't keep delimiters and other quote marks inside the quoted text.
            case (34): // Double quotes
            case (39): // Single quotes
                if (optionMask & LineParser::IgnoreQuotes)
                    break;
                if (quotechar == '\0')
                    quotechar = c;
                else if (quotechar == c)
                {
                    quotechar = '\0';
                    hadquotes = true;
                    done = true;
                }
                else
                {
                    tempArg_[arglen] = c;
                    arglen++;
                }
                break;
            // Curly brackets - treat in the same way as quotes
            case ('{'):
            case ('}'):
                if (!(optionMask & LineParser::UseBraces))
                {
                    // Just add as normal character
                    tempArg_[arglen] = c;
                    arglen++;
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
                        arglen++;
                    }
                    // No previous quoting, so begin quoting if '{'
                    if (c == '{')
                        quotechar = '{';
                }
                break;
            // Parentheses
            case ('('): // Left parenthesis
            case (')'): // Right parenthesis
                if (optionMask & LineParser::StripBrackets)
                    break;
                tempArg_[arglen] = c;
                arglen++;
                break;
            // Comment markers
            case ('#'): // "#" Rest/all of line is a comment
                endOfLine_ = true;
                done = true;
                break;
            // Normal character
            default:
                tempArg_[arglen] = c;
                arglen++;
                break;
        }
        // Increment line position
        pos++;
        if (done || failed)
            break;
    }
    // Finalise argument
    tempArg_[arglen] = '\0';
    // Store the result in the desired destination
    if (destarg != NULL)
        (*destarg) = tempArg_;
    // Trim characters from source string
    source->eraseStart(pos);
    if (failed)
        return false;
    return (arglen == 0 ? (hadquotes ? true : false) : true);
}

// Return a number of characters from the input stream
const char *LineParser::getChars(int nchars, bool skipeol)
{
    char c;
    // Check number of characters requested
    auto i = 0;
    if (nchars == 0)
        return NULL;
    else if (nchars > MAXLINELENGTH)
    {
        Messenger::error("The maximum number of characters read at once from a file is currently %i.\n", MAXLINELENGTH);
        return NULL;
    }
    else if (nchars < 0)
    {
        tempArg_[0] = '\0';
        for (int i = nchars; i < 0; i++)
            inputStream()->unget();
    }
    else
        for (i = 0; i < nchars; ++i)
        {
            inputStream()->get(c);
            if (skipeol)
                while ((c == '\n') || (c == '\r'))
                {
                    if (inputStream()->eof())
                        break;
                    inputStream()->get(c);
                }
            tempArg_[i] = c;
            if (inputStream()->eof())
                break;
        }
    tempArg_[i] = '\0';
    if (inputStream()->eof())
    {
        //	closeFile();
        return NULL;
    }
    if (inputStream()->fail())
    {
        //	closeFile();
        return NULL;
    }
    return tempArg_;
}

// Write line to file
bool LineParser::writeLine(const char *s) const
{
    auto result = true;

    // Master handles the writing
    if ((!processPool_) || processPool_->isMaster())
    {
        if (!directOutput_)
        {
            if (cachedFile_ == NULL)
            {
                Messenger::print("Unable to delayed-writeLine - destination cache is not open.\n");
                return false;
            }
            else
                (*cachedFile_) << s;
        }
        else if (outputFile_ == NULL)
        {
            Messenger::print("Unable to direct-writeLine - destination file is not open.\n");
            return false;
        }
        else
            (*outputFile_) << s;
    }

    // Broadcast result of write
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Write formatted line to file
bool LineParser::writeLineF(const char *fmt, ...) const
{
    auto result = true;

    // Master handles the writing
    if ((!processPool_) || processPool_->isMaster())
    {
        if (!directOutput_)
        {
            if (cachedFile_ == NULL)
            {
                Messenger::print("Unable to delayed-writeLineF - destination cache is not open.\n");
                result = false;
                if (processPool_ && (!processPool_->broadcast(result)))
                    return false;
                return false;
            }
        }
        else if (outputFile_ == NULL)
        {
            Messenger::print("Unable to direct-writeLineF - destination file is not open.\n");
            result = false;
            if (processPool_ && (!processPool_->broadcast(result)))
                return false;
            return false;
        }

        // Construct line
        va_list arguments;
        static char s[8096];
        s[0] = '\0';

        // Parse the argument list (...) and internally write the output string into s[]
        va_start(arguments, fmt);
        vsprintf(s, fmt, arguments);
        va_end(arguments);
        if (directOutput_)
            (*outputFile_) << s;
        else
            (*cachedFile_) << s;
    }

    // Broadcast result of write
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Print banner comment of fixed width
bool LineParser::writeBannerComment(const char *fmt, ...)
{
    static CharString bannerChars;
    const auto width = 80;
    if (bannerChars.length() < width)
    {
        bannerChars.createEmpty(width + 1);
        bannerChars.fill('-');
        bannerChars[0] = '#';
        bannerChars[width - 1] = '#';
    }

    // First, create the text using vsprintf
    va_list arguments;
    va_start(arguments, fmt);
    vsprintf(workingText_, fmt, arguments);
    va_end(arguments);
    CharString bannerText = workingText_;

    // Now, get the length of the banner text and create a format for printing it into a line 80 chars wide
    auto leftPad = (width - bannerText.length()) / 2 - 1;
    auto rightPad = width - bannerText.length() - leftPad - 2;
    char bannerFormat[64];
    sprintf(bannerFormat, "%%s\n%%c%%%is%%s%%%is%%c\n%%s", leftPad, rightPad);

    // Finally, print the banner
    if (!writeLineF("\n"))
        return false;
    if (!writeLineF(bannerFormat, bannerChars.get(), '#', " ", bannerText.get(), " ", '#', bannerChars.get()))
        return false;
    ;
    if (!writeLineF("\n"))
        return false;

    return true;
}

// Write long int argument as single line
bool LineParser::writeArg(long int i) const { return writeLineF("%li\n", i); }

// Write long long int argument as single line
bool LineParser::writeArg(long long int i) const { return writeLineF("%lli\n", i); }

// Read long int argument as single line
bool LineParser::readArg(long int &i)
{
    if (readNextLine(LineParser::Defaults) != LineParser::Success)
        return false;

    i = atol(line_);

    return true;
}

// Read long long int argument as single line
bool LineParser::readArg(long long int &i)
{
    if (readNextLine(LineParser::Defaults) != LineParser::Success)
        return false;

    i = atoll(line_);

    return true;
}

// Commit cached output stream to actual output file
bool LineParser::commitCache()
{
    auto result = true;

    // Master handles the writing
    if ((!processPool_) || processPool_->isMaster())
    {
        // Were we using cached writing?
        if (directOutput_)
        {
            printf("Internal Error: Tried to commit cached writes when direct output was enabled.\n");
            result = false;
            if (processPool_ && (!processPool_->broadcast(result)))
                return false;
            return false;
        }

        std::ofstream outputFile(outputFilename_);
        if (outputFile.is_open())
        {
            outputFile << cachedFile_->str();
            outputFile.close();
        }
        else
        {
            Messenger::error("Couldn't open output file '%s' for writing.\n", outputFilename_.get());
            result = false;
        }
    }

    // Broadcast result of write
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Skip lines from file
LineParser::ParseReturnValue LineParser::skipLines(int nlines)
{
    LineParser::ParseReturnValue result;
    for (int n = 0; n < nlines; ++n)
    {
        result = readNextLine(LineParser::KeepBlanks);
        if (result != LineParser::Success)
            return result;
    }

    return LineParser::Success;
}

/*
 * Argument Data
 */

// Returns number of arguments grabbed from last parse
int LineParser::nArgs() const { return arguments_.size(); }

// Returns the specified argument as a character string
const char *LineParser::argc(int i)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range - returning \"NULL\"...\n", i);
        return "NULL";
    }
    return arguments_[i].get();
}

// Returns the specified argument as an integer
int LineParser::argi(int i)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range - returning 0...\n", i);
        return 0;
    }
    return arguments_[i].asInteger();
}

// Returns the specified argument as a long integer
long int LineParser::argli(int i)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range - returning 0...\n", i);
        return 0;
    }
    return arguments_[i].asLongInteger();
}

// Returns the specified argument as a double
double LineParser::argd(int i)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range - returning 0.0...\n", i);
        return 0.0;
    }
    return arguments_[i].asDouble();
}

// Returns the specified argument as a bool
bool LineParser::argb(int i)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range - returning false...\n", i);
        return false;
    }
    return arguments_[i].asBool();
}

// Returns the specified argument as a float
float LineParser::argf(int i)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range - returning 0.0f...\n", i);
        return 0.0f;
    }
    return (float)argd(i);
}

// Return the specified and next two arguments as a Vec3<int>
Vec3<int> LineParser::arg3i(int i)
{
    if ((i < 0) || (i >= (nArgs() - 2)))
    {
        printf("Warning: Argument %i is out of range - returning Vec3<int>()...\n", i);
        return Vec3<int>();
    }
    return Vec3<int>(argi(i), argi(i + 1), argi(i + 2));
}

// Return the specified and next two arguments as a Vec3<double>
Vec3<double> LineParser::arg3d(int i)
{
    if ((i < 0) || (i >= (nArgs() - 2)))
    {
        printf("Warning: Argument %i is out of range - returning Vec3<double>()...\n", i);
        return Vec3<double>();
    }
    return Vec3<double>(argd(i), argd(i + 1), argd(i + 2));
}

// Returns whether the specified argument exists (has been provided)
bool LineParser::hasArg(int i) const
{
    if ((i < 0) || (i >= nArgs()))
        return false;
    return true;
}

// Return specified argument as int, placing in variable provided
bool LineParser::argAs(int i, int &destination)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range.\n", i);
        return false;
    }

    destination = argi(i);

    return true;
}

// Return specified argument as double, placing in variable provided
bool LineParser::argAs(int i, double &destination)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range.\n", i);
        return false;
    }

    destination = argd(i);

    return true;
}

// Return specified argument as bool, placing in variable provided
bool LineParser::argAs(int i, bool &destination)
{
    if ((i < 0) || (i >= nArgs()))
    {
        printf("Warning: Argument %i is out of range.\n", i);
        return false;
    }

    destination = argb(i);

    return true;
}

// Return specified argument as Vec3<double>, placing in variable provided
bool LineParser::argAs(int i, Vec3<int> &destination)
{
    if ((i < 0) || (i >= (nArgs() - 2)))
    {
        printf("Warning: Argument %i is out of range.\n", i);
        return false;
    }

    destination.set(argi(i), argi(i), argi(i));

    return true;
}

// Return specified argument as Vec3<double>, placing in variable provided
bool LineParser::argAs(int i, Vec3<double> &destination)
{
    if ((i < 0) || (i >= (nArgs() - 2)))
    {
        printf("Warning: Argument %i is out of range.\n", i);
        return false;
    }

    destination.set(argd(i), argd(i), argd(i));

    return true;
}
