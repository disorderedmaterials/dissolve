// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include <limits>

LineParser::LineParser(const ProcessPool *procPool) : processPool_(procPool)
{
    arguments_.clear();
    reset();
}

LineParser::~LineParser()
{
    closeFiles();

    if (inputFile_ != nullptr)
        delete inputFile_;
    if (outputFile_ != nullptr)
        delete outputFile_;
    if (cachedFile_ != nullptr)
        delete cachedFile_;
    if (inputStrings_ != nullptr)
        delete inputStrings_;
}

/*
 * Source / Destination Streams
 */

// Reset data
void LineParser::reset()
{
    inputFilename_.clear();
    outputFilename_.clear();
    endOfLine_ = false;
    line_.clear();
    linePos_ = 0;
    lastLineNo_ = 0;
    inputFile_ = nullptr;
    outputFile_ = nullptr;
    cachedFile_ = nullptr;
    inputStrings_ = nullptr;
    fileInput_ = true;
    directOutput_ = false;
    arguments_.clear();
}

// Return current stream for input
std::istream *LineParser::inputStream() const
{
    if (fileInput_)
        return inputFile_;
    return inputStrings_;
}

// Return associated process pool (if any)
const ProcessPool *LineParser::processPool() const { return processPool_; }

// Return filename of current input file (if any)
std::string_view LineParser::inputFilename() const { return inputFilename_; }

// Return filename of current output file (if any)
std::string_view LineParser::outputFilename() const { return outputFilename_; }

// Return pointer to current line
std::string_view LineParser::line() const { return line_; }

// Return integer line number of last read line
int LineParser::lastLineNo() const { return lastLineNo_; }

// Open new file for reading
bool LineParser::openInput(std::string_view filename)
{
    // Master needs to check for an existing input file
    if ((!processPool_) || processPool_->isMaster())
    {
        if (inputFile_ != nullptr)
        {
            Messenger::warn("LineParser already appears to have an open file...\n");
            inputFile_->close();
            delete inputFile_;
            inputFile_ = nullptr;
        }
    }

    fileInput_ = true;

    // Master will open the file
    auto result = true;
    if ((!processPool_) || processPool_->isMaster())
    {
        inputFile_ = new std::ifstream(std::string(filename), std::ios::in | std::ios::binary);
        if (!inputFile_->is_open())
        {
            closeFiles();
            Messenger::warn("Failed to open file '{}' for reading.\n", filename);
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
bool LineParser::openInputString(std::string_view s)
{
    if (inputFile_ != nullptr)
    {
        Messenger::warn("LineParser already appears to have an open file...\n");
        inputFile_->close();
        delete inputFile_;
        inputFile_ = nullptr;
    }
    if (inputStrings_ != nullptr)
    {
        Messenger::warn("LineParser already appears to have open strings...\n");
        delete inputStrings_;
        inputStrings_ = nullptr;
    }

    fileInput_ = false;

    // Create a new stringstream and copy the input string to it
    inputStrings_ = new std::stringstream;
    (*inputStrings_) << s;
    inputStrings_->seekg(std::ios_base::beg);

    lastLineNo_ = 0;

    return true;
}

// Open new stream for writing
bool LineParser::openOutput(std::string_view filename, bool directOutput)
{
    auto result = true;

    outputFilename_ = filename;

    // Master handles the opening of the output file
    if ((!processPool_) || processPool_->isMaster())
    {
        // Check for existing output file
        if ((outputFile_ != nullptr) || (cachedFile_ != nullptr))
        {
            Messenger::warn("LineParser already appears to have an open file/cache...\n");
            if (outputFile_ != nullptr)
            {
                outputFile_->close();
                delete outputFile_;
                outputFile_ = nullptr;
            }
            if (cachedFile_ != nullptr)
            {
                delete cachedFile_;
                cachedFile_ = nullptr;
            }
        }

        // Open new file
        directOutput_ = directOutput;
        if (directOutput_)
        {
            outputFile_ = new std::ofstream(outputFilename_, std::ios::out);
            if (!outputFile_->is_open())
            {
                closeFiles();
                Messenger::error("Failed to open file '{}' for writing.\n", outputFilename_);
                result = false;
            }
        }
        else
            cachedFile_ = new std::stringstream;
    }

    // Broadcast result of open
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Open existing stream for writing
bool LineParser::appendOutput(std::string_view filename)
{
    auto result = true;

    outputFilename_ = filename;

    // Master handles the opening of the output file
    if ((!processPool_) || processPool_->isMaster())
    {
        // Check for existing output file
        if ((outputFile_ != nullptr) || (cachedFile_ != nullptr))
        {
            Messenger::warn("LineParser already appears to have an open file/cache...\n");
            if (outputFile_ != nullptr)
            {
                outputFile_->close();
                delete outputFile_;
                outputFile_ = nullptr;
            }
            if (cachedFile_ != nullptr)
            {
                delete cachedFile_;
                cachedFile_ = nullptr;
            }
        }

        // Open file for appending
        directOutput_ = true;
        outputFile_ = new std::ofstream(outputFilename_, std::ios::app);
        if (!outputFile_->is_open())
        {
            closeFiles();
            Messenger::error("Failed to open file '{}' for writing.\n", outputFilename_);
            result = false;
        }
    }

    // Broadcast result of open
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Close file
void LineParser::closeFiles()
{
    if ((!processPool_) || processPool_->isMaster())
    {
        if (inputFile_ != nullptr)
        {
            inputFile_->close();
            delete inputFile_;
        }
        if (outputFile_ != nullptr)
        {
            outputFile_->close();
            delete outputFile_;
        }
    }

    if (inputStrings_ != nullptr)
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
        if (fileInput_ && (inputFile_ == nullptr))
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
            if (outputFile_ == nullptr)
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
    if (inputStream() == nullptr)
        return '\0';
    return inputStream()->peek();
}

// Tell current position of input stream
std::streampos LineParser::tellg() const
{
    std::streampos result = 0;
    if (inputStream() != nullptr)
        result = inputStream()->tellg();
    else
        Messenger::warn("LineParser tried to tellg() on a non-existent input file.\n");
    return result;
}

// Seek position in input stream
void LineParser::seekg(std::streampos pos)
{
    if (inputFile_ != nullptr)
    {
        if (inputStream()->eof())
            inputStream()->clear();
        inputStream()->seekg(pos);
    }
    else
        Messenger::warn("LineParser tried to seekg() on a non-existent input file.\n");
}

// Seek n bytes in specified direction in input stream
void LineParser::seekg(std::streamoff off, std::ios_base::seekdir dir)
{
    if (inputStream() != nullptr)
        inputFile_->seekg(off, dir);
    else
        Messenger::warn("LineParser tried to seekg() on a non-existent input file.\n");
}

// Rewind input stream to start
void LineParser::rewind()
{
    if (inputStream() != nullptr)
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
        if (inputStream() == nullptr)
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
        } while (true);
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

// Gets next delimited arg from internal line
bool LineParser::getNextArg(int optionMask, std::string &arg)
{
    // Get the next input chunk from the internal string and put into argument specified.
    auto done = false, hadquotes = false, failed = false, quoting = false;
    char c, quotechar = '\0';
    endOfLine_ = false;

    while (linePos_ < line_.length())
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
                    arg += c;
                    break;
                }
            case (9):   // Horizontal Tab
            case (' '): // Space
                if (quotechar != '\0')
                    arg += c;
                else if (arg.length() != 0)
                    done = true;
                break;
            // Quote marks
            // If LineParser::IgnoreQuotes, don't keep delimiters and other quote marks inside the quoted text.
            case (34): // Double quotes
            case (39): // Single quotes
                if (optionMask & LineParser::IgnoreQuotes)
                    break;
                if (quotechar == '\0')
                {
                    quotechar = c;
                    quoting = true;
                }
                else if (quotechar == c)
                {
                    quotechar = '\0';
                    hadquotes = true;
                    done = true;
                    quoting = false;
                }
                else
                    arg += c;
                break;
            // Curly brackets - treat in the same way as quotes
            case ('{'):
            case ('}'):
                // If explicitly not using braces, add as normal character
                if (!(optionMask & LineParser::UseBraces))
                    arg += c;
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
                        arg += c;

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
                arg += c;
                break;
            // Comment markers
            case ('#'): // "#" Rest/all of line is a comment
                if (quoting)
                {
                    arg += c;
                    break;
                }
                endOfLine_ = true;
                done = true;
                break;
            // Normal character
            default:
                arg += c;
                break;
        }
        // Increment line position
        ++linePos_;
        if (done || failed)
            break;
    }

    // Check for end of line
    if (linePos_ == line_.length())
        endOfLine_ = true;

    if (failed)
        return false;

    // Return false if there are no characters in the argument, unless it was a quoted "null string" arg
    return (arg.length() == 0 ? (hadquotes ? true : false) : true);
}

// Get all arguments (delimited) from LineParser::line_
void LineParser::getAllArgsDelim(int optionMask)
{
    // Parse the string in 'line_' into arguments in 'args'
    arguments_.clear();
    endOfLine_ = false;
    while (!endOfLine_)
    {
        // We must pass on the current optionMask, else it will be reset by the default value in getNextArg()
        std::string nextArg;
        if (getNextArg(optionMask, nextArg))
        {
            // Add this char to the list
            arguments_.push_back(nextArg);
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

// Set line and parse into delimited arguments
void LineParser::getArgsDelim(int optionMask, std::string_view s)
{
    line_ = s;
    linePos_ = 0;
    getAllArgsDelim(optionMask);
}

// Read single line from internal file source
LineParser::ParseReturnValue LineParser::readNextLine(int optionMask)
{
    line_.clear();

    // Master will check the file and broadcast the result
    LineParser::ParseReturnValue result = LineParser::Success;
    if ((!processPool_) || processPool_->isMaster())
    {
        // Returns : 0=ok, 1=error, -1=eof
        if (fileInput_ && (inputFile_ == nullptr))
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
        int enumValue = getIntFromParseReturnValue(result);
        if (!processPool_->broadcast(enumValue))
            return LineParser::Fail;

        result = getParseReturnValueFromInt(enumValue);
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
            char c;
            result = LineParser::Fail;
            while (inputStream()->get(c).good())
            {
                if (c == '\r')
                {
                    if (inputStream()->peek() == '\n')
                        inputStream()->ignore();
                    break;
                }
                else if (c == '\n')
                    break;
                else if ((c == ';') && (optionMask & LineParser::SemiColonLineBreaks))
                    break;

                line_ += c;

                // Check here for overfilling the line_ buffer - perhaps it's a binary file?
                if (line_.length() >= 8096)
                    result = LineParser::Fail;
            }
            ++lastLineNo_;
            Messenger::printVerbose("Line from file is: [{}]\n", line_);

            // Remove comments from line
            if (!(optionMask & LineParser::KeepComments))
                DissolveSys::removeComments(line_);

            // If we are skipping blank lines, check for a blank line here
            if (!(optionMask & LineParser::KeepBlanks))
            {
                // Now, see if our line contains only blanks
                nchars = 0;
                nspaces = 0;
                for (auto &c : line_)
                {
                    nchars++;
                    if (std::isspace(c))
                        ++nspaces;
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

            linePos_ = 0;

            // Exit on EOF or error
            if (result != LineParser::Success)
                break;
        };
    }

    // Broadcast result
    if (processPool_)
    {
        int enumValue = getIntFromParseReturnValue(result);
        if (!processPool_->broadcast(enumValue))
            return LineParser::Fail;

        result = getParseReturnValueFromInt(enumValue);
        if (result != LineParser::Success)
            return result;
    }

    // Broadcast line
    if (processPool_)
    {
        if (!processPool_->broadcast(line_))
            return LineParser::Fail;

        if (processPool_->isSlave())
            linePos_ = 0;
    }

    return LineParser::Success;
}

// Read next line from internal source file, setting as parsing source and copying to specified string
LineParser::ParseReturnValue LineParser::readNextLine(int optionMask, std::string &dest)
{
    LineParser::ParseReturnValue result = readNextLine(optionMask);
    if (result != LineParser::Success)
        return result;
    dest = line_;
    return result;
}

// Skip lines from file
LineParser::ParseReturnValue LineParser::skipLines(int nlines)
{
    LineParser::ParseReturnValue result;
    for (auto n = 0; n < nlines; ++n)
    {
        result = readNextLine(LineParser::KeepBlanks);
        if (result != LineParser::Success)
            return result;
    }

    return LineParser::Success;
}

// Write line to file
bool LineParser::writeLine(std::string_view s) const
{
    auto result = true;

    // Master handles the writing
    if ((!processPool_) || processPool_->isMaster())
    {
        if (!directOutput_)
        {
            if (cachedFile_ == nullptr)
            {
                Messenger::print("Unable to delayed-writeLine - destination cache is not open.\n");
                return false;
            }
            else
                (*cachedFile_) << s << std::endl;
        }
        else if (outputFile_ == nullptr)
        {
            Messenger::print("Unable to direct-writeLine - destination file is not open.\n");
            return false;
        }
        else
            (*outputFile_) << s << std::endl;
    }

    // Broadcast result of write
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

// Write long int argument as single line
bool LineParser::writeArg(long int i) const { return writeLineF("{}\n", i); }

// Write long long int argument as single line
bool LineParser::writeArg(long long int i) const { return writeLineF("{}\n", i); }

// Read long int argument as single line
bool LineParser::readArg(long int &i)
{
    if (readNextLine(LineParser::Defaults) != LineParser::Success)
        return false;

    i = std::stol(line_);

    return true;
}

// Read long long int argument as single line
bool LineParser::readArg(long long int &i)
{
    if (readNextLine(LineParser::Defaults) != LineParser::Success)
        return false;

    i = std::stoll(line_);

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
            Messenger::error("Tried to commit cached writes when direct output was enabled.\n");
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
            Messenger::error("Couldn't open output file '{}' for writing.\n", outputFilename_);
            result = false;
        }
    }

    // Broadcast result of write
    if (processPool_ && (!processPool_->broadcast(result)))
        return false;

    return result;
}

/*
 * Argument Data
 */

// Returns number of arguments grabbed from last parse
int LineParser::nArgs() const { return arguments_.size(); }

// Returns the specified argument as a string
std::string LineParser::args(int i) const
{
    if ((i < 0) || (i >= nArgs()))
    {
        Messenger::warn("LineParser::args() - Argument {} is out of range - returning \"NULL\"...\n", i);
        return "NULL";
    }
    return arguments_[i];
}

// Returns the specified argument as a character string view
std::string_view LineParser::argsv(int i) const
{
    if ((i < 0) || (i >= nArgs()))
    {
        Messenger::warn("LineParser::args() - Argument {} is out of range - returning \"NULL\"...\n", i);
        return "NULL";
    }
    return arguments_[i];
}

// Returns the specified argument as an integer
int LineParser::argi(int i) const
{
    if ((i < 0) || (i >= nArgs()))
    {
        Messenger::warn("LineParser::argi() - Argument {} is out of range - returning 0...\n", i);
        return 0;
    }
    return std::stoi(arguments_[i]);
}

// Returns the specified argument as a long integer
long int LineParser::argli(int i) const
{
    if ((i < 0) || (i >= nArgs()))
    {
        Messenger::warn("LineParser::argli() - Argument {} is out of range - returning 0...\n", i);
        return 0;
    }
    return std::stol(arguments_[i]);
}

// Returns the specified argument as a double
double LineParser::argd(int i) const
{
    if ((i < 0) || (i >= nArgs()))
    {
        Messenger::warn("LineParser::argd() - Argument {} is out of range - returning 0.0...\n", i);
        return 0.0;
    }

    // Attempt to convert the current argument
    try
    {
        return std::stod(arguments_[i]);
    }
    catch (std::out_of_range &rangeError)
    {
        std::string exponent{DissolveSys::afterChar(arguments_[i], "eE")};
        if (exponent.empty())
            Messenger::printVerbose(
                "LineParser::argd() : String '{}' causes an out-of-range exception on conversion - returning 0.0...",
                arguments_[i]);
        else if (std::stoi(exponent) >= std::numeric_limits<int>::max_exponent)
            Messenger::printVerbose("LineParser::argd() : String '{}' causes an overflow on conversion - returning 0.0...",
                                    arguments_[i]);
        else if (std::stoi(exponent) <= std::numeric_limits<int>::min_exponent)
            Messenger::printVerbose("LineParser::argd() : String '{}' causes an underflow on conversion - returning 0.0...",
                                    arguments_[i]);
    }

    return 0.0;
}

// Returns the specified argument as a bool
bool LineParser::argb(int i) const
{
    if ((i < 0) || (i >= nArgs()))
    {
        Messenger::warn("LineParser::argb() - Argument {} is out of range - returning false...\n", i);
        return false;
    }
    return DissolveSys::stob(arguments_[i]);
}

// Return the specified and next two arguments as a Vec3<int>
Vec3<int> LineParser::arg3i(int i) const
{
    if ((i < 0) || (i >= (nArgs() - 2)))
    {
        Messenger::warn("LineParser::arg3i() - Argument {} is out of range - returning Vec3<int>()...\n", i);
        return Vec3<int>();
    }
    return Vec3<int>(argi(i), argi(i + 1), argi(i + 2));
}

// Return the specified and next two arguments as a Vec3<double>
Vec3<double> LineParser::arg3d(int i) const
{
    if ((i < 0) || (i >= (nArgs() - 2)))
    {
        Messenger::warn("LineParser::arg3d() - Argument {} is out of range - returning Vec3<double>()...\n", i);
        return Vec3<double>();
    }
    return Vec3<double>(argd(i), argd(i + 1), argd(i + 2));
}

// Return a vector of double parameters, starting from the specified argument
std::vector<double> LineParser::argvd(int i) const
{
    if ((i < 0) || (i >= nArgs()))
    {
        Messenger::warn("LineParser::argvd() - Argument {} is out of range - returning std::vector<double>()...\n", i);
        return std::vector<double>();
    }
    std::vector<double> result;
    for (auto n = i; n < nArgs(); ++n)
        result.push_back(argd(n));
    return result;
}

// Returns whether the specified argument exists (has been provided)
bool LineParser::hasArg(int i) const
{
    if ((i < 0) || (i >= nArgs()))
        return false;
    return true;
}

// Utility LineParser::ParseReturnValue converter to int
int LineParser::getIntFromParseReturnValue(LineParser::ParseReturnValue value)
{
    int enumValue = value;
    return enumValue;
}

// Utility LineParser::ParseReturnValue converter from int
LineParser::ParseReturnValue LineParser::getParseReturnValueFromInt(int value)
{
    if (value == -1)
        return LineParser::ParseReturnValue::EndOfFile;
    else if (value == 0)
        return LineParser::ParseReturnValue::Success;

    return LineParser::ParseReturnValue::Fail;
}
