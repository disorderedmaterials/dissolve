// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>

// Create a temporary file in the appropriate temp directory. Also
// make a best effort to delete the file when we're done with it.
namespace UnitTest
{
class TempFile
{
    public:
    TempFile(std::optional<std::string> name = {})
    {
        path = std::filesystem::temp_directory_path();
        path /= name ? *name : randomName() + ".dissolve.tmp";
    }
    ~TempFile()
    {
        if (out_.is_open())
            out_.close();
        if constexpr (delete_files)
            std::filesystem::remove(path);
    }

    std::ofstream &out()
    {
        if (!out_.is_open())
        {
            out_.open(path);
        }
        return out_;
    }

    // Get the file name on conversion to string
    operator std::string() const { return path; }
    operator std::filesystem::path() const { return path; }

    private:
    // The actual path of the temp file
    std::filesystem::path path;
    // A compile time switch to control deleting the temp files.  Set
    // this to false while debugging to read keep the file so you can
    // analyse the output.
    constexpr static bool delete_files = true;
    // A local stream that can be used for writing
    std::ofstream out_;

    // Generate random names for files
    static std::string randomName()
    {
        std::string result(10, ' ');
        std::ranges::generate(
            result, [] { return std::string_view("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz").at(rand() % 52); });
        return result;
    }
};
}; // namespace UnitTest
