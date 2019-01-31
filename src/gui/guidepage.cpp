/*
	*** Guide Page
	*** src/gui/guidepage.cpp
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

#include "gui/guidepage.h"

// Constructor / Destructor
GuidePage::GuidePage() : ListItem<GuidePage>()
{
}

GuidePage::~GuidePage()
{
}

/*
 * Basic Definition
 */


// Set page title
void GuidePage::setTitle(const char* title)
{
	title_ = title;
}
// Return title text for page
const char* GuidePage::title() const
{
	return title_.get();
}

// Set resource location for icon
void GuidePage::setIconUrl(const char* resourceUrl)
{
	iconUrl_ = resourceUrl;
}

// Return resource location for icon
const char* GuidePage::iconUrl() const
{
	return iconUrl_.get();
}

// Return whether an icon url has been set
bool GuidePage::hasIconUrl() const
{
	return (!iconUrl_.isEmpty());
}

// Set whether this page is a Finish point
void GuidePage::setFinishPoint(bool b)
{
	finishPoint_ = b;
}

// Whether this page is a Finish point
bool GuidePage::finishPoint() const
{
	return finishPoint_;
}

// Set tag for this page
void GuidePage::setTag(const char* tag)
{
	tag_ = tag;
}

// Return tag for this page
const char* GuidePage::tag() const
{
	return tag_.get();
}

// Set tag for the page following this one
void GuidePage::setNextPageTag(const char* nextTag)
{
	nextPageTag_ = nextTag;
}

// Return tag for the page following this one
const char* GuidePage::nextPageTag() const
{
	return nextPageTag_.get();
}

// Return whether a next page tag has been set
bool GuidePage::hasNextPageTag() const
{
	return (!nextPageTag_.isEmpty());
}

/*
 * Page Content
 */

// Set rich text content for the page
void GuidePage::setRichTextContent(QString richText)
{
	richTextContent_ = richText;
}

// Return rich text content for the page
QString GuidePage::richTextContent() const
{
	return richTextContent_;
}

/*
 * Interaction
 */

// Add new widget highlight
GuidePageHighlight* GuidePage::addHighlight()
{
	return highlights_.add();
}

// Return highlighted widgets list
const List<GuidePageHighlight>& GuidePage::highlights() const
{
	return highlights_;
}
