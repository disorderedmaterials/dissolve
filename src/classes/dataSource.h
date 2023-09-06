// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "io/fileAndFormat.h"
#include "items/list.h"
#include "keywords/dataSourceBase.h"
#include "math/data1D.h"
#include "math/dataBase.h"
#include "math/sampledData1D.h"

class DataSource : public Serialisable<const CoreData &>
{
    public:
    DataSource() = default;
    DataSource(const DataSource &dataSource) = default;
    DataSource(DataSource &&dataSource) = default;
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

    private:
    DataSourceType dataSourceType_;
    std::string internalDataSource_;
    std::unique_ptr<FileAndFormat> externalDataSource_;
    std::unique_ptr<DataBase> data_;

    public:
    void addData(std::string_view internalDataSource)
    {
        dataSourceType_ = Internal;
        internalDataSource_ = internalDataSource;
    }
    template <class DataType, class Format> void addData(DataType data, Format &fileAndFormat)
    {
        dataSourceType_ = External;
        externalDataSource_ = std::make_unique<Format>(fileAndFormat);
        auto tempData = std::make_unique<DataType>(data);
        data_ = std::move(tempData);
    }
    template <class DataType, class Format> bool sourceData(GenericList &processingModuleData)
    {
        if (!dataExists())
        {
            return false;
        }
        if (dataSourceType_ == Internal)
        {
            // If data is a child of Data1DBase
            if (std::is_convertible<DataType *, Data1DBase *>::value)
            {
                // Locate target data from tag and cast to base
                auto optData = processingModuleData.searchBase<Data1DBase, Data1D, SampledData1D>(internalDataSource_);
                if (!optData)
                {
                    return Messenger::error("No data with tag '{}' exists.\n", internalDataSource_);
                }
                Messenger::print("Sourced data '{}'", internalDataSource_);
                // Fill object with located data
                data_ = std::make_unique<DataType>(optData->get());
            }

            // Data2D, Data3D
            else
            {
                // Locate target data from tag and cast to base
                auto optData = processingModuleData.search<const DataType>(internalDataSource_);
                if (!optData)
                {
                    return Messenger::error("No data with tag '{}' exists.\n", internalDataSource_);
                }
                Messenger::print("Sourced data '{}'", internalDataSource_);

                // Fill object with located data
                data_ = std::make_unique<DataType>(optData->get());
            }
        }

        return data_.get() != nullptr ? true : false;
    }
    template <class DataType> DataType data() const
    {
        assert(dataExists());
        auto data = dynamic_cast<DataType &>(*data_);

        return data;
    }

    // Return if data exists and has been initialised
    bool dataExists() const;
    // Return internal data source
    std::optional<std::string> internalDataSource() const;
    // Return external data source
    OptionalReferenceWrapper<FileAndFormat> externalDataSource() const;

    /*
     * I/O
     */
    public:
    // Write through specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &data) override;
};
