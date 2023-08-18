#include "dataSource.h"

template <typename T, typename F> DataSourceKeyword::DataSourceKeyword(F addData) { addData_ = addData; }

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
    sourceType_ = parser.nArgs() == 1 ? SourceType::Internal : SourceType::External;

    T data;

    if (parser.nArgs() == 1)
    {
        data.setTag(parser.argsv(startArg));
    }

    else
        (data2D_) {}

    return false;
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
