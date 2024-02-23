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
#include <variant>

template <typename DataType> class DataSource
{

    public:
    DataSource() = default;
    ~DataSource() = default;

    public:
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
    // Pointer to formatter object
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
    // Function to source data
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
    // Function to add internal data
    void addData(std::string_view internalDataSource)
    {
        dataSourceType_ = Internal;
        internalDataSource_ = internalDataSource;
        // Set data name to be data tag
        dataName_ = internalDataSource;
    }
    // Function to add external data
    bool addData(LineParser &parser, int startArg, std::string_view endKeyword, const CoreData &coreData)
    {
        dataSourceType_ = External;
        // Create format object in place in variant
        auto readResult = externalDataSource_.read(parser, startArg, endKeyword, coreData);
        if (readResult == FileAndFormat::ReadResult::UnrecognisedFormat ||
            readResult == FileAndFormat::ReadResult::UnrecognisedOption)
        {
            return false;
        }
        // Set data name to be base filename
        dataName_ = externalDataSource_.filename().substr(externalDataSource_.filename().find_last_of("/\\") + 1);

        return true;
    }

    // Returns data
    const DataType &data() const
    {
        if (!dataExists())
        {
            throw(std::runtime_error("Data doesn't exist\n"));
        }
        return data_;
    }

    /*
     * I/O
     */
    public:
    // Write through specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
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

        else if (dataSourceType_ == External)
        {
            // Write filename and format
            if (!externalDataSource_.writeFilenameAndFormat(parser, prefix))
            {
                return false;
            }

            // Write extra keywords
            if (!externalDataSource_.writeBlock(parser, prefix)) // Express as a serialisable value
            {
                return false;
            }

            // End the block
            if (!parser.writeLineF("End{}", dataSourceTypes().keyword(External)))
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
        SerialisedValue result;

        result["dataSourceType"] = dataSourceTypes().keyword(dataSourceType_);
        if (dataSourceType_ == Internal)
        {
            result["data"] = internalDataSource_;
        }
        else
        {
            result["data"] = externalDataSource_.serialise();
        }

        return result;
    }
};
