// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "io/import/dataBase.h"
#include "keywords/dataSourceBase.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "templates/optionalRef.h"
#include <optional>
#include <vector>

// Keyword managing data sources
// Template arguements: data class (Data1D, Data2D ...), add data function type
template <class DataType, class DataFormat> class DataSourceKeyword : public DataSourceKeywordBase
{
    public:
    explicit DataSourceKeyword(std::vector<DataPair> &dataSources, std::string_view endKeyword)
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
    private:
    // End keyword for the dataSource keyword
    const std::string endKeyword_;

    // Return minimum number of arguments accepted
    int minArguments() const override { return 0; };
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return std::nullopt; };
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        int dataCounter = 0;
        // dataSources_.push_back(DataPair());
        // Add the data pair to data vector
        auto &[dataSourceA, dataSourceB] = dataSources_.emplace_back();

        while (!parser.eofOrBlank())
        {
            // Only allows maximum of two data sources per keyword
            if (dataCounter >= 2)
            {
                break;
            }

            // If data source type supplied is valid
            if (!DataSource::dataSourceTypes().isValid(parser.argsv(startArg)))
            {
                return DataSource::dataSourceTypes().errorAndPrintValid(parser.argsv(startArg));
            }

            // If data is internal
            if (DataSource::dataSourceTypes().enumeration(parser.argsv(startArg)) == DataSource::Internal)
            {

                // Has the first item already been initialised?
                dataCounter == 0 ? dataSourceA.addData(parser.argsv(startArg + 1))
                                 : dataSourceB.addData(parser.argsv(startArg + 1));
                dataCounter++;
            }
            // If data is external
            else if (DataSource::dataSourceTypes().enumeration(parser.argsv(startArg)) == DataSource::External)
            {
                DataType data;
                DataFormat format;
                // Read the supplied arguements
                if (format.read(parser, startArg + 1, fmt::format("End{}", externalKeyword()), coreData) !=
                    FileAndFormat::ReadResult::Success)
                {
                    return false;
                }

                // Import the data
                if (!format.importData(data, parser.processPool()))
                {
                    return false;
                }

                // Has the first item already been initialised?
                // Has the first item already been initialised?
                dataCounter == 0 ? dataSourceA.addData<DataType, DataFormat>(data, format)
                                 : dataSourceB.addData<DataType, DataFormat>(data, format);
                dataCounter++;
            }
            else
            {
                return Messenger::error("Unsupported data location '{}' provided to keyword '{}'\n", parser.argsv(startArg),
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

        return true;
    }

    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        for (auto &[dataSourceA, dataSourceB] : dataSources_)
        {
            if (!parser.writeLineF("{}{}\n", prefix, keywordName))
            {
                return false;
            }

            if (!dataSourceA.serialise(parser, keywordName, prefix))
            {
                return false;
            }

            // Skip to next iteration if dataSourceB is undefined
            if (!dataSourceB.dataExists())
            {
                continue;
            }

            // Second data source (optional)

            if (!dataSourceB.serialise(parser, keywordName, prefix))
            {
                return false;
            }

            if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
            {
                return false;
            }
        }

        return true;
    }
    // Express as a serialisable value
    SerialisedValue serialise() const override { return {{"type", "x"}, {"parameters", "y"}}; }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override {}
};
