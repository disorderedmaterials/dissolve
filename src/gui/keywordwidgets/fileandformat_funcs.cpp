/*
    *** Keyword Widget - FileAndFormat
    *** src/gui/keywordwidgets/fileandformat_funcs.cpp
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

#include "genericitems/listhelper.h"
#include "gui/keywordwidgets/dialog.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/fileandformat.h"
#include "io/fileandformat.h"
#include "main/dissolve.h"
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>

FileAndFormatKeywordWidget::FileAndFormatKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Create and set up our UI
    ui_.setupUi(this);

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<FileAndFormatKeyword *>(keyword);
    if (!keyword_)
    {
        Messenger::error("Couldn't cast base keyword '{}' into FileAndFormatKeyword.\n", keyword->name());
        return;
    }

    refreshing_ = true;

    // Populate combo with the file formats available
    ui_.FormatCombo->clear();
    for (int n = 0; n < keyword_->data().nFormats(); ++n)
        ui_.FormatCombo->addItem(QString::fromStdString(std::string(keyword_->data().formatKeyword(n))));

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

    emit(keywordValueChanged(keyword_->optionMask()));
}

void FileAndFormatKeywordWidget::on_FileEdit_returnPressed()
{
    if (refreshing_)
        return;

    updateKeywordData();

    checkFileValidity();

    emit(keywordValueChanged(keyword_->optionMask()));
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
            QFileDialog::getOpenFileName(this, tr("Select Input File"), fileInfo.absoluteFilePath(), tr("All Files (*.*)"));
    }
    else
    {
        // Output File
        filename =
            QFileDialog::getSaveFileName(this, tr("Select Output File"), fileInfo.absoluteFilePath(), tr("All Files (*.*)"));
    }

    if (!filename.isEmpty())
    {
        // Set relative path to file, using the current input file as the reference point
        QFileInfo fileInfo(QString::fromStdString(std::string(coreData_.inputFilename())));
        ui_.FileEdit->setText(fileInfo.dir().relativeFilePath(filename));
        updateKeywordData();
        updateWidgetValues(coreData_);
        emit(keywordValueChanged(keyword_->optionMask()));
    }
}

void FileAndFormatKeywordWidget::on_FormatCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateKeywordData();

    emit(keywordValueChanged(keyword_->optionMask()));
}

void FileAndFormatKeywordWidget::on_OptionsButton_clicked(bool checked)
{
    KeywordsDialog optionsDialog(this, keyword_->data().keywords(), coreData_);

    optionsDialog.showOptions();

    if (optionsDialog.keywordsModified())
        emit(keywordValueChanged(keyword_->optionMask()));
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
void FileAndFormatKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void FileAndFormatKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Grab the target FileAndFormat
    auto &fileAndFormat = keyword_->data();

    // UPdate widgets
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
    fileAndFormat.setFormatIndex(ui_.FormatCombo->currentIndex());

    keyword_->hasBeenSet();
}
