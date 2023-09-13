// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/dataSource.h"
#include "templates/optionalRef.h"

// Return enum options for data source types
EnumOptions<DataSource::DataSourceType> DataSource::dataSourceTypes()
{
    return EnumOptions<DataSource::DataSourceType>("DataSourceType", {{Internal, "Internal"}, {External, "External"}});
}

// Return data name (tag or filename)
std::string_view DataSource::dataName() const { return dataName_; }

// Return whether or not the data has been sourced and exists
bool DataSource::dataExists() const
{
    return (!internalDataSource_.empty() && dataSourceType_ == Internal) ||
           (!externalDataSource_.valueless_by_exception() && !data_.valueless_by_exception() && dataSourceType_ == External);
}

// Return data source type enum
DataSource::DataSourceType DataSource::dataSourceType() const { return dataSourceType_; }

// Return internal data source
std::optional<std::string> DataSource::internalDataSource() const
{
    if (!internalDataSource_.empty())
    {
        return internalDataSource_;
    }
    return std::nullopt;
}

// Return external data source
DataSource::Format &DataSource::externalDataSource() const { return externalDataSource_; }

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
        return std::visit(
            [&](auto &data)
            {
                // Write filename and format
                if (!data.writeFilenameAndFormat(parser, prefix))
                {
                    return false;
                }

                // Write extra keywords
                if (!data.writeBlock(parser, prefix)) // Express as a serialisable value
                {
                    return false;
                }

                // Write extra keywords
                if (!data.writeBlock(parser, prefix)) // Express as a serialisable value
                {
                    return false;
                }

                // End the block
                if (!parser.writeLineF("End{}", dataSourceTypes().keyword(External)))
                {
                    return false;
                }

                return true;
            },
            externalDataSource_);
    }

    return true;
}

// Express as a serialisable value
SerialisedValue DataSource::serialise() const
{
    SerialisedValue result;

    result["dataSourceType"] = dataSourceTypes().keyword(dataSourceType_);
    if (dataSourceType_ == Internal)
    {
        result["data"] = internalDataSource_;
    }
    else
    {
        result["data"] = std::visit([](auto &data) { return data.serialise(); }, externalDataSource_);
    }

    return result;
}
