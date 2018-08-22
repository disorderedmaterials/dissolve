/*
	*** Analysis Node - Node Sequence
	*** src/analyse/nodes/sequence.cpp
	Copyright T. Youngs 2012-2018

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

#include "analyse/nodes/sequence.h"
#include "analyse/nodes/nodes.h"
#include "analyse/sitecontextstack.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisSequenceNode::AnalysisSequenceNode(const char* blockTerminationKeyword) : AnalysisNode()
{
	blockTerminationKeyword_ = blockTerminationKeyword;

	type_ = AnalysisNode::SequenceNode;
}

// Destructor
AnalysisSequenceNode::~AnalysisSequenceNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* SequenceNodeKeywords[] = { "_DUMMY_" };

// Convert string to node keyword
AnalysisSequenceNode::SequenceNodeKeyword AnalysisSequenceNode::sequenceNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisSequenceNode::nSequenceNodeKeywords; ++nk) if (DissolveSys::sameString(s, SequenceNodeKeywords[nk])) return (AnalysisSequenceNode::SequenceNodeKeyword) nk;

	return AnalysisSequenceNode::nSequenceNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisSequenceNode::sequenceNodeKeyword(AnalysisSequenceNode::SequenceNodeKeyword nk)
{
	return SequenceNodeKeywords[nk];
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisSequenceNode::execute(Configuration* cfg)
{
	AnalysisNode::NodeExecutionResult result = AnalysisNode::Success;

	// Loop over nodes in the list, executing each in turn
	ListIterator<AnalysisNode> nodeIterator(sequence_);
	while (AnalysisNode* node = nodeIterator.iterate())
	{
		// Get the result of executing the node
		result = node->execute(cfg);

		// If the result is not Success, don't process any more nodes
		if (result != AnalysisNode::Success) break;
	}

	return result;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisSequenceNode::read(LineParser& parser, SiteContextStack& contextStack)
{
	// The sequence node constructs a new context...
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
			case (SequenceNodeKeyword::nSequenceNodeKeywords):
				break;
		}

		// Not a control keyword, so must be a node type
		AnalysisNode* newNode = NULL;
		AnalysisNode::NodeType nt = AnalysisNode::nodeType(parser.argc(0));
		switch (nt)
		{
			case (AnalysisNode::CollectNode):
				newNode = new AnalysisCollectNode;
				break;
			case (AnalysisNode::ExcludeNode):
				newNode = new AnalysisExcludeNode;
				break;
			case (AnalysisNode::SelectNode):
				newNode = new AnalysisSelectNode;
				break;
			case (AnalysisNode::SequenceNode):
				/* This should never be called */
				newNode = new AnalysisSequenceNode;
				break;
			case (AnalysisNode::nNodeTypes):
				return Messenger::error("Unrecognised analysis node type '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to create a node of type '%s'.\n", parser.argc(0));
		}

		// Add the new node to our list
		sequence_.own(newNode);

		// Read the new node
		if (!newNode->read(parser, contextStack)) return Messenger::error("Failed to read analysis sequence.\n");
	}

	// Remove our context, since it is now 'out-of-scope'
	if (!contextStack.pop()) return Messenger::error("Internal error while removing context layer for AnalysisSequenceNode.\n");

	return true;
}

// Write structure to specified LineParser
bool AnalysisSequenceNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
