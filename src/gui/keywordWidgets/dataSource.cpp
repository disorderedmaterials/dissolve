// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/dataSource.h"
#include "gui/keywordWidgets/dialog.h"
#include "io/fileAndFormat.h"
#include "main/dissolve.h"
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>

DataSourceKeywordWidget::DataSourceKeywordWidget(QWidget *parent, DataSourceKeyword<Data1D> *keyword, CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword), coreData_(coreData)
{
    // Add QWidget with Box Layout
    enumOptionsModel_.setData(dataSource_.dataSourceTypes());
    FileAndFormatKeyword k(dataSource_.externalDataSource(), "end");
    FileAndFormatKeywordWidget fileAndFormat(&ui_, &k, coreData);

    if (dataSource_.dataName() == "External")
    {
        externalLayout_.addWidget(&fileAndFormat);
        ui_.setLayout(&externalLayout_);
    }
    else if (dataSource_.dataName() == "Internal")
    {
    }
}