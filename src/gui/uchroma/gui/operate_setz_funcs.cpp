/*
	*** Data - SetZ Functions
	*** src/gui/uchroma/gui/data_setz_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/gui/operate_setz.h"
#include "gui/uchroma/classes/collection.h"
#include "base/lineparser.h"
#include "gui/uchroma/expression/variable.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

// Constructor
OperateSetZDialog::OperateSetZDialog(QWidget* parent, Collection* targetCollection) : QDialog(parent)
{
	ui.setupUi(this);

	targetCollection_ = targetCollection;

	// Setup initial equation
	indexVariable_ = equation_.createVariable("index", NULL, true);
	zVariable_ = equation_.createVariable("z", NULL, true);
	equation_.setGenerateMissingVariables(false);
	ui.FromEquationEdit->setText("index");

	// Update source files data
	updateFromSourceFilesData(true);

	updateControls();
}

// Destructor
OperateSetZDialog::~OperateSetZDialog()
{
}

/*
 * Slots
 */

void OperateSetZDialog::on_FromEquationRadio_toggled(bool checked)
{
	updateControls();
}

void OperateSetZDialog::on_FromEquationEdit_textChanged(QString text)
{
	if (equation_.generate(qPrintable(text))) ui.FromEquationEdit->setPalette(ui.FromEquationRadio->palette());
	else
	{
		QPalette redText;
		redText.setColor(QPalette::Text, Qt::red);
		ui.FromEquationEdit->setPalette(redText);
	}

	updateControls();
}

void OperateSetZDialog::on_FromSourceFilesRadio_toggled(bool checked)
{
	updateControls();
}

void OperateSetZDialog::on_FromSourceFilesLineSpin_valueChanged(int value)
{
	updateFromSourceFilesData(true);
}

void OperateSetZDialog::on_FromSourceFilesRegExpEdit_textChanged(QString text)
{
	updateFromSourceFilesData(false);
}

void OperateSetZDialog::on_FromSourceFilesDateTimeCheck_clicked(bool checked)
{
	ui.FromSourceFilesDateTimeEdit->setEnabled(checked);
}

void OperateSetZDialog::on_FromTimeStampsRadio_toggled(bool checked)
{
	updateControls();
}

void OperateSetZDialog::on_FromTimeStampsChooseButton_clicked(bool checked)
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select directory containing source files", ui.FromTimeStampsDirEdit->text());
	if (dir.isEmpty()) return;
	ui.FromTimeStampsDirEdit->setText(dir);
}

void OperateSetZDialog::on_FromListFileRadio_toggled(bool checked)
{
	updateControls();
}

void OperateSetZDialog::on_FromListFileChooseButton_clicked(bool checked)
{
	// TODO
}

void OperateSetZDialog::on_FromIndexFileRadio_toggled(bool checked)
{
	updateControls();
}

void OperateSetZDialog::on_FromIndexFileChooseButton_clicked(bool checked)
{
	// TODO
}

void OperateSetZDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void OperateSetZDialog::on_ApplyButton_clicked(bool checked)
{
	// Set some z's!
	bool result;
	if (ui.FromEquationRadio->isChecked()) result = setZFromEquation();
	else if (ui.FromTimeStampsRadio->isChecked()) result = setZFromTimeStamps();
	else if (ui.FromSourceFilesRadio->isChecked()) result = setZFromSourceFiles();
	if (result) accept();
	else reject();
}

/*
 * Functions
 */

// Set Z based on defined equation
bool OperateSetZDialog::setZFromEquation()
{
	if (!equation_.isValid()) return false;

	int count = 0;
	bool success;
	for (DataSet* dataSet = targetCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		indexVariable_->set(count);
		zVariable_->set(dataSet->z());
		double newZ = equation_.execute(success);
		if (!success)
		{
			QMessageBox::critical(this, "Expression error", "Failed to run expression");
			return false;
		}
		targetCollection_->setDataSetZ(dataSet, newZ);
		++count;
	}

	return true;
}

// Set Z from source files
bool OperateSetZDialog::setZFromSourceFiles()
{
	if (!sourceFilesAvailable_) return false;
	if (!sourceFilesRegExp_.isValid()) return false;

	// Loop over source files for datasets
	QString text;
	int nFailed = 0;
	double earliest = 0.0;
	QDateTime referenceTime(QDate(1970,1,1)), extractedTime;
	for (DataSet* dataSet = targetCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		if (dataSet->dataSource() != DataSet::FileSource)
		{
			++nFailed;
			continue;
		}

		LineParser parser;
		parser.openInput(dataSet->sourceFileName());
		if (!parser.isFileGoodForReading())
		{
			Messenger::print("No filename is set for dataset '%s' (and source = FileSource).\n", qPrintable(dataSet->name()));
			parser.closeFiles();
			++nFailed;
			continue;
		}
		parser.skipLines(ui.FromSourceFilesLineSpin->value()-1);
		if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
		{
			Messenger::print("Ran out of lines when getting data from file '%s'.\n", qPrintable(dataSet->sourceFileName()));
			++nFailed;
			continue;
		}
		text = parser.line();

		// Perform regexp
		sourceFilesRegExp_.exactMatch(text);
		if (sourceFilesRegExp_.captureCount() > 0)
		{
			if (ui.FromSourceFilesDateTimeCheck->isChecked())
			{
				extractedTime = QDateTime::fromString(sourceFilesRegExp_.cap(1), ui.FromSourceFilesDateTimeEdit->text());
				if (extractedTime.isValid())
				{
					dataSet->setZ(referenceTime.secsTo(extractedTime));
					if ((earliest == 0) || (dataSet->z() < earliest)) earliest = dataSet->z();
				}
				else
				{
					Messenger::print("Failed to extract date/time from string '%s' (in file '%s').\n", qPrintable(sourceFilesRegExp_.cap(1)), qPrintable(dataSet->sourceFileName()));
					++nFailed;
				}
			}
			else targetCollection_->setDataSetZ(dataSet, sourceFilesRegExp_.cap(1).toDouble());
		}
	}

	// Offset values if we were using date/time
	if (ui.FromSourceFilesDateTimeCheck->isChecked())
	{
		for (DataSet* dataSet = targetCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next) targetCollection_->setDataSetZ(dataSet, dataSet->z() - earliest);
	}

	// Any failures?
	if (nFailed > 0) QMessageBox::warning(this, "Errors encountered", "One or more errors were encountered while processing files. See the Log Window for details.");

	return true;
}

// Set Z from file timestamp
bool OperateSetZDialog::setZFromTimeStamps()
{
	QString extension = ui.FromTimeStampsExtensionEdit->text();
	if (!extension.isEmpty()) return false;

	QDir dir(ui.FromTimeStampsDirEdit->text());
	if (!dir.exists()) return false;

	// Load timestamp data from files - set offset in seconds from an arbitrary point to start with, then adjust afterwards
	QString s;
	double earliest = 0.0;
	QDateTime referenceTime(QDate(1970,1,1));
	for (DataSet* dataSet = targetCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Construct filename to search for
		QFileInfo baseInfo(dataSet->sourceFileName());
		s = dir.absoluteFilePath(baseInfo.baseName()) + "." + extension;
		QFileInfo fileInfo(s);
		if (!fileInfo.exists())
		{
			QMessageBox::warning(this, "Failed to Open File", "The file '" + s + "' could not be found.");
			break;
		}
		dataSet->setZ(referenceTime.secsTo(fileInfo.lastModified()));

		if ((earliest == 0) || (dataSet->z() < earliest)) earliest = dataSet->z();
	}
	
	// Set correct offset
	for (DataSet* dataSet = targetCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next) targetCollection_->setDataSetZ(dataSet, dataSet->z() - earliest);

	return true;
}

/*
 * Update
 */

// Update controls
void OperateSetZDialog::updateControls()
{
	// Set status of controls for each option
	ui.FromEquationEdit->setEnabled(ui.FromEquationRadio->isChecked());
	ui.FromSourceFilesRegExpEdit->setEnabled(ui.FromSourceFilesRadio->isChecked());
	ui.FromSourceFilesLineSpin->setEnabled(ui.FromSourceFilesRadio->isChecked());
	ui.FromSourceFilesDateTimeCheck->setEnabled(ui.FromSourceFilesRadio->isChecked());
	ui.FromSourceFilesDateTimeEdit->setEnabled(ui.FromSourceFilesRadio->isChecked() && ui.FromSourceFilesDateTimeCheck->isChecked());
	ui.FromSourceFilesTestTextEdit->setEnabled(ui.FromSourceFilesRadio->isChecked());
	ui.FromSourceFilesResultTextEdit->setEnabled(ui.FromSourceFilesRadio->isChecked());
	ui.FromTimeStampsDirEdit->setEnabled(ui.FromTimeStampsRadio->isChecked());
	ui.FromTimeStampsExtensionEdit->setEnabled(ui.FromTimeStampsRadio->isChecked());
	ui.FromTimeStampsChooseButton->setEnabled(ui.FromTimeStampsRadio->isChecked());
	ui.FromListFileEdit->setEnabled(ui.FromListFileRadio->isChecked());
	ui.FromListFileChooseButton->setEnabled(ui.FromListFileRadio->isChecked());
	ui.FromIndexFileEdit->setEnabled(ui.FromIndexFileRadio->isChecked());
	ui.FromIndexFileChooseButton->setEnabled(ui.FromIndexFileRadio->isChecked());

	// Set the status of the Apply button, depending on what has been selected...
	if (ui.FromEquationRadio->isChecked()) ui.ApplyButton->setEnabled(equation_.isValid());
	else if (ui.FromSourceFilesRadio->isChecked()) ui.ApplyButton->setEnabled(sourceFilesAvailable_ && sourceFilesRegExp_.isValid());
	else if (ui.FromTimeStampsRadio->isChecked())
	{
		if (ui.FromTimeStampsExtensionEdit->text().isEmpty()) ui.ApplyButton->setEnabled(false);
		else if (!QDir(ui.FromTimeStampsDirEdit->text()).exists()) ui.ApplyButton->setEnabled(false);
		else ui.ApplyButton->setEnabled(true);
	}
}

// Update data
void OperateSetZDialog::updateFromSourceFilesData(bool reReadLine)
{
	sourceFilesAvailable_ = false;
	sourceFilesTestText_ = "";

	// Update source files controls
	for (DataSet* dataSet = targetCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		if (dataSet->dataSource() != DataSet::FileSource) continue;

		LineParser parser;
		parser.openInput(qPrintable(dataSet->sourceFileName()));
		if (!parser.isFileGoodForReading())
		{
			parser.closeFiles();
			continue;
		}

		parser.skipLines(ui.FromSourceFilesLineSpin->value()-1);
		if (parser.readNextLine(LineParser::Defaults) != LineParser::Success) continue;
		sourceFilesTestText_ = parser.line();

		// All OK!
		sourceFilesAvailable_ = true;
	}

	ui.FromSourceFilesTestTextEdit->setText(sourceFilesTestText_);

	if (sourceFilesAvailable_)
	{
		sourceFilesRegExp_.setPattern(ui.FromSourceFilesRegExpEdit->text());
		if (sourceFilesRegExp_.isValid())
		{
			ui.FromSourceFilesRegExpEdit->setPalette(ui.FromSourceFilesRadio->palette());
			sourceFilesRegExp_.exactMatch(sourceFilesTestText_);
			ui.FromSourceFilesResultTextEdit->setText(sourceFilesRegExp_.cap(1));
		}
		else
		{
			QPalette redText;
			redText.setColor(QPalette::Text, Qt::red);
			ui.FromSourceFilesRegExpEdit->setPalette(redText);
			ui.FromSourceFilesResultTextEdit->setText("");
		}
	}

	// Enable / disable controls
	ui.FromSourceFilesRadio->setEnabled(sourceFilesAvailable_);
	ui.FromSourceFilesLineSpin->setEnabled(sourceFilesAvailable_);
	ui.FromSourceFilesRegExpEdit->setEnabled(sourceFilesAvailable_);
}
