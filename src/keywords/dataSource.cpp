#include "dataSource.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"

template <typename T, typename F> DataSourceKeyword::DataSourceKeyword(F addData, std::string_view endKeyword)
{
    addData_ = addData;
    endKeyword_ = endKeyword;

    if (std::is_same_v<T, Data1D>)
    {
        format_ = Data1DFileAndFormat();
    }
    else if (std::is_same_v<T, Data2D>)
    {
        format_ = Data2DFileAndFormat();
    }
    else if (std::is_same_v<T, Data3D>)
    {
        format_ = Data3DFileAndFormat();
    }
}

DataSource &DataSourceKeyword::data()
{
    // TODO: insert return statement here
}

const DataSource &DataSourceKeyword::data() const
{
    // TODO: insert return statement here
}

// Return minimum number of arguments accepted
int DataSourceKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
std::optional<int> DataSourceKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
template <typename T, typename F>
bool DataSourceKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    /**
     * TODO: We get first line -> it is either internal or external data
     * TODO: We must pass argument one and then argument two (Maybe start by parsing lines)
     */
    std::vector<T> dataSources;

    while (!parser.eofOrBlank())
    {

        T data;

        if (DissolveSys::sameString(parser.argsv(startArg), internalKwd_))
        {
            data.setTag(parser.argsv(startArg + 1));
        }

        else if (DissolveSys::sameString(parser.argsv(startArg), externalKwd_))
        {
            if (format_.read(parser, startArg + 1, externalEndKwd_, coreData) != FileAndFormat::ReadResult::Success)
            {
                return false;
            }

            if (!format.importData(data, parser.processPool()))
            {
                return false;
            }
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

        dataSources.push_back(data);
    }

    if (!addData_(DataSources))
    {
        Messenger::error("Invalid data sources supplied for keyword '{}'", name());
        return false;
    }

    return true;
}

// Serialise data to specified LineParser
bool DataSourceKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return false;
}

// Express as a serialisable value
SerialisedValue DataSourceKeyword::serialise() const { return SerialisedValue(); }

// Read values from a serialisable value
void DataSourceKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) {}

// Has not changed from initial value
bool DataSourceKeyword::isDefault() const { return false; }

// Explicit instantiation of allowed template types
template DataSourceKeyword::DataSourceKeyword<Data1D, std::function<bool(std::vector<Data1D>)>>(
    std::function<bool(std::vector<Data1D>)>);
template DataSourceKeyword::DataSourceKeyword<Data2D, std::function<bool(std::vector<Data2D>)>>(
    std::function<bool(std::vector<Data2D>)>);
template DataSourceKeyword::DataSourceKeyword<Data3D, std::function<bool(std::vector<Data3D>)>>(
    std::function<bool(std::vector<Data3D>)>);
