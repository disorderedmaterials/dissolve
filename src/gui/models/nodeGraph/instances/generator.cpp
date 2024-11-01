#include "generator/generator.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

template <> std::string nodeTypeName<Generator *>(Generator *const &value) { return "Generator"; }

template <> std::string nodeTypeIcon<Generator *>(Generator *const &value)
{
    return "file:/home/adam/Code/dissolve/src/gui/icons/generator.svg";
}

template <> std::string nodeName<Generator *>(Generator *const &value) { return "Generator"; }

template <> QVariant nodeData(Generator *const &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::Size:
            return QVariant::fromValue(value->rootSequence().nNodes());
        default:
            return {};
    }
}

template <> bool nodeSetData(Generator *&item, const QVariant &value, int role) { return false; }
