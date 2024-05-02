// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_fileAndFormat.h"
#include "gui/models/enumOptionsModel.h"
#include "keywords/fileAndFormat.h"
#include <QWidget>

class FileAndFormatKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    FileAndFormatKeywordWidget(QWidget *parent, FileAndFormatKeyword *keyword, CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    FileAndFormatKeyword *keyword_;

    protected:
    // mutable coredata source
    CoreData &coreData_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::FileAndFormatWidget ui_;
    // Model for enum options
    EnumOptionsModel enumOptionsModel_;

    private Q_SLOTS:
    void on_FileEdit_editingFinished();
    void on_FileEdit_returnPressed();
    void on_FormatCombo_currentIndexChanged(int index);
    void on_FileSelectButton_clicked(bool checked);
    void on_OptionsButton_clicked(bool checked);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Check / set validity of current filename
    void checkFileValidity();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values
    void updateKeywordData();
};
