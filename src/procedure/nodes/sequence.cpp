/*
	*** Procedure Node - Sequence
	*** src/procedure/nodes/sequence.cpp
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "procedure/nodes/sequence.h"
#include "procedure/nodes/nodes.h"
#include "procedure/nodescopestack.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
SequenceProcedureNode::SequenceProcedureNode(ProcedureNode::NodeContext context, const char* blockTerminationKeyword) : ProcedureNode(ProcedureNode::SequenceNode)
{
	context_ = context;
	blockTerminationKeyword_ = blockTerminationKeyword;
}

// Destructor
SequenceProcedureNode::~SequenceProcedureNode()
{
	clear();
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool SequenceProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context != ProcedureNode::NoContext);
}

/*
 * Node Keywords
 */

// Set up keywords for node
void SequenceProcedureNode::setUpKeywords()
{
}

// Return enum option info for SequenceNodeKeyword
EnumOptions<SequenceProcedureNode::SequenceNodeKeyword> SequenceProcedureNode::sequenceNodeKeywords()
{
	static EnumOptionsList SequenceNodeTypeKeywords;

	static EnumOptions<SequenceProcedureNode::SequenceNodeKeyword> options("SequenceNodeKeyword", SequenceNodeTypeKeywords);

	return options;
}

/*
 * Node Contents
 */

// Clear all data
void SequenceProcedureNode::clear()
{
	sequence_.clear();
}

// Add (own) node into sequence, checking the context
void SequenceProcedureNode::addNode(ProcedureNode* node)
{
	if (!node) return;

	if (!node->isContextRelevant(context_)) Messenger::error("Node '%s' (type = '%s') is not relevant to the '%s' context.\n", node->name(), ProcedureNode::nodeTypes().keyword(node->type()), ProcedureNode::nodeContexts().keyword(context_));

	sequence_.own(node);
}

// Return sSequential node list
const List<ProcedureNode>& SequenceProcedureNode::sequence() const
{
	return sequence_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool SequenceProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Loop over nodes in the list, preparing each in turn
	ListIterator<ProcedureNode> nodeIterator(sequence_);
	while (ProcedureNode* node = nodeIterator.iterate()) if (!node->prepare(cfg, prefix, targetList)) return false;

	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult SequenceProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	ProcedureNode::NodeExecutionResult result = ProcedureNode::Success;

	// Loop over nodes in the list, executing each in turn
	ListIterator<ProcedureNode> nodeIterator(sequence_);
	while (ProcedureNode* node = nodeIterator.iterate())
	{
		// Get the result of executing the node
		result = node->execute(procPool, cfg, prefix, targetList);

		// If the result is not Success, don't process any more nodes
		if (result != ProcedureNode::Success) break;
	}

	return result;
}

// Finalise any necessary data after execution
bool SequenceProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Loop over nodes in the list, finalising each in turn
	ListIterator<ProcedureNode> nodeIterator(sequence_);
	while (ProcedureNode* node = nodeIterator.iterate()) if (!node->finalise(procPool, cfg, prefix, targetList)) return false;

	return true;
}

/*
 * Read / Write
 */

// Set block termination keyword for current context when reading
void SequenceProcedureNode::setBlockTerminationKeyword(const char* endKeyword)
{
	blockTerminationKeyword_ = endKeyword;
}

// Return block termination keyword for current context
const char* SequenceProcedureNode::blockTerminationKeyword() const
{
	return blockTerminationKeyword_.get();
}

// Read structure from specified LineParser
bool SequenceProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The sequence node now pushes a new scope (mirroring our own context)
	scopeStack.push(context_);

	// Read until we encounter the block-ending keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Is the first argument the block termination keyword for the current context?
		if (DissolveSys::sameString(parser.argc(0), blockTerminationKeyword_)) break;

		// Is the first argument on the current line a valid control keyword?
		SequenceNodeKeyword nk = sequenceNodeKeywords().enumeration(parser.argc(0));
		switch (nk)
		{
			case (SequenceProcedureNode::nSequenceNodeKeywords):
				break;
		}

		// Not a control keyword, so must be a node type
		ProcedureNode* newNode = NULL;
		ProcedureNode::NodeType nt = ProcedureNode::nodeTypes().enumeration(parser.argc(0));
		switch (nt)
		{
			case (ProcedureNode::AddSpeciesNode):
				newNode = new AddSpeciesProcedureNode();
				break;
			case (ProcedureNode::BoxNode):
				newNode = new BoxProcedureNode();
				break;
			case (ProcedureNode::CalculateNode):
				newNode = new CalculateProcedureNode();
				break;
			case (ProcedureNode::Collect1DNode):
				newNode = new Collect1DProcedureNode();
				break;
			case (ProcedureNode::Collect2DNode):
				newNode = new Collect2DProcedureNode();
				break;
			case (ProcedureNode::Collect3DNode):
				newNode = new Collect3DProcedureNode();
				break;
			case (ProcedureNode::ExcludeNode):
				newNode = new ExcludeProcedureNode();
				break;
			case (ProcedureNode::Fit1DNode):
				newNode = new Fit1DProcedureNode();
				break;
			case (ProcedureNode::ParametersNode):
				newNode = new ParametersProcedureNode();
				break;
			case (ProcedureNode::Process1DNode):
				newNode = new Process1DProcedureNode();
				break;
			case (ProcedureNode::Process2DNode):
				newNode = new Process2DProcedureNode();
				break;
			case (ProcedureNode::Process3DNode):
				newNode = new Process3DProcedureNode();
				break;
			case (ProcedureNode::SelectNode):
				newNode = new SelectProcedureNode();
				break;
			case (ProcedureNode::SequenceNode):
				/* This should never be called */
				newNode = new SequenceProcedureNode(ProcedureNode::NoContext);
				break;
			case (ProcedureNode::nNodeTypes):
				return Messenger::error("Unrecognised procedure node type '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to create a node of type '%s'.\n", parser.argc(0));
		}

		// Is the new node permitted in the context of the current scope?
		if (!newNode->isContextRelevant(scopeStack.currentContext()))
		{
			return Messenger::error("'%s' node not allowed / relevant in '%s' context.\n", ProcedureNode::nodeTypes().keyword(newNode->type()), ProcedureNode::nodeContexts().keyword(context_));
		}

		// Add the new node to our list
		sequence_.own(newNode);

		// Read the new node
		if (!newNode->read(parser, coreData, scopeStack)) return Messenger::error("Failed to read node sequence.\n");
	}

	// Remove our context, since it is now 'out-of-scope'
	if (!scopeStack.pop()) return Messenger::error("Internal error while removing context layer for SequenceProcedureNode.\n");

	return true;
}

// Write structure to specified LineParser
bool SequenceProcedureNode::write(LineParser& parser, const char* prefix)
{
	// Block Start - should have already been written by the calling function, since we don't know the keyword we are linked to

	// Loop over nodes in this sequence
	ListIterator<ProcedureNode> nodeIterator(sequence_);
	while (ProcedureNode* node = nodeIterator.iterate()) if (!node->write(parser, prefix)) return false;

	// Block End
	if (!parser.writeLineF("%s%s\n", blockTerminationKeyword_.get())) return false;

	return true;
}
