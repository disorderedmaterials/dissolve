// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "gui/keywordwidgets/speciessitevector.h"
#include "templates/variantpointer.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QSpacerItem>

SpeciesSiteVectorKeywordWidget::SpeciesSiteVectorKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                 const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Connect signals / slots
    connect(&siteModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<SpeciesSiteVectorKeyword *>(keyword);
    if (!keyword_)
        throw(
            std::runtime_error(fmt::format("Couldn't cast base keyword '{}' into SpeciesSiteVectorKeyword.\n", keyword->name())));
    siteModel_.setCheckStateData(keyword_->data());

    // Summary text on KeywordDropDown button
    setSummaryText("<None>");
}

/*
 * Widgets
 */

void SpeciesSiteVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    keyword_->setAsModified();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesSiteVectorKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void SpeciesSiteVectorKeywordWidget::updateWidgetValues(const CoreData &coreData)
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
//            QListView *listView = new QListView;
//            QAb
//
//            layout->addWidget()
            // Loop over sites defined in this Species
            for (auto &site : sp->sites())
            {
                QCheckBox *checkBox = new QCheckBox(QString::fromStdString(std::string(site.name())));
                if (std::find(keyword_->data().begin(), keyword_->data().end(), &site) != keyword_->data().end())
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
void SpeciesSiteVectorKeywordWidget::updateKeywordData()
{
    // Not relevant - Handled via checkbox callbacks
}

// Update summary text
void SpeciesSiteVectorKeywordWidget::updateSummaryText()
{
    QString siteText;
    if (keyword_->data().size() == 0)
        siteText = "<None>";
    else
    {
        auto first = true;
        for (auto *site : keyword_->data())
        {
            if (!first)
                siteText += ", ";
            siteText += QString::fromStdString(fmt::format("{} ({})", site->name(), site->parent()->name()));
            first = false;
        }
    }

    setSummaryText(siteText);
}
