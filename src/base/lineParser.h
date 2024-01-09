// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "base/processPool.h"
#include "templates/vector3.h"
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Line Parser
class LineParser
{
    public:
    LineParser(const ProcessPool *procPool = nullptr);
    ~LineParser();
    // Parse Options Enum
    enum ParseOption
    {
        Defaults = 0,              /* Default parsing behaviour */
        KeepComments = 1,          /* Automatically strip comments (beginning # or //) from files */
        IgnoreQuotes = 2,          /* Quoted text in files will not be retained as a single argument */
        KeepBlanks = 4,            /* Don't automatically skip blank lines (those containing nothing or only whitespace) */
        StripBrackets = 8,         /* Remove parentheses during parsing */
        NoEscapes = 16,            /* Don't convert excaped characters */
        UseBraces = 32,            /* Text inside curly brackets is retaind as a single argument */
        CommasAreDelimiters = 64,  /* Treat commas as delimiting marks in files */
        SemiColonLineBreaks = 128, /* Treat semicolons as line-breaks */
        nParseOptions = 9
    };
    // Parse Return Value
    enum ParseReturnValue
    {
        EndOfFile = -1, /* Operation encountered the end of the current file */
        Success = 0,    /* Operation succeeded */
        Fail = 1        /* Operation failed */
    };

    /*
     * Source / Destination Streams
     */
    private:
    // Associated process pool (if any)
    const ProcessPool *processPool_;
    // Current input filename (if any)
    std::string inputFilename_;
    // Current output filename (if any)
    std::string outputFilename_;
    // Line to parse
    std::string line_;
    // Current reading position in line
    int linePos_;
    // Integer line number of last read line
    int lastLineNo_;
    // Source stream for reading
    std::ifstream *inputFile_;
    // Source string stream for reading
    std::stringstream *inputStrings_;
    // Target stream for writing
    std::ofstream *outputFile_;
    // Target stream for cached writing
    std::stringstream *cachedFile_;

    private:
    // Reset data
    void reset();
    // Return current stream for input
    std::istream *inputStream() const;

    public:
    // Return associated process pool (if any)
    const ProcessPool *processPool() const;
    // Return filename of current inputFile (if any)
    std::string_view inputFilename() const;
    // Return filename of current outputFile (if any)
    std::string_view outputFilename() const;
    // Return current line
    std::string_view line() const;
    // Return integer line number of last read line
    int lastLineNo() const;
    // Return read-only status of file
    bool isFileReadOnly() const;
    // Open new file for reading
    bool openInput(std::string_view filename);
    // Open input string for reading
    bool openInputString(std::string_view s);
    // Open new stream for writing
    bool openOutput(std::string_view filename, bool directOutput = true);
    // Open existing stream for writing
    bool appendOutput(std::string_view filename);
    // Close file(s)
    void closeFiles();
    // Return whether current file source is good for reading
    bool isFileGoodForReading() const;
    // Return whether current file source is good for writing
    bool isFileGoodForWriting() const;
    // Tell current position of input stream
    std::streampos tellg() const;
    // Peek next character in input stream
    char peek() const;
    // Seek position in input stream
    void seekg(std::streampos pos);
    // Seek n bytes in specified direction in input stream
    void seekg(std::streamoff off, std::ios_base::seekdir dir);
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
    // Whether the end of the string has been found in getNextArg()
    bool endOfLine_;

    private:
    // Gets next delimited arg from internal line
    bool getNextArg(int optionMask, std::string &destarg);
    // Gets all delimited args from internal line
    void getAllArgsDelim(int optionMask);

    public:
    // Read line from file and do delimited parse
    ParseReturnValue getArgsDelim(int optionMask = LineParser::Defaults);
    // Set line and parse into delimited arguments
    void getArgsDelim(int optionMask, std::string_view s);
    // Read next line from internal source file, setting as parsing source
    ParseReturnValue readNextLine(int optionMask);
    // Read next line from internal source file, setting as parsing source and copying to specified string
    ParseReturnValue readNextLine(int optionMask, std::string &dest);
    // Skip 'n' lines from internal file
    ParseReturnValue skipLines(int nlines);
    // Write line to file
    bool writeLine(std::string_view s) const;
    // Write formatted line to file
    template <typename... Args> bool writeLineF(std::string_view format, Args... args) const
    {
        auto result = true;

        // Master handles the writing
        if ((!processPool_) || processPool_->isMaster())
        {
            if (!directOutput_)
            {
                if (cachedFile_ == nullptr)
                {
                    Messenger::print("Unable to delayed-writeLineF - destination cache is not open.\n");
                    result = false;
                    if (processPool_ && (!processPool_->broadcast(result)))
                        return false;
                    return false;
                }
            }
            else if (outputFile_ == nullptr)
            {
                Messenger::print("Unable to direct-writeLineF - destination file is not open.\n");
                result = false;
                if (processPool_ && (!processPool_->broadcast(result)))
                    return false;
                return false;
            }

            // Format the line and store it
            if (directOutput_)
                (*outputFile_) << fmt::format(format, args...);
            else
                (*cachedFile_) << fmt::format(format, args...);
        }

        // Broadcast result of write
        if (processPool_ && (!processPool_->broadcast(result)))
            return false;

        return result;
    }
    // Print banner comment of fixed width
    template <typename... Args> bool writeBannerComment(std::string_view format, Args... args)
    {
        const auto bannerWidth = 80;
        static std::string bannerBorder = fmt::format("#{0:=^{1}}#", "", bannerWidth - 2);

        // Finally, print the banner
        if (!writeLineF("\n{}\n", bannerBorder))
            return false;
        if (!writeLineF("#{:^{}}#", fmt::format(format, args...), bannerWidth - 2))
            return false;
        if (!writeLineF("\n{}\n", bannerBorder))
            return false;

        return true;
    }
    // Write long int argument as single line
    bool writeArg(long int i) const;
    // Write long long int argument as single line
    bool writeArg(long long int i) const;
    // Read long int argument as single line
    bool readArg(long int &i);
    // Read long long int argument as single line
    bool readArg(long long int &i);
    // Commit cached output stream to actual output file
    bool commitCache();

    /*
     * Argument Data
     */
    private:
    // Parsed arguments
    std::vector<std::string> arguments_;

    public:
    // Returns number of arguments grabbed from last parse
    int nArgs() const;
    // Returns the specified argument as a string
    std::string args(int i) const;
    // Returns the specified argument as a string view
    std::string_view argsv(int i) const;
    // Returns the specified argument as an integer
    int argi(int i) const;
    // Returns the specified argument as a long integer
    long int argli(int i) const;
    // Returns the specified argument as a double
    double argd(int i) const;
    // Returns the specified argument as a bool
    bool argb(int i) const;
    // Return the specified and next two arguments as a Vec3<int>
    Vec3<int> arg3i(int i) const;
    // Return the specified and next two arguments as a Vec3<double>
    Vec3<double> arg3d(int i) const;
    // Return a vector of double parameters, starting from the specified argument
    std::vector<double> argvd(int i) const;
    // Returns whether the specified argument exists
    bool hasArg(int i) const;

    private:
    // Utility LineParser::ParseReturnValue converter to int
    int getIntFromParseReturnValue(LineParser::ParseReturnValue value);
    // Utility LineParser::ParseReturnValue converter from int
    LineParser::ParseReturnValue getParseReturnValueFromInt(int value);
};
