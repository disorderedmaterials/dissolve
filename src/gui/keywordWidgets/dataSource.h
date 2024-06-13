// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/fileAndFormat.h"
#include "gui/keywordWidgets/enumOptions.h"
#include "gui/models/enumOptionsModel.h"
#include "keywords/fileAndFormat.h"
#include "io/import/data1D.h"
#include "math/data1D.h"
#include "keywords/dataSource.h"
#include <QVBoxLayout>
#include <QWidget>

class DataSourceKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    DataSourceKeywordWidget(QWidget *parent, DataSourceKeyword<Data1D> *keyword, CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    DataSourceKeyword<Data1D> *keyword_;

    protected:
    // mutable coredata source
    CoreData &coreData_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    QWidget ui_;
    QVBoxLayout internalLayout_;
    QVBoxLayout externalLayout_;
    DataSource<Data1D> dataSource_;

    // Model for enum options
    EnumOptionsModel enumOptionsModel_;

    private Q_SLOTS:

    Q_SIGNALS:
    // Keyword data changed
    //void keywordDataChanged(int flags);

    /*
     * Update
     */

    public:
    // Update value displayed in widget
    //void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update widget values data based on keyword data
    //void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values
    //void updateKeywordData();
};
