/*
	*** General Wizard Functions
	*** src/gui/generalwizard_funcs.cpp
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

#include "gui/generalwizard.h"
#include "gui/gui.h"
#include "main/dissolve.h"
#include <QFile>
#include <QXmlStreamReader>

// Constructor / Destructor
GeneralWizard::GeneralWizard(QWidget* parent)
{
	dissolveReference_ = NULL;
	dissolveWindow_ = NULL;

	// Set up our UI, and attach the wizard's widgets to our vacant QFrame
	ui_.setupUi(this);
	setUpHeaderAndFooter(this);

	// Connect signals / slots

	lockedForRefresh_ = 0;
	startPageIndex_ = -1;
}

GeneralWizard::~GeneralWizard()
{
}

/*
 * Data
 */

// Set Dissolve reference
void GeneralWizard::setMainDissolveReference(const Dissolve* dissolveReference)
{
	dissolveReference_ = dissolveReference;
}

// Set main GUI window pointer
void GeneralWizard::setDissolveWindow(DissolveWindow* dissolveWindow)
{
	dissolveWindow_ = dissolveWindow;
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool GeneralWizard::displayControlPage(int index)
{
	// Find page with given index
	WizardWidgetPageInfo* page = findPage(index);
	if (!page) return Messenger::error("Page index %i not recognised.\n", index);

	// Page index is valid, so show the related widget
	if (!page->stackedWidgetPage()) return Messenger::error("Page index %i does not have an associated stack page widget.\n", index);
	ui_.MainStack->setCurrentWidget(page->stackedWidgetPage());

	// Highlight any widgets that need it
	page->highlightWidgets();

	return true;
}

// Return whether progression to the next page from the current page is allowed
bool GeneralWizard::progressionAllowed(int index) const
{
	return true;
}

// Perform any necssary actions before moving to the next page
bool GeneralWizard::prepareForNextPage(int currentIndex)
{
	// Find page with given index
	WizardWidgetPageInfo* page = findPage(currentIndex);
	if (!page) return Messenger::error("Page index %i not recognised.\n", currentIndex);

	// Un-highlight any widgets that were previously highlighted
	page->unHighlightWidgets();

	return true;
}

// Perform any necssary actions before moving to the previous page
bool GeneralWizard::prepareForPreviousPage(int currentIndex)
{
	// Find page with given index
	WizardWidgetPageInfo* page = findPage(currentIndex);
	if (!page) return Messenger::error("Page index %i not recognised.\n", currentIndex);

	// Un-highlight any widgets that were previously highlighted
	page->unHighlightWidgets();

	return true;
}

// Finish button clicked
void GeneralWizard::finishButtonClicked(bool checked)
{
	emit(finished());
}

/*
 * Controls
 */

// Reset widget, ready for adding a new Species
void GeneralWizard::reset()
{
	// Reset the underlying WizardWidget
	resetToPage(startPageIndex_);
}

/*
 * Content Management
 */

// Clear current content
void GeneralWizard::clear()
{
	// Remove all page widgets
	while (ui_.MainStack->count() > 0)
	{
		QWidget* page = ui_.MainStack->widget(0);
		ui_.MainStack->removeWidget(page);
		page->deleteLater();
	}

	// Clear the underlying pages
	clearPages();
}

// Set up specified content (from resource)
bool GeneralWizard::setContent(QString resourceName)
{
	// Set up an XML stream reader on the supplied resource
	QFile guideFile(resourceName);
	if (!guideFile.open(QIODevice::ReadOnly | QIODevice::Text)) return Messenger::error("Couldn't open guide '%s'.\n", qPrintable(resourceName));
	QXmlStreamReader guideReader(&guideFile);

	// Check that we have a file with the correct root node
	if (!guideReader.readNextStartElement()) return Messenger::error("Failed to do initial read from guide xml.\n");
	if (guideReader.name() != "guide") return Messenger::error("Guide file has wrong root node type.\n");

	// Now loop over root node elements
	while (guideReader.readNextStartElement())
	{
		// Check the name of the root element
		QString name = guideReader.name().toString();
		if (name == "title") Messenger::printVerbose("Guide Name: %s\n", qPrintable(guideReader.readElementText()));
		else if (name == "icon") setWizardHeaderIcon(guideReader.readElementText());
		else if (name == "start") startPageTag_ = qPrintable(guideReader.readElementText());
		else if (name == "pages")
		{
			// Start of the pages set
			while (guideReader.readNextStartElement())
			{
				// Check that this is a <page>...
				if (guideReader.name() != "page")
				{
					Messenger::error("Encountered unrecognised node '%s' when we expected a <page>.\n", qPrintable(guideReader.name().toString()));
					guideReader.skipCurrentElement();
					continue;
				}

				// Create a new page widget
				QWidget* page = new QWidget;
				QVBoxLayout* layout = new QVBoxLayout;
				layout->setContentsMargins(4,4,4,4);
				layout->setSpacing(4);
				QLabel* label = new QLabel;
				label->setWordWrap(true);
				label->setAlignment(Qt::AlignTop);
				layout->addWidget(label);
				page->setLayout(layout);
				ui_.MainStack->addWidget(page);

				// Create a pageinfo reference
				WizardWidgetPageInfo* pageInfo = addPage();
				pageInfo->setStackedWidgetPage(page);

				// Process the page's content
				while (guideReader.readNextStartElement())
				{
					QString name = guideReader.name().toString();

					if (name == "title") pageInfo->setTitle(qPrintable(guideReader.readElementText()));
					else if (name == "tag") pageInfo->setPageTag(qPrintable(guideReader.readElementText()));
					else if (name == "next") pageInfo->setNextPageTag(qPrintable(guideReader.readElementText()));
					else if (name == "content") label->setText(guideReader.readElementText());
					else if (name == "highlight")
					{
						WizardWidgetHighlight* wh = pageInfo->addWidgetHighlight();
						int r = 0, g = 0, b = 0, a = 255;
						while (guideReader.readNextStartElement())
						{
							QString name = guideReader.name().toString();

							if (name == "widget") wh->setWidget(dissolveWindow_->findChild<QWidget*>(guideReader.readElementText()));
							else if (name == "r") r = guideReader.readElementText().toInt();
							else if (name == "g") g = guideReader.readElementText().toInt();
							else if (name == "b") b = guideReader.readElementText().toInt();
							else if (name == "a") a = guideReader.readElementText().toInt();
						}
						wh->setColour(r, g, b, a);
					}
				}

				Messenger::printVerbose("Found page '%s' \n");
			}
		}
	}

	// Need to set page indices for any page that references a next page tag
	resolveTaggedPageLinks();
	startPageIndex_ = indexOfTag(startPageTag_);

	// Reset the wizard to its starting page
	reset();

	return true;
}
