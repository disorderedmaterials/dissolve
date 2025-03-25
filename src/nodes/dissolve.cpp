#include "nodes/dissolve.h"

// Set dissolve
void DissolveNode::setDissolve(Dissolve& dissolve)
{ 
    dissolve_ = dissolve;
}

// Return dissolve
Dissolve &DissolveNode::dissolve() const override
{
    return dissolve_;
}

// Return world pool
ProcessPool &DissolveNode::processPool() const override
{
    return dissolve_.processPool();
}
