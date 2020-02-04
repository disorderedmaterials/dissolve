/*
	*** Keyword Widget - SpeciesSite RefList
	*** src/gui/keywordwidgets/speciessitereflist_funcs.cpp
	Copyright T. Youngs 2012-2020

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

#include "gui/keywordwidgets/speciessitereflist.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "templates/variantpointer.h"
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSpacerItem>

// Constructor
SpeciesSiteRefListKeywordWidget::SpeciesSiteRefListKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData) : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui_.setupUi(dropWidget());

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<SpeciesSiteRefListKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base keyword '%s' into SpeciesSiteRefListKeyword.\n", keyword->name());
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}

	// Summary text on KeywordDropDown button
	setSummaryText("<None>");
}

/*
 * Widgets
 */
void SpeciesSiteRefListKeywordWidget::siteCheckBox_clicked(bool checked)
{
	if (refreshing_) return;

	QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
	if (!checkBox) return;

	// Retrieve the SpeciesSite from the checkBox
	SpeciesSite* site = VariantPointer<SpeciesSite>(checkBox->property("SpeciesSite"));
	if (!site) return;

	// If the box is checked, we need to add the site to the list. If not, remove it.
	if (checked) keyword_->data().addUnique(site);
	else keyword_->data().remove(site);

	keyword_->hasBeenSet();

	updateSummaryText();

	emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesSiteRefListKeywordWidget::updateValue()
{
	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void SpeciesSiteRefListKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Clear all tabs from our tab widget
	ui_.SpeciesTabs->clear();

	// Add new tabs in, one for each defined Species, and each containing checkboxes for each available site
	ListIterator<Species> speciesIterator(coreData_.constSpecies());
	while (Species* sp = speciesIterator.iterate())
	{
		// Create the widget to hold our checkboxes for this Species
		QWidget* widget = new QWidget();
		QVBoxLayout* layout = new QVBoxLayout;
		layout->setContentsMargins(4,4,4,4);
		layout->setSpacing(4);
		widget->setLayout(layout);
		widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

		// Are there sites defined?
		if (sp->nSites() == 0)
		{
			layout->addWidget(new QLabel("No sites defined."));
		}
		else
		{
			// Loop over sites defined in this Species
			ListIterator<SpeciesSite> siteIterator(sp->sites());
			while (SpeciesSite* site = siteIterator.iterate())
			{
				QCheckBox* checkBox = new QCheckBox(site->name());
				if (keyword_->data().contains(site)) checkBox->setChecked(true);
				connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(siteCheckBox_clicked(bool)));
				checkBox->setProperty("SpeciesSite", VariantPointer<SpeciesSite>(site));
				layout->addWidget(checkBox);

				// If this keyword demands oriented sites, disable the radio button if the site has no axes
				if (keyword_->axesRequired() && (!site->hasAxes())) checkBox->setDisabled(true);
			}

			// Add on a vertical spacer to take up any extra space at the foot of the widget
			layout->addSpacing(0);
		}

		// Create the page in the tabs
		ui_.SpeciesTabs->addTab(widget, sp->name());
	}

	updateSummaryText();

	refreshing_ = false;
}

// Update keyword data based on widget values
void SpeciesSiteRefListKeywordWidget::updateKeywordData()
{
	// Not relevant - Handled via checkbox callbacks
}

// Update summary text
void SpeciesSiteRefListKeywordWidget::updateSummaryText()
{
	CharString siteText;
	if (keyword_->data().nItems() == 0) siteText = "<None>";
	else
	{
		bool first = true;
		for (SpeciesSite* site : keyword_->data())
		{
			if (first) siteText.strcatf("%s (%s)", site->name(), site->parent()->name());
			else siteText.strcatf(", %s (%s)", site->name(), site->parent()->name());
			first = false;
		}
	}

	setSummaryText(siteText);
}
