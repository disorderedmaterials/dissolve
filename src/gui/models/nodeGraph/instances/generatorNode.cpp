#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/nodeWrapper.h"
#include "keywords/double.h"
#include "keywords/nodeValue.h"

template <> std::string nodeTypeName<GeneratorNode *>(GeneratorNode *const &value) { return "GeneratorNode"; }

template <> std::string nodeTypeIcon<GeneratorNode *>(GeneratorNode *const &value)
{
    auto name = GeneratorNode::nodeTypes().keyword(value->type());
    return "qrc:/Dissolve/icons/nodes/" + name + ".svg";
}

template <> std::string nodeName<GeneratorNode *>(GeneratorNode *const &value)
{
    std::string result = {value->name().begin(), value->name().end()};
    return result;
}

template <> QVariant nodeData(GeneratorNode *const &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::Temperature:
        {
            auto temp = value->keywords().find("Temperature");
            // Check that we found a keyword
            if (!temp)
            {
                std::cout << "Temperature not found in " << value->name() << " " << value->keywords().nVisibleKeywords()
                          << std::endl;
                for (auto x : value->keywords().targetKeywords())
                    std::cout << x << std::endl;
                return {};
            }
            auto as_double = dynamic_cast<NodeValueKeyword *>(temp.value().first);
            if (as_double)
                return QVariant::fromValue(as_double->data().asDouble());
            std::cout << "Temperature is not a NodeValue in " << value->name() << std::endl;
            return {};
        }
        default:
            return {};
    }
}

template <> bool nodeSetData(GeneratorNode *&item, const QVariant &value, int role) { return false; }

template <> bool nodeDelete(GeneratorNode *&item, CoreData &coreData)
{
  for (auto &conf : coreData.configurations())
  {
    conf->generator().removeNode(item->shared_from_this());
  }
  return true;
}
