/*
	*** Procedure Node - Node Sequence
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
#include "procedure/nodecontextstack.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
SequenceProcedureNode::SequenceProcedureNode(const char* blockTerminationKeyword) : ProcedureNode(ProcedureNode::SequenceNode)
{
	blockTerminationKeyword_ = blockTerminationKeyword;
}

// Destructor
SequenceProcedureNode::~SequenceProcedureNode()
{
	clear();
}

/*
 * Node Keywords
 */

// Node Keywords
const char* SequenceNodeKeywords[] = { "_DUMMY_" };

// Convert string to node keyword
SequenceProcedureNode::SequenceNodeKeyword SequenceProcedureNode::sequenceNodeKeyword(const char* s)
{
	for (int nk=0; nk < SequenceProcedureNode::nSequenceNodeKeywords; ++nk) if (DissolveSys::sameString(s, SequenceNodeKeywords[nk])) return (SequenceProcedureNode::SequenceNodeKeyword) nk;

	return SequenceProcedureNode::nSequenceNodeKeywords;
}

// Convert node keyword to string
const char* SequenceProcedureNode::sequenceNodeKeyword(SequenceProcedureNode::SequenceNodeKeyword nk)
{
	return SequenceNodeKeywords[nk];
}

/*
 * Node Contents
 */

// Clear all data
void SequenceProcedureNode::clear()
{
	sequence_.clear();
}

// Add (own) node into sequence
void SequenceProcedureNode::addNode(ProcedureNode* node)
{
	sequence_.own(node);

	// Pass on our Procedure parent
	node->setProcedure(procedure());
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

// Read structure from specified LineParser
bool SequenceProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack)
{
	// The sequence node now constructs a new context...
	contextStack.push();

	// Read until we encounter the block-ending keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument the block termination keyword for the current context?
		if (DissolveSys::sameString(parser.argc(0), blockTerminationKeyword_)) break;

		// Is the first argument on the current line a valid control keyword?
		SequenceNodeKeyword nk = sequenceNodeKeyword(parser.argc(0));
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
			case (ProcedureNode::CalculateNode):
				newNode = new CalculateProcedureNode;
				break;
			case (ProcedureNode::Collect1DNode):
				newNode = new Collect1DProcedureNode;
				break;
			case (ProcedureNode::Collect2DNode):
				newNode = new Collect2DProcedureNode;
				break;
			case (ProcedureNode::Collect3DNode):
				newNode = new Collect3DProcedureNode;
				break;
			case (ProcedureNode::ExcludeNode):
				newNode = new ExcludeProcedureNode;
				break;
			case (ProcedureNode::Fit1DNode):
				newNode = new Fit1DProcedureNode;
				break;
			case (ProcedureNode::Process1DNode):
				newNode = new Process1DProcedureNode;
				break;
			case (ProcedureNode::Process2DNode):
				newNode = new Process2DProcedureNode;
				break;
			case (ProcedureNode::Process3DNode):
				newNode = new Process3DProcedureNode;
				break;
			case (ProcedureNode::SelectNode):
				newNode = new SelectProcedureNode;
				break;
			case (ProcedureNode::SequenceNode):
				/* This should never be called */
				newNode = new SequenceProcedureNode;
				break;
			case (ProcedureNode::nNodeTypes):
				return Messenger::error("Unrecognised analysis node type '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to create a node of type '%s'.\n", parser.argc(0));
		}

		// Add the new node to our list
		sequence_.own(newNode);

		// Set the Procedure parent of the new node to be the same as ours
		newNode->setProcedure(procedure());

		// Read the new node
		if (!newNode->read(parser, coreData, contextStack)) return Messenger::error("Failed to read analysis sequence.\n");
	}

	// Remove our context, since it is now 'out-of-scope'
	if (!contextStack.pop()) return Messenger::error("Internal error while removing context layer for SequenceProcedureNode.\n");

	return true;
}

// Write structure to specified LineParser
bool SequenceProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
