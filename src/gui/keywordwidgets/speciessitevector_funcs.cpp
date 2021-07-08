// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "gui/keywordwidgets/speciessitevector.h"
#include "templates/algorithms.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QSpacerItem>

SpeciesSiteVectorKeywordWidget::SpeciesSiteVectorKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<SpeciesSiteVectorKeyword *>(keyword);
    if (!keyword_)
        throw(std::runtime_error(
            fmt::format("Couldn't cast base keyword '{}' into SpeciesSiteVectorKeyword.\n", keyword->name())));

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
            layout->addWidget(new QLabel("No sites defined."));
        else
        {
            auto *list = new QListView;

            // Create model for the sites and an accompanying filter proxy if oriented sites are required
            auto *model = models_.emplace_back(new SpeciesSiteModel(sp->sites(), keyword_->data()));
            if (keyword_->axesRequired())
            {
                auto *proxy = proxyFilters_.emplace_back(new SpeciesSiteFilterProxy(SpeciesSiteFilterProxy::IsOriented));
                proxy->setSourceModel(model);
                list->setModel(proxy);
            }
            else
                list->setModel(model);

            // Connect signals / slots
            connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
                    SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

            layout->addWidget(list);
            //            // Add on a vertical spacer to take up any extra space at the foot of the widget
            //            layout->addSpacing(0);
        }

        // Create the page in the tabs
        ui_.SpeciesTabs->addTab(widget, QString::fromStdString(std::string(sp->name())));
    }

    // Summary text on KeywordDropDown button
    setSummaryText("<None>");
}

SpeciesSiteVectorKeywordWidget::~SpeciesSiteVectorKeywordWidget()
{
    for (auto *proxy : proxyFilters_)
        proxy->deleteLater();
    for (auto *model : models_)
        model->deleteLater();
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
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(
            QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &site) { return site->name(); })));
}
