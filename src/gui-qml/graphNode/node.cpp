#include "node.h"

GraphNode::OutgoingConnections &GraphNode::EdgeDefintion::outgoingConnections(const std::string &nodeName)
{
    return connections_[nodeName].first;
}


GraphNode::IncomingConnections &GraphNode::EdgeDefintion::outgoingConnections(const std::string &nodeName)
{
    return connections_[nodeName].second;
}