// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/dialog.h"
#include "gui/keywordWidgets/fileAndFormat.h"
#include "io/fileAndFormat.h"
#include "main/dissolve.h"
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>

FileAndFormatKeywordWidget::FileAndFormatKeywordWidget(QWidget *parent, FileAndFormatKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    ui_.setupUi(this);

    enumOptionsModel_.setData(keyword_->data().formats());

    refreshing_ = true;

    // Populate combo with the file formats available
    ui_.FormatCombo->clear();
    ui_.FormatCombo->setModel(&enumOptionsModel_);

    // If the FileAndFormat has keyword options, enable the options button.
    ui_.OptionsButton->setEnabled(keyword_->hasOptions());

    // Set current information
    updateWidgetValues(coreData_);

    refreshing_ = false;
}

/*
 * Widgets
 */

void FileAndFormatKeywordWidget::on_FileEdit_editingFinished()
{
    if (refreshing_)
        return;

    updateKeywordData();

    checkFileValidity();

    emit(keywordDataChanged(keyword_->editSignals()));
}

void FileAndFormatKeywordWidget::on_FileEdit_returnPressed()
{
    if (refreshing_)
        return;

    updateKeywordData();

    checkFileValidity();

    emit(keywordDataChanged(keyword_->editSignals()));
}

void FileAndFormatKeywordWidget::on_FileSelectButton_clicked(bool checked)
{
    // Grab the target FileAndFormat
    auto &fileAndFormat = keyword_->data();

    // Determine what sort of dialog we need to raise...
    QString filename;
    QFileInfo fileInfo(QString::fromStdString(std::string(fileAndFormat.filename())));
    if (fileAndFormat.fileMustExist())
    {
        // Input File
        filename =
            QFileDialog::getOpenFileName(this, tr("Select Input File"), fileInfo.absoluteFilePath(), tr("All Files (*)"));
    }
    else
    {
        // Output File
        filename =
            QFileDialog::getSaveFileName(this, tr("Select Output File"), fileInfo.absoluteFilePath(), tr("All Files (*)"));
    }

    if (!filename.isEmpty())
    {
        // Set relative path to file, using the current input file as the reference point
        QFileInfo fileInfo(QString::fromStdString(std::string(coreData_.inputFilename())));
        ui_.FileEdit->setText(fileInfo.dir().relativeFilePath(filename));
        updateKeywordData();
        updateWidgetValues(coreData_);
        emit(keywordDataChanged(keyword_->editSignals()));
    }
}

void FileAndFormatKeywordWidget::on_FormatCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateKeywordData();

    emit(keywordDataChanged(keyword_->editSignals()));
}

void FileAndFormatKeywordWidget::on_OptionsButton_clicked(bool checked)
{
    KeywordsDialog optionsDialog(this, keyword_->data().keywords(), coreData_);

    optionsDialog.showOptions();

    if (optionsDialog.keywordsModified())
        emit(keywordDataChanged(optionsDialog.keywordSignalMask() + keyword_->editSignals()));
}

/*
 * Update
 */

// Check / set validity of current filename
void FileAndFormatKeywordWidget::checkFileValidity()
{
    // Grab the target FileAndFormat
    const auto &fileAndFormat = keyword_->data();

    // If this is an export FileAndFormat, no need to show the indicator or check if the file exists
    if (fileAndFormat.fileMustExist())
    {
        ui_.FileExistsIndicator->setVisible(true);
        auto ok = fileAndFormat.hasFilename() ? DissolveSys::fileExists(fileAndFormat.filename()) : false;
        ui_.FileExistsIndicator->setOK(ok);
    }
    else
        ui_.FileExistsIndicator->setVisible(false);
}

// Update value displayed in widget
void FileAndFormatKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void FileAndFormatKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Grab the target FileAndFormat
    auto &fileAndFormat = keyword_->data();

    // Update widgets
    ui_.FileEdit->setText(QString::fromStdString(std::string(fileAndFormat.filename())));
    ui_.FormatCombo->setCurrentIndex(fileAndFormat.formatIndex());
    checkFileValidity();

    refreshing_ = false;
}

// Update keyword data based on widget values
void FileAndFormatKeywordWidget::updateKeywordData()
{
    // Grab the target FileAndFormat
    auto &fileAndFormat = keyword_->data();

    fileAndFormat.setFilename(qPrintable(ui_.FileEdit->text()));
    fileAndFormat.setFormatByIndex(ui_.FormatCombo->currentIndex());
}
