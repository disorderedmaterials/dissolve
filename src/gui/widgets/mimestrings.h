// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <QMimeData>

// Forward declarations
/* none */

// Mime String Object
class MimeString
{
    public:
    // Mime Strings Data Type
    enum MimeStringType
    {
        NoMimeType,
        LocalType,  /* Local block identifier */
        ModuleType, /* Module type */
        nMimeStringTypes
    };
    MimeString(MimeString::MimeStringType type = NoMimeType, std::string_view data = "");

    private:
    // Type of data contained in string
    MimeStringType type_;
    // String data
    std::string data_;

    public:
    // Return type of data contained in string
    MimeStringType type() const;
    // Return string data
    std::string_view data() const;
};

// Mime Strings Data
class MimeStrings : public QMimeData
{
    public:
    MimeStrings();
    ~MimeStrings();
    MimeStrings(const MimeStrings &source);
    void operator=(const MimeStrings &source);

    /*
     * Reimplementations
     */
    public:
    bool hasFormat(const QString &mimeType) const;
    QStringList formats() const;
    QVariant retrieveData(const QString &mimeType, QVariant::Type type) const;

    /*
     * Mime Data
     */
    private:
    // List of mime strings
    std::vector<MimeString> strings_;

    public:
    // Add mime string
    void add(MimeString::MimeStringType type, std::string_view data);
    // Add mime strings from source MimeStrings
    void add(MimeStrings &sourceStrings);
    // Return whether the specified MimeString data is present
    bool hasData(MimeString::MimeStringType type) const;
    // Return the data for the specified type
    std::string_view data(MimeString::MimeStringType type) const;
    // Return mime strings
    std::vector<MimeString> &strings();
};
