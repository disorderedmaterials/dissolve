// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "io/fileAndFormat.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "items/list.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/sampledData1D.h"
#include "module/context.h"

// Template arguments: data class (Data1D, Data2D ...)
template <typename DataType> class DataSource : public Serialisable<const CoreData &>
{

    public:
    DataSource() = default;
    ~DataSource() = default;

    public:
    // Types of data sources allowed
    enum DataSourceType
    {
        Internal,
        External
    };

    public:
    // Return enum options for DataSourceType
    static EnumOptions<DataSourceType> dataSourceTypes()
    {
        return EnumOptions<DataSourceType>("DataSourceType", {{Internal, "Internal"}, {External, "External"}});
    }
    // Return data source type enum
    DataSourceType dataSourceType() const { return dataSourceType_; }

    /*
     * Data
     */

    private:
    // Name of data (tag or filename)
    std::string dataName_;
    // Type of data source being stored
    DataSourceType dataSourceType_;
    // String to hold internal data tag (if internal)
    std::string internalDataSource_;
    // Formatter object
    typename DataType::Formatter externalDataSource_;
    // Data object stored
    DataType data_;

    public:
    // Return data name
    std::string_view dataName() const { return dataName_; }
    // Return if data exists and has been initialised
    bool dataExists() const
    {
        return (!internalDataSource_.empty() && dataSourceType_ == Internal) ||
               (externalDataSource_.hasFilename() && dataSourceType_ == External);
    }

    // Changes data name to full filepath if data is external
    void updateNameToPath()
    {
        if (dataSourceType_ == External)
        {
            dataName_ = externalDataSource_.filename();
        }
    }

    std::string_view getFilepath() { return dataSourceType_ == External ? externalDataSource_.filename() : ""; }

    // Obtain data from the relevant source
    bool sourceData(const ProcessPool &procPool, GenericList &processingModuleData)
    {
        if (!dataExists())
        {
            return false;
        }
        if (dataSourceType_ == Internal)
        {
            // Locate target data from tag and cast to base
            auto optData = processingModuleData.search<const DataType>(internalDataSource_);
            if (!optData)
            {
                return Messenger::error("No data with tag '{}' exists.\n", internalDataSource_);
            }
            // Set data
            data_ = optData->get();

            return true;
        }
        else if (dataSourceType_ == External)
        {
            // For external datatypes, import the data
            if (!externalDataSource_.importData(data_, &procPool))
            {
                return Messenger::error("Error importing data from '{}'", externalDataSource_.filename());
            }

            return true;
        }

        return false;
    }

    // Return the data
    const DataType &data() const { return data_; }

    /*
     * I/O
     */
    public:
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData)
    {
        if (!DataSource<DataType>::dataSourceTypes().isValid(parser.argsv(0)))
        {
            return dataSourceTypes().errorAndPrintValid(parser.argsv(0));
        }

        // If data is internal
        if (dataSourceTypes().enumeration(parser.argsv(startArg)) == Internal)
        {
            // Add data to dataSource
            dataSourceType_ = Internal;
            internalDataSource_ = parser.argsv(startArg + 1);
            // Set data name to be data tag
            dataName_ = internalDataSource_;
            return true;
        }
        // If data is external
        else if (dataSourceTypes().enumeration(parser.argsv(startArg)) == External)
        {
            // Read the supplied arguments
            auto readResult = externalDataSource_.read(parser, startArg + 1,
                                                       fmt::format("End{}", dataSourceTypes().keyword(External)), coreData);
            if (readResult == FileAndFormat::ReadResult::UnrecognisedFormat ||
                readResult == FileAndFormat::ReadResult::UnrecognisedOption)
            {
                return Messenger::error("Failed to read file/format for '{}'.\n", parser.argsv(startArg + 2));
            }
            else
            {
                dataSourceType_ = External;
                // Set data name to be base filename
                dataName_ = externalDataSource_.filename().substr(externalDataSource_.filename().find_last_of("/\\") + 1);
                return true;
            }
        }
        return false;
    }

    void deserialise(const SerialisedValue &node, const CoreData &coreData)
    {
        auto dataSourceType = toml::find<std::string>(node, "dataSourceType");
        if (dataSourceTypes().enumeration(dataSourceType) == Internal)
        {
            dataSourceType_ = Internal;
            // Set data to be the tag
            internalDataSource_ = toml::find<std::string>(node, "source");
            // Set data name to be data tag
            dataName_ = toml::find<std::string>(node, "source");
        }
        // If data source type is external
        else if (dataSourceTypes().enumeration(dataSourceType) == External)
        {
            dataSourceType_ = External;
            // Read the file and format
            externalDataSource_.deserialise(node.at("source"), coreData);
            // Set the data name as root filename
            dataName_ = externalDataSource_.filename().substr(externalDataSource_.filename().find_last_of("/\\") + 1);
        }
    }

    // Write through specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
    {
        // Write source: internal/external
        if (!parser.writeLineF("  {}{}", prefix, dataSourceTypes().keyword(dataSourceType_)))
        {
            return false;
        }

        // If data is internal
        if (dataSourceType_ == Internal)
        {
            if (!parser.writeLineF("  '{}'\n", internalDataSource_))
                return false;
        }

        else if (dataSourceType_ == External)
        {
            // Write filename and format
            if (!externalDataSource_.writeFilenameAndFormat(parser, "  "))
            {
                return false;
            }

            // Write extra keywords
            if (!externalDataSource_.writeBlock(parser, fmt::format("  {}", prefix)))
            {
                return false;
            }

            // End the block
            if (!parser.writeLineF("      End{}\n", dataSourceTypes().keyword(External)))
            {
                return false;
            }

            return true;
        }

        return true;
    }
    // Express as a serialisable value
    SerialisedValue serialise() const
    {
        if (dataSourceType_ == Internal)
        {
            return {{"dataSourceType", dataSourceTypes().keyword(dataSourceType_)}, {"source", internalDataSource_}};
        }
        else
        {
            return {{"dataSourceType", dataSourceTypes().keyword(dataSourceType_)},
                    {"source", externalDataSource_.serialise()}};
        }
    }
};
