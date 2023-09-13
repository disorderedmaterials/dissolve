// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "io/fileAndFormat.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "items/list.h"
#include "keywords/dataSourceBase.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/dataBase.h"
#include "math/sampledData1D.h"
#include <variant>

class DataSource
{
    public:
    using DataType = std::variant<Data1D, SampledData1D, Data2D, Data3D>;
    using Format = std::variant<Data1DImportFileFormat, Data2DImportFileFormat, Data3DImportFileFormat>;

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
    static EnumOptions<DataSourceType> dataSourceTypes();
    // Return data source type enum
    DataSourceType dataSourceType() const;

    /*
     * Data
     */

    private:
    // Name of data (tag or filename)
    std::string dataName_;
    // Type of data being stored
    DataSourceType dataSourceType_;
    // String to hold internal data tag (if internal)
    std::string internalDataSource_;
    // File and format of external data (if external)
    Format externalDataSource_;
    // DataBases pointer to hold data
    DataType data_;

    public:
    // Return data name
    std::string_view dataName() const;
    // Return if data exists and has been initialised
    bool dataExists() const;
    // Return internal data source
    std::optional<std::string> internalDataSource() const;
    // Return external data source
    Format &externalDataSource() const;
    // Function to add internal data
    void addData(std::string_view internalDataSource)
    {
        dataSourceType_ = Internal;
        internalDataSource_ = internalDataSource;
        // Set data name to be data tag
        dataName_ = internalDataSource;
    }
    // Overloaded function to add external data
    template <class D, class F> void addData(D data, F &fileAndFormat)
    {
        dataSourceType_ = External;
        externalDataSource_ = fileAndFormat;
        data_ = data;
        // Set data name to be base filename
        dataName_ = fileAndFormat.filename().substr(fileAndFormat.filename().find_last_of("/\\") + 1);
    }
    // Function to source data (only required for internal data sources)
    bool sourceData(GenericList &processingModuleData)
    {
        if (!dataExists())
        {
            return false;
        }
        if (dataSourceType_ == Internal)
        {
            if (std::holds_alternative<Data1D>(data_) || std::holds_alternative<SampledData1D>(data_))
            {
                // Locate target data from tag and cast to base
                auto optData = processingModuleData.searchBase<Data1DBase, Data1D, SampledData1D>(internalDataSource_);
                if (!optData)
                {
                    return Messenger::error("No data with tag '{}' exists.\n", internalDataSource_);
                }
                // Create unique pointer for data
                data_ = optData->get();
            }

            // Data2D, Data3D
            else
            {
                return std::visit(
                    [&]<class T>(T &data)
                    {
                        // Locate target data from tag
                        auto optData = processingModuleData.search<const T>(internalDataSource_);
                        if (!optData)
                        {
                            return Messenger::error("No data with tag '{}' exists.\n", internalDataSource_);
                        }
                        // Store data
                        data_ = optData->get();
                        return true;
                    },
                    data_);
            }
        }

        return !data_.valueless_by_exception();
    }
    // Returns the data casted to the requested type
    DataType data() const
    {
        assert(dataExists());
        return data_;
    }

    /*
     * I/O
     */
    public:
    // Write through specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const;
    // Express as a serialisable value
    SerialisedValue serialise() const;
};
