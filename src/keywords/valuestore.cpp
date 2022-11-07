// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/valuestore.h"
#include "base/lineparser.h"
#include "classes/valuestore.h"
#include "io/export/values.h"

ValueStoreKeyword::ValueStoreKeyword(ValueStore &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
ValueStore &ValueStoreKeyword::data() { return data_; }
const ValueStore &ValueStoreKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ValueStoreKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> ValueStoreKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool ValueStoreKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.argsv(startArg + 1) == "@")
        Messenger::print("Reading inline test data '{}' (format={})...\n", parser.argsv(startArg), parser.argsv(startArg + 2),
                         parser.argsv(startArg + 1));
    else
        Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                         parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    return true;
}

// Serialise data to specified LineParser
bool ValueStoreKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &[tag, data, format] : data_.data())
    {
        if (!format.writeFilenameAndFormat(parser, fmt::format("{}{}  '{}'  ", prefix, keywordName, tag)))
            return false;
        if (!format.writeBlock(parser, fmt::format("{}  ", prefix)))
            return false;
        if (!parser.writeLineF("{}End{}\n", prefix, name()))
            return false;

        // If data was read "inline" from the input file, write it out now
        if (format.filename() == "@")
        {
            ValueExportFileFormat exportFormat("@", ValueExportFileFormat::ValueExportFormat::Simple);
            if (!exportFormat.exportData(data, parser))
                return false;
        }
    }

    return true;
}

// Express as a tree node
SerialisedValue ValueStoreKeyword::serialise() const { return data_; }

// Read values from a tree node
void ValueStoreKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_.deserialise(node, coreData);
}

// Has not changed from initial value
bool ValueStoreKeyword::isDefault() const { return data_.data().empty(); }
