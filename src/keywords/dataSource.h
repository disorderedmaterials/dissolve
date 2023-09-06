// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "keywords/dataSourceBase.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "templates/optionalRef.h"

// Keyword managing data sources
// Template arguments: data class (Data1D, Data2D ...), data import file format
template <class DataType, class DataFormat> class DataSourceKeyword : public DataSourceKeywordBase
{
    public:
    explicit DataSourceKeyword(std::vector<DataSourceKeywordBase::DataPair> &dataSources, std::string_view endKeyword)
        : DataSourceKeywordBase(dataSources, endKeyword){};
    ~DataSourceKeyword() override = default;

    /*
     * Data
     */
    private:
    // Format object for the data
    DataFormat format_;

    /*
     * Arguments
     */
    // Return minimum number of arguments accepted
    int minArguments() const override { return 0; };
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return std::nullopt; };
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        // Counter of dataSources being read
        int dataCounter = 0;
        // Emplacing back on data vector and getting the reference to the objects
        auto &newDataSource = dataSources_.emplace_back();
        auto &dataSourceA = newDataSource.first;
        auto &dataSourceB = newDataSource.second;

        // Read the next line
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        {
            return false;
        }

        while (!parser.eofOrBlank())
        {
            // Only allows maximum of two data sources per keyword
            if (dataCounter >= 2)
            {
                break;
            }
            // If data source type supplied is valid
            if (!DataSource::dataSourceTypes().isValid(parser.argsv(0)))
            {
                // If not, print accepted options
                return DataSource::dataSourceTypes().errorAndPrintValid(parser.argsv(0));
            }

            // If data is internal
            if (DataSource::dataSourceTypes().enumeration(parser.argsv(0)) == DataSource::Internal)
            {
                // Add data to pair element depending on counter
                dataCounter == 0 ? dataSourceA.addData(parser.argsv(1)) : dataSourceB.addData(parser.argsv(1));
                dataCounter++;
            }
            // If data is external
            else if (DataSource::dataSourceTypes().enumeration(parser.argsv(0)) == DataSource::External)
            {
                // Initialise data and format objects
                DataType data;
                DataFormat format;

                // Read the supplied arguments
                if (format.read(parser, 1, fmt::format("End{}", DataSource::dataSourceTypes().keyword(DataSource::External)),
                                coreData) != FileAndFormat::ReadResult::Success)
                {
                    return false;
                }
                // Import the data
                if (!format.importData(data, parser.processPool()))
                {
                    return false;
                }

                // Add data to pair element depending on counter
                dataCounter == 0 ? dataSourceA.addData<DataType, DataFormat>(data, format)
                                 : dataSourceB.addData<DataType, DataFormat>(data, format);
                dataCounter++;
            }
            else
            {
                return Messenger::error("Unsupported data location '{}' provided to keyword '{}'\n", parser.argsv(0), name());
            }

            // Read the next line
            if (parser.getArgsDelim() != LineParser::Success)
            {
                return false;
            }

            // Is this the end of the block?
            if (DissolveSys::sameString(parser.argsv(0), endKeyword_))
            {
                break;
            }
        }

        return true;
    }

    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        for (auto &[dataSourceA, dataSourceB] : dataSources_)
        {
            // Write the keyword name
            if (!parser.writeLineF("{}{}\n", prefix, keywordName))
            {
                return false;
            }
            // Serialise the first data source
            if (!dataSourceA.serialise(parser, keywordName, prefix))
            {
                return false;
            }
            // Skip to next iteration if dataSourceB is undefined
            if (!dataSourceB.dataExists())
            {
                continue;
            }

            // Serialise the second data source (optional)
            if (!dataSourceB.serialise(parser, keywordName, prefix))
            {
                return false;
            }

            // Write end keyword
            if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
            {
                return false;
            }
        }

        return true;
    }
    // Express as a serialisable value
    SerialisedValue serialise() const override { return {{}}; }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override {}
};
