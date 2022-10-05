#include "procedure/nodes/generator.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/parameters.h"

NodeRef nodeGenerator(ProcedureNode::NodeType type, const SerialisedValue &node, const CoreData &data)
{
    NodeRef result;
    switch (type)
    {
        case ProcedureNode::NodeType::Add:
            result = std::make_shared<AddProcedureNode>();
            break;
        case ProcedureNode::NodeType::Box:
            result = std::make_shared<BoxProcedureNode>();
            break;
        case ProcedureNode::NodeType::Parameters:
            result = std::make_shared<ParametersProcedureNode>();
            break;
    }
    result->deserialise(node, data);
    return result;
}
