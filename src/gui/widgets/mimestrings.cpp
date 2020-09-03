/*
    *** Mime Strings
    *** src/gui/widgets/mimestrings.cpp
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

#include "gui/widgets/mimestrings.h"

/*
 * Mime String
 */

MimeString::MimeString(MimeString::MimeStringType type, std::string_view data) : type_(type), data_(data) {}

// Return type of data contained in string
MimeString::MimeStringType MimeString::type() const { return type_; }

// Return string data
std::string_view MimeString::data() const { return data_; }

/*
 * Mime Strings
 */

MimeStrings::MimeStrings() {}

MimeStrings::~MimeStrings() {}

MimeStrings::MimeStrings(const MimeStrings &source) { (*this) = source; }

void MimeStrings::operator=(const MimeStrings &source) { strings_ = source.strings_; }

bool MimeStrings::hasFormat(const QString &mimeType) const
{
    if (mimeType == "dissolve/mimestrings")
        return true;

    return false;
}

QStringList MimeStrings::formats() const { return QStringList() << "dissolve/mimestrings"; }

QVariant MimeStrings::retrieveData(const QString &mimeType, QVariant::Type type) const { return QVariant(); }

// Add mime string
void MimeStrings::add(MimeString::MimeStringType type, std::string_view data) { strings_.emplace_back(type, data); }

// Add mime strings from source MimeStrings
void MimeStrings::add(MimeStrings &sourceStrings)
{
    for (auto &mimeString : sourceStrings.strings())
        add(mimeString.type(), mimeString.data());
}

// Return whether the specified MimeString data is present
bool MimeStrings::hasData(MimeString::MimeStringType type) const
{
    return (std::find_if(strings_.begin(), strings_.end(),
                         [type](const auto &mimeString) { return mimeString.type() == type; }) != strings_.end());
}

// Return the data for the specified type
std::string_view MimeStrings::data(MimeString::MimeStringType type) const
{
    auto it =
        std::find_if(strings_.begin(), strings_.end(), [type](const auto &mimeString) { return mimeString.type() == type; });

    return it == strings_.end() ? "" : it->data();
}

// Return mime strings
std::vector<MimeString> &MimeStrings::strings() { return strings_; }
