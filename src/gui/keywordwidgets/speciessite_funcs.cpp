/*
    *** Keyword Widget - SpeciesSite RefList
    *** src/gui/keywordwidgets/speciessite_funcs.cpp
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

#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/keywordwidgets/speciessite.h"
#include "templates/variantpointer.h"
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpacerItem>

SpeciesSiteKeywordWidget::SpeciesSiteKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<SpeciesSiteKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into SpeciesSiteKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateWidgetValues(coreData_);
    }
}

/*
 * Widgets
 */
void SpeciesSiteKeywordWidget::siteRadioButton_clicked(bool checked)
{
    if (refreshing_)
        return;

    QRadioButton *radioButton = qobject_cast<QRadioButton *>(sender());
    if (!radioButton)
        return;

    // Retrieve the SpeciesSite from the radioButton
    SpeciesSite *site = VariantPointer<SpeciesSite>(radioButton->property("SpeciesSite"));
    if (!site)
        return;

    keyword_->data() = site;
    keyword_->hasBeenSet();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesSiteKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void SpeciesSiteKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Clear all tabs from our tab widget
    ui_.SpeciesTabs->clear();

    // Create button group for the radio buttons
    auto *buttonGroup = new QButtonGroup(this);

    // Add new tabs in, one for each defined Species, and each containing checkboxes for each available site
    ListIterator<Species> speciesIterator(coreData_.constSpecies());
    while (auto *sp = speciesIterator.iterate())
    {
        // Create the widget to hold our checkboxes for this Species
        auto *widget = new QWidget();
        auto *layout = new QVBoxLayout;
        layout->setContentsMargins(4, 4, 4, 4);
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
            while (auto *site = siteIterator.iterate())
            {
                auto *radioButton = new QRadioButton(QString::fromStdString(std::string(site->name())));
                if (keyword_->data() == site)
                    radioButton->setChecked(true);
                connect(radioButton, SIGNAL(clicked(bool)), this, SLOT(siteRadioButton_clicked(bool)));
                radioButton->setProperty("SpeciesSite", VariantPointer<SpeciesSite>(site));
                layout->addWidget(radioButton);
                buttonGroup->addButton(radioButton);

                // If this keyword demands oriented sites, disable the radio button if the site has no axes
                if (keyword_->axesRequired() && (!site->hasAxes()))
                    radioButton->setDisabled(true);
            }

            // Add on a vertical spacer to take up any extra space at the foot of the widget
            layout->addSpacing(0);
        }

        // Create the page in the tabs
        ui_.SpeciesTabs->addTab(widget, QString::fromStdString(std::string(sp->name())));
    }

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void SpeciesSiteKeywordWidget::updateKeywordData()
{
    // Not relevant - Handled via checkbox callbacks
}

// Update summary text
void SpeciesSiteKeywordWidget::updateSummaryText()
{
    if (keyword_->data())
        setSummaryText(
            QString::fromStdString(fmt::format("{} ({})", keyword_->data()->name(), keyword_->data()->parent()->name())));
    else
        setSummaryText("<None>");
}
