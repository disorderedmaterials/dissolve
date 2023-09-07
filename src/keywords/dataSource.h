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
#include <queue>

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
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override { return 0; };
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return std::nullopt; };
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        // Emplacing back on data vector and getting the reference to the objects
        DataSource dataSourceA, dataSourceB;
        // Create a queue for the dataSource objects
        std::queue<DataSource *> sourceQueue({&dataSourceA, &dataSourceB});

        // Read the next line
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        {
            return false;
        }

        while (!parser.eofOrBlank())
        {
            // Only allows maximum of two data sources per keyword
            if (sourceQueue.empty())
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
                // Add data to dataSource
                sourceQueue.front()->addData(parser.argsv(1));
                // Remove dataSource from queue
                sourceQueue.pop();
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

                // Add data to dataSource
                sourceQueue.front()->addData(data, format);
                // Remove dataSource from queue
                sourceQueue.pop();
            }
            else
            {
                return Messenger::error("Unsupported data source type '{}' provided to keyword '{}'\n", parser.argsv(0),
                                        name());
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

        dataSources_.push_back({std::move(dataSourceA), std::move(dataSourceB)});

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
    SerialisedValue serialise() const override
    {
        return fromVector(dataSources_,
                          [](const auto &item) -> SerialisedValue
                          {
                              SerialisedValue result = toml::array{};
                              auto &[dataSourceA, dataSourceB] = item;
                              result.push_back(dataSourceA.serialise());
                              // If optional second data source exists
                              if (dataSourceB.dataExists())
                              {
                                  result.push_back(dataSourceB.serialise());
                              }
                              return result;
                          });
    }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override
    {
        // Emplacing back on data vector and getting the reference to the objects
        DataSource dataSourceA, dataSourceB;
        // Create a queue for the dataSource objects
        std::queue<DataSource *> sourceQueue({&dataSourceA, &dataSourceB});

        toVector(node,
                 [this, &coreData, &sourceQueue](const auto &item)
                 {
                     // If data source type is internal
                     if (DataSource::dataSourceTypes().enumeration(toml::find<std::string>(item, "dataSourceType")) ==
                             DataSource::Internal &&
                         !sourceQueue.empty())
                     {
                         // Add data to dataSource
                         sourceQueue.front()->addData(toml::find<std::string>(item, "data"));
                         // Remove dataSource from queue
                         sourceQueue.pop();
                     }
                     // If data source type is external
                     else if (!sourceQueue.empty())
                     {
                         DataType data;
                         DataFormat format;

                         // Deserialise FileAndFormat
                         format.deserialise(item.at("data"), coreData);
                         // Import data
                         format.importData(data);

                         // Add data to dataSource
                         sourceQueue.front()->addData(toml::find<std::string>(item, "data"));
                         // Remove dataSource from queue
                         sourceQueue.pop();
                     }
                 });

        dataSources_.push_back({std::move(dataSourceA), std::move(dataSourceB)});
    }
};
