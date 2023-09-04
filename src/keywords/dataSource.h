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
    explicit DataSourceKeyword(std::shared_ptr<std::vector<DataPair>> dataSources, std::string_view endKeyword)
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
        while (!parser.eofOrBlank())
        {
            // Only allows maximum of two data sources per keyword
            if (dataCounter >= 2)
            {
                break;
            }

            DataPair dataPair;

            // If data source type supplied is valid
            if (!dataPair.first.dataSourceTypes().isValid(parser.argsv(startArg)))
            {
                return dataPair.first.dataSourceTypes().errorAndPrintValid(parser.argsv(startArg));
            }

            // If data is internal
            if (dataPair.first.dataSourceTypes().enumeration(parser.argsv(startArg)) == DataSource::Internal)
            {
                DataSource internalData{DataSource::Internal};
                internalData.addData(parser.argsv(startArg + 1));
                // Has the first item already been initialised?
                dataCounter == 0 ? dataPair.first = internalData : dataPair.second = internalData;
                dataCounter++;
            }
            // If data is external
            else if (dataPair.first.dataSourceTypes().enumeration(parser.argsv(startArg)) == DataSource::External)
            {
                DataSource externalData{DataSource::External};
                DataType data;
                DataFormat format;
                // Read the supplied arguements
                if (format->read(parser, startArg + 1, fmt::format("End{}", externalKeyword()), coreData) !=
                    FileAndFormat::ReadResult::Success)
                {
                    return false;
                }

                // Import the data
                if (!format->importData(data, parser.processPool()))
                {
                    return false;
                }

                externalData.addData(&data, format);

                // Has the first item already been initialised?
                dataCounter == 0 ? dataPair.first = externalData : dataPair.second = externalData;
                dataCounter++;
            }
            else
            {
                return Messenger::error("Unsupported data location '{}' provided to keyword '{}'\n", parser.argsv(startArg),
                                        name());
            }

            // Add the data to data vector
            dataSources_.get()->push_back(dataPair);

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
        for (auto &[dataSourceA, dataSourceB] : *dataSources_.get())
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
