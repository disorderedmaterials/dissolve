// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/dataSource.h"
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
// Template arguments: data class (Data1D, Data2D ...)
template <class DataType> class DataSourceKeyword : public DataSourceKeywordBase
{
    // Typedef
    public:
    using DataPair = std::pair<std::shared_ptr<DataSource<DataType>>, std::shared_ptr<DataSource<DataType>>>;

    public:
    DataSourceKeyword(std::vector<DataPair> &dataSources, std::string_view endKeyword)
        : DataSourceKeywordBase(), dataSources_(dataSources), endKeyword_(endKeyword)
    {
    }
    ~DataSourceKeyword() override = default;

    /*
     * Data
     */
    private:
    // Vector of data source pairs
    std::vector<DataPair> &dataSources_;
    // End keyword
    const std::string endKeyword_;
    // Gets path basename
    std::string_view getBasename(std::string_view filename) const { return filename.substr(filename.find_last_of("/\\") + 1); }

    public:
    // Return data source pairs
    std::vector<DataPair> &dataSources() { return dataSources_; }

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
        auto &[dataSourceA, dataSourceB] =
            dataSources_.emplace_back(std::make_shared<DataSource<DataType>>(), std::make_shared<DataSource<DataType>>());
        // Create a queue for the dataSource objects
        std::queue<std::shared_ptr<DataSource<DataType>>> sourceQueue({dataSourceA, dataSourceB});

        // Read the next line
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        {
            dataSources_.pop_back();
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
            if (!sourceQueue.front()->deserialise(parser, 0, coreData))
            {
                // If not, print accepted options
                dataSources_.pop_back();
                return false;
            }

            // Check to make sure we don't have the same names
            for (auto &[existingSourceA, existingSourceB] : dataSources_)
            {
                if (getBasename(existingSourceA->dataName()) == getBasename(sourceQueue.front()->dataName()))
                {
                    if (existingSourceA->getFilepath() != sourceQueue.front()->getFilepath())
                    {
                        existingSourceA->updateNameToPath();
                        sourceQueue.front()->updateNameToPath();
                    }
                }
                if (getBasename(existingSourceB->dataName()) == getBasename(sourceQueue.front()->dataName()))
                {
                    if (existingSourceB->getFilepath() != sourceQueue.front()->getFilepath())
                    {
                        existingSourceB->updateNameToPath();
                        sourceQueue.front()->updateNameToPath();
                    }
                }
            }

            sourceQueue.pop();

            // Read the next line
            if (parser.getArgsDelim() != LineParser::Success)
            {
                dataSources_.pop_back();
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
            if (!dataSourceA->serialise(parser, keywordName, prefix))
            {
                return false;
            }

            // Skip to next iteration if dataSourceB is undefined
            if (!dataSourceB->dataExists())
            {
                continue;
            }

            // Serialise the second data source (optional)
            if (!dataSourceB->serialise(parser, keywordName, prefix))
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
                              auto &[dataSourceA, dataSourceB] = item;
                              if (dataSourceB->dataExists())
                              {
                                  return {{"dataSourceA", dataSourceA->serialise()}, {"dataSourceB", dataSourceB->serialise()}};
                              }
                              else
                              {
                                  return {{"dataSourceA", dataSourceA->serialise()}};
                              }
                          });
    }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override
    {
        toVector(node,
                 [this, &coreData](const auto &dataPair)
                 {
                     // Emplacing back on data vector and getting the reference to the objects
                     auto &[dataSourceA, dataSourceB] = dataSources_.emplace_back(std::make_shared<DataSource<DataType>>(),
                                                                                  std::make_shared<DataSource<DataType>>());
                     // Create a queue for the dataSource objects
                     std::queue<std::shared_ptr<DataSource<DataType>>> sourceQueue({dataSourceA, dataSourceB});

                     toMap(dataPair,
                           [this, &coreData, &sourceQueue](const auto &key, const auto &dataSource)
                           {
                               if (sourceQueue.empty())
                                   return;
                               // Add data to dataSource
                               sourceQueue.front()->deserialise(dataSource, coreData);
                               // Check to make sure we don't have the same names
                               for (auto &[existingSourceA, existingSourceB] : dataSources_)
                               {
                                   if (getBasename(existingSourceA->dataName()) == getBasename(sourceQueue.front()->dataName()))
                                   {
                                       if (existingSourceA->getFilepath() != sourceQueue.front()->getFilepath())
                                       {
                                           existingSourceA->updateNameToPath();
                                           sourceQueue.front()->updateNameToPath();
                                       }
                                   }
                                   if (getBasename(existingSourceB->dataName()) == getBasename(sourceQueue.front()->dataName()))
                                   {
                                       if (existingSourceB->getFilepath() != sourceQueue.front()->getFilepath())
                                       {
                                           existingSourceB->updateNameToPath();
                                           sourceQueue.front()->updateNameToPath();
                                       }
                                   }
                               }
                               // Remove dataSource from queue
                               sourceQueue.pop();
                           });
                 });
    }
};
