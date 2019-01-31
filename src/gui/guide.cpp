/*
	*** Guide for GuideWizard
	*** src/gui/guide.cpp
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

#include "gui/guide.h"
#include "base/sysfunc.h"
#include <QFile>
#include <QXmlStreamReader>

// Constructor / Destructor
Guide::Guide() : ListItem<Guide>()
{
	valid_ = false;
}

Guide::~Guide()
{
}

// Clear all data
void Guide::clear()
{
	valid_ = false;
	name_.clear();
	iconUrl_.clear();
	pages_.clear();
}

/*
 * Description
 */

// Return name of the guide
const char* Guide::name() const
{
	return name_.get();
}

// Return resource location for icon
const char* Guide::iconUrl() const
{
	return iconUrl_.get();
}

// Return whether an icon url has been set
bool Guide::hasIconUrl() const
{
	return (!iconUrl_.isEmpty());
}

// Return whether the guide is valid
bool Guide::isValid() const
{
	return valid_;
}

/*
 * Page Data
 */

// Return index of page with supplied name
int Guide::indexOfPage(const char* name) const
{
	ListIterator<GuidePage> pageIterator(pages_);
	int count = 0;
	while (GuidePage* page = pageIterator.iterate())
	{
		if (DissolveSys::sameString(name, page->name())) return count;
		++count;
	}

	return -1;
}

// Return GuidePage with name specified
GuidePage* Guide::page(const char* name)
{
	ListIterator<GuidePage> pageIterator(pages_);
	while (GuidePage* page = pageIterator.iterate()) if (DissolveSys::sameString(name, page->name())) return page;

	return NULL;
}

// Return name of intended start page
const char* Guide::startPageName() const
{
	return startPageName_.get();
}

// Return list of defined pages
const List<GuidePage>& Guide::pages() const
{
	return pages_;
}

/*
 * I/O
 */

// Resolve internal links between pages, converting tags to pointers
void Guide::resolveInternalLinks()
{
	// Set starting page
	if (!startPageName_.isEmpty())
	{
		startPage_ = page(startPageName_);
		if (!startPage_) Messenger::warn("No page named '%s' exists, so the starting page pointer cannot be set.\n", startPageName_.get());
	}
	else
	{
		startPage_ = NULL;
		Messenger::warn("No starting page has been set for this guide.\n");
	}

	// Loop over pages, setting pointers to linked pages based on the stored tags
	ListIterator<GuidePage> pageIterator(pages_);
	while (GuidePage* guidePage = pageIterator.iterate())
	{
		// Next page pointer
		if (guidePage->hasNextPageName())
		{
			// Locate the next page in our list
			GuidePage* nextPage = page(guidePage->nextPageName());
			if (!nextPage) Messenger::warn("No page named '%s' exists, so the next page pointer for page '%s' cannot be set.\n", guidePage->nextPageName(), guidePage->name());
			guidePage->setNextPage(nextPage);
		}
	}
}

// Load page data from specified resource
bool Guide::load(const char* resourceName)
{
	// Set up an XML stream reader on the supplied resource
	QFile guideFile(resourceName);
	if (!guideFile.open(QIODevice::ReadOnly | QIODevice::Text)) return Messenger::error("Couldn't open guide '%s'.\n", resourceName);
	QXmlStreamReader guideReader(&guideFile);

	// Check that we have a file with the correct root node
	if (!guideReader.readNextStartElement()) return Messenger::error("Failed to do initial read from guide xml.\n");
	if (guideReader.name() != "guide") return Messenger::error("Guide file has wrong root node type.\n");

	// Now loop over root node elements
	while (guideReader.readNextStartElement())
	{
		// Check the name of the root element
		QString token = guideReader.name().toString();

		if (token == "name") name_ = qPrintable(guideReader.readElementText());
		else if (token == "icon") iconUrl_= qPrintable(guideReader.readElementText());
		else if (token == "start") startPageName_ = qPrintable(guideReader.readElementText());
		else if (token == "pages")
		{
			// Start of the pages set
			while (guideReader.readNextStartElement())
			{
				// Check that this is a <page>...
				if (guideReader.name() != "page")
				{
					Messenger::error("Encountered unrecognised token '%s' when we expected a <page>.\n", qPrintable(guideReader.name().toString()));
					guideReader.skipCurrentElement();
					continue;
				}

				// Create a new GuidePage object
				GuidePage* page = pages_.add();

				// Process the page's content
				while (guideReader.readNextStartElement())
				{
					QString pageToken = guideReader.name().toString();

					if (pageToken == "name") page->setName(qPrintable(guideReader.readElementText()));
					else if (pageToken == "next") page->setNextPageName(qPrintable(guideReader.readElementText()));
					else if (pageToken == "content") page->setRichTextContent(qPrintable(guideReader.readElementText()));
					else if (pageToken == "highlight")
					{
						GuidePageHighlight* highlight = page->addHighlight();
						int r = 0, g = 0, b = 0, a = 255;
						while (guideReader.readNextStartElement())
						{
							QString name = guideReader.name().toString();

							if (name == "widget") highlight->setWidgetName(qPrintable(guideReader.readElementText()));
							else if (name == "r") r = guideReader.readElementText().toInt();
							else if (name == "g") g = guideReader.readElementText().toInt();
							else if (name == "b") b = guideReader.readElementText().toInt();
							else if (name == "a") a = guideReader.readElementText().toInt();
						}
						highlight->setColour(r, g, b, a);
					}
					else
					{
						Messenger::error("Unrecognised token '%s' found in page definition.\n", qPrintable(pageToken));
						return false;
					}
				}

				Messenger::printVerbose("Found page '%s'\n", page->name());
			}
		}
		else
		{
			Messenger::error("Unrecognised token '%s' found in guide definition.\n", qPrintable(token));
			return false;
		}
	}

	// Now we have all the pages loaded, we can resolve internal links between them
	resolveInternalLinks();

	return true;
}

// Save guide data to specified resource
bool Guide::save(const char* resourceName)
{
	// Set up an XML stream writer on the supplied filename
	QFile guideFile(resourceName);
	if (!guideFile.open(QIODevice::WriteOnly | QIODevice::Text)) return Messenger::error("Couldn't open guide '%s' for writing.\n", resourceName);
	QXmlStreamWriter guideWriter(&guideFile);

	// Set up the writer
	guideWriter.setAutoFormatting(true);
	guideWriter.writeStartDocument();

	// Write opening element
	guideWriter.writeStartElement("guide");

	// Write guide information
	guideWriter.writeTextElement("name", name_.get());
	if (!iconUrl_.isEmpty()) guideWriter.writeTextElement("icon", iconUrl_.get());
	if (startPage_) guideWriter.writeTextElement("start", startPage_->name());

	// Write pages
	guideWriter.writeStartElement("pages");
	ListIterator<GuidePage> pageIterator(pages_);
	while (GuidePage* page = pageIterator.iterate())
	{
		// Open the page element
		guideWriter.writeStartElement("page");

		guideWriter.writeTextElement("name", page->name());
		if (page->nextPage()) guideWriter.writeTextElement("next", page->nextPage()->name());

		// Write rich text content
		guideWriter.writeStartElement("content");
		guideWriter.writeCDATA(page->richTextContent());
		guideWriter.writeEndElement();

		// Write widget higlights
		ListIterator<GuidePageHighlight> highlightIterator(page->highlights());
		while (GuidePageHighlight* highlight = highlightIterator.iterate())
		{
			guideWriter.writeStartElement("highlight");
			guideWriter.writeTextElement("widget", highlight->widgetName());
			QColor colour = highlight->colour();
			guideWriter.writeTextElement("r", QString::number(colour.red()));
			guideWriter.writeTextElement("g", QString::number(colour.green()));
			guideWriter.writeTextElement("b", QString::number(colour.blue()));
			guideWriter.writeTextElement("a", QString::number(colour.alpha()));
			guideWriter.writeEndElement();
		}

		// Close the page element
		guideWriter.writeEndElement();
	}
	guideWriter.writeEndElement();

	// Write ending element for the entire guide
	guideWriter.writeEndElement();

	guideWriter.writeEndDocument();

	return true;
}
