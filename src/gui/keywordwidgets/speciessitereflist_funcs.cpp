// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "gui/keywordwidgets/speciessitereflist.h"
#include "templates/variantpointer.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>

SpeciesSiteRefListKeywordWidget::SpeciesSiteRefListKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                 const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<SpeciesSiteRefListKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into SpeciesSiteRefListKeyword.\n", keyword->name());
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
    if (refreshing_)
        return;

    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    if (!checkBox)
        return;

    // Retrieve the SpeciesSite from the checkBox
    SpeciesSite *site = VariantPointer<SpeciesSite>(checkBox->property("SpeciesSite"));
    if (!site)
        return;

    // If the box is checked, we need to add the site to the list. If not, remove it.
    if (checked)
        keyword_->data().addUnique(site);
    else
        keyword_->data().remove(site);

    keyword_->hasBeenSet();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesSiteRefListKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void SpeciesSiteRefListKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Clear all tabs from our tab widget
    ui_.SpeciesTabs->clear();

    // Add new tabs in, one for each defined Species, and each containing checkboxes for each available site
    for (const auto &sp : coreData_.species())
    {
        // Create the widget to hold our checkboxes for this Species
        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout;
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
            for (auto &site : sp->sites())
            {
                QCheckBox *checkBox = new QCheckBox(QString::fromStdString(std::string(site.name())));
                if (keyword_->data().contains(&site))
                    checkBox->setChecked(true);
                connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(siteCheckBox_clicked(bool)));
                checkBox->setProperty("SpeciesSite", VariantPointer<SpeciesSite>(&site));
                layout->addWidget(checkBox);

                // If this keyword demands oriented sites, disable the radio button if the site has no axes
                if (keyword_->axesRequired() && (!site.hasAxes()))
                    checkBox->setDisabled(true);
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
void SpeciesSiteRefListKeywordWidget::updateKeywordData()
{
    // Not relevant - Handled via checkbox callbacks
}

// Update summary text
void SpeciesSiteRefListKeywordWidget::updateSummaryText()
{
    QString siteText;
    if (keyword_->data().nItems() == 0)
        siteText = "<None>";
    else
    {
        auto first = true;
        for (SpeciesSite *site : keyword_->data())
        {
            if (!first)
                siteText += ", ";
            siteText += QString::fromStdString(fmt::format("{} ({})", site->name(), site->parent()->name()));
            first = false;
        }
    }

    setSummaryText(siteText);
}
