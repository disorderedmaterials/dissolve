/*
	*** Keyword Widget - FileAndFormat
	*** src/gui/keywordwidgets/fileandformat_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/keywordwidgets/fileandformat.h"
#include "gui/keywordwidgets/dropdown.h"
#include "base/fileandformat.h"
#include "templates/genericlisthelper.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>

// Constructor
FileAndFormatKeywordWidget::FileAndFormatKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : QWidget(parent), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Create and set up our UI
	ui.setupUi(this);

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<FileAndFormatModuleKeyword*>(keyword);
	if (!keyword_)
	{
		Messenger::error("Couldn't cast base module keyword '%s' into FileAndFormatModuleKeyword.\n", keyword->keyword());
		return;
	}

	refreshing_ = true;

	// Populate combo with the file formats available
	ui.FileFormatCombo->clear();
	for (int n=0; n < keyword_->data().nFormats(); ++n) ui.FileFormatCombo->addItem(keyword_->data().format(n));

	// Set current information
	updateWidgetValues(coreData_);

	refreshing_ = false;
}

/*
 * Signals / Slots
 */

void FileAndFormatKeywordWidget::on_FileEdit_editingFinished()
{
	if (refreshing_) return;

	updateKeywordData();

	checkFileValidity();

	emit(keywordValueChanged());
}

void FileAndFormatKeywordWidget::on_FileEdit_returnPressed()
{
	if (refreshing_) return;

	updateKeywordData();

	checkFileValidity();

	emit(keywordValueChanged());
}

void FileAndFormatKeywordWidget::on_FileFormatCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	updateKeywordData();

	emit(keywordValueChanged());
}

void FileAndFormatKeywordWidget::on_FileSelectButton_clicked(bool checked)
{
	// Grab the target FileAndFormat
	FileAndFormat& fileAndFormat = keyword_->data();

	// Determine what sort of dialog we need to raise...
	QString filename;
	if (fileAndFormat.fileMustExist())
	{
		// Input File
		QFileInfo fileInfo(fileAndFormat.filename());
		filename = QFileDialog::getOpenFileName(this, tr("Select Input File"), fileInfo.absoluteFilePath(), tr("All Files (*.*)"));
	}
	else
	{
		// Output File
		QFileInfo fileInfo(fileAndFormat.filename());
		filename = QFileDialog::getSaveFileName(this, tr("Select Output File"), fileInfo.absoluteFilePath(), tr("All Files (*.*)"));
	}

	if (!filename.isEmpty())
	{
		fileAndFormat.setFilename(qPrintable(filename));
		updateWidgetValues(coreData_);
	}
}

/*
 * Update
 */

// Check / set validity of current filename
void FileAndFormatKeywordWidget::checkFileValidity()
{
	// Grab the target FileAndFormat
	const FileAndFormat& fileAndFormat = keyword_->data();

	// If this is an export FileAndFormat, no need to show the indicator or check if the file exists
	if (fileAndFormat.fileMustExist())
	{
		ui.FileExistsIndicator->setVisible(true);
		ui.FileExistsIndicator->setOK(fileAndFormat.hasFilename() ? QFile::exists(fileAndFormat.filename()) : false);
	}
	else
	{
		ui.FileExistsIndicator->setVisible(false);
	}
}

// Update value displayed in widget, using specified source if necessary
void FileAndFormatKeywordWidget::updateValue()
{
	refreshing_ = true;

	/*
	 * We normally check in our moduleData to see if updated data exists there, however in the case of 
	 * FileAndFormat keywords we store references to the derived classes. So, such searching in this way is not
	 * possible.
	 */
// 	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
// 	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
// 	{
// 		// Retrieve the item from the list and set our widgets
// 		keyword_->data() = GenericListHelper<FileAndFormat>::value(moduleData_, keyword_->keyword(), modulePrefix_);
// 	}

	refreshing_ = false;

	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void FileAndFormatKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Grab the target FileAndFormat
	FileAndFormat& fileAndFormat = keyword_->data();

	// Widgets
	ui.FileEdit->setText(fileAndFormat.filename());
	ui.FileFormatCombo->setCurrentIndex(fileAndFormat.formatIndex());
	checkFileValidity();

	refreshing_ = false;
}

// Update keyword data based on widget values
void FileAndFormatKeywordWidget::updateKeywordData()
{
	// Grab the target FileAndFormat
	FileAndFormat& fileAndFormat = keyword_->data();

	fileAndFormat.setFilename(qPrintable(ui.FileEdit->text()));
	fileAndFormat.setFormatIndex(ui.FileFormatCombo->currentIndex());
}
