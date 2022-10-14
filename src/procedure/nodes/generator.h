#include "procedure/nodes/node.h"

NodeRef nodeGenerator(ProcedureNode::NodeType type, const SerialisedValue &node, const CoreData &data,
                      const std::shared_ptr<SelectProcedureNode> parent = nullptr);
