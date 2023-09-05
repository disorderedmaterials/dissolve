// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/dataSource.h"

static EnumOptions<DataSourceType> DataSource::dataSourceTypes()
{
    return EnumOptions<DataSourceType>("DataSourceType", {{Internal, "Internal"}, {External, "External"}});
}

// Return whether or not the data has been sourced and exists
bool DataSource::dataExists() const
{
    return (!internalDataSource_.empty() && externalDataSource_ != std::nullptr) || data_.get() != std::nullptr;
}

// Return data source type enum
DataSourceType DataSource::dataSourceType() const { return dataSourceType_; }

// Return internal data source
std::optional<std::string> DataSource::internalDataSource() const
{
    return !internalDataSource_.empty() ? internalDataSource_ : std::nullopt;
}

// Return external data source
std::optional<FileAndFormat> &DataSource::externalDataSource() const
{
    return externalDataSource_ != std::nullptr ? *externalDataSource_.get() : std::nullopt;
}

// Write through specified LineParser
bool DataSource::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!dataExists())
    {
        return false;
    }

    // Write source: internal/external
    if (!parser.writeLineF("{}{}  ", prefix, dataSourceTypes().keyword(dataSourceType_)))
    {
        return false;
    }

    // If data is internal
    if (dataSourceType_ == Internal)
    {
        if (!parser.writeLineF("'{}'\n", internalDataSource_))
            return false;
    }
    // If data is external
    else
    {
        // Write filename and format
        if (externalDataSource_.get()->writeFilenameAndFormat(parser, prefix))
            return false;
        // Write extra keywords
        if (externalDataSource_.get()->writeBlock(parser, prefix)) // Express as a serialisable value
            return false;
        // End the block
        if (!parser.writeLineF("End{}", dataSourceTypes().keyword(External)))
            return false;
    }

    return true;
}

// Express as a serialisable value
SerialisedValue DataSource::serialise() const {}

// Read values from a serialisable value
void DataSource::deserialise(const SerialisedValue &node, const CoreData &data) {}
