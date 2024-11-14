#include "classes/configuration.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/nodeWrapper.h"
#include "configuration.h"

template <> std::string nodeTypeName<Configuration *>(Configuration *const &value) { return "Configuration"; }

template <> std::string nodeTypeIcon<Configuration *>(Configuration *const &value)
{
    return "qrc:/Dissolve/icons/configuration.svg";
}

template <> std::string nodeName<Configuration *>(Configuration *const &value)
{

    if (!value)
        return std::string("Null");
    std::string s = {value->niceName().begin(), value->niceName().end()};
    return s;
}

template <> QVariant nodeData(Configuration *const &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::Temperature:
            return value->temperature();
        case names::AtomicDensity:
            return value->atomicDensity().value_or(0.0);
        default:
            return {};
    }
}

template <> bool nodeSetData(Configuration *&item, const QVariant &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    switch (role)
    {
        case names::Temperature:
            item->setTemperature(value.toDouble());
            return true;
        default:
            return false;
    }
}

template <> bool nodeDelete(Configuration *&item, typename GraphNodeContext<Configuration *>::type &coreData)
{
  coreData->removeConfiguration(item);
  return true;
}
