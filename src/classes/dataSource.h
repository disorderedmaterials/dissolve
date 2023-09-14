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
#include "math/sampledData1D.h"
#include <variant>

// Foward declarations
class Data1DImportFileFormat;
class Data2DImportFileFormat;
class Data3DImportFileFormat;

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
    // Variant to hold file and format of external data (if external)
    Format externalDataSource_;
    // Variant to hold data
    DataType data_;

    public:
    // Return data name
    std::string_view dataName() const;
    // Return if data exists and has been initialised
    bool dataExists() const;
    // Return internal data source
    std::optional<std::string> internalDataSource() const;
    // Return external data source
    Format &externalDataSource();
    // Function to source data (only required for internal data sources)
    bool sourceData(GenericList &processingModuleData);
    // Function to add internal data
    void addData(std::string_view internalDataSource);
    // Overloaded function to add external data
    template <class D> void addData(D data, class D::Formatter &fileAndFormat)
    {
        dataSourceType_ = External;
        // Create format object in place in variant
        externalDataSource_.emplace<class D::Formatter>(fileAndFormat);
        data_ = data;
        // Set data name to be base filename
        dataName_ = fileAndFormat.filename().substr(fileAndFormat.filename().find_last_of("/\\") + 1);
    }
    // Returns the data of the requested type
    template <class D> D data() const
    {
        assert(dataExists());
        return std::get<D>(data_);
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
