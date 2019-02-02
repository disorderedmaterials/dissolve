/*
	*** Save Image Dialog
	*** src/gui/uchroma/gui/saveimage_funcs.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/uchroma/gui/saveimage.h"
#include "gui/uchroma/uchromabase.h"
#include <QFileDialog>

// Constructor
SaveImageDialog::SaveImageDialog(UChromaBase& uChromaBase, QWidget* parent) : QDialog(parent), uChromaBase_(uChromaBase)
{
	ui.setupUi(this);
	
	// Populate format combo
	for (int n=0; n<uChromaBase_.nImageFormats; ++n) ui.ImageFormatCombo->addItem( QString(uChromaBase_.imageFormatExtension((UChromaBase::ImageFormat) n)).toUpper());
}

// Destructor
SaveImageDialog::~SaveImageDialog()
{
}

// Call dialog to get image save information
bool SaveImageDialog::getImageDetails(double currentAspect)
{
	ui.FileNameEdit->setText(uChromaBase_.imageExportFileName());
	currentDirectory_.setPath(uChromaBase_.imageExportFileName());
	aspectRatio_ = currentAspect;
	ui.MaintainAspectRatioCheck->setChecked(uChromaBase_.imageExportMaintainAspect());
	ui.ImageWidthSpin->setValue(uChromaBase_.imageExportWidth());
	ui.ImageHeightSpin->setValue(uChromaBase_.imageExportMaintainAspect() ? uChromaBase_.imageExportWidth()/aspectRatio_ : uChromaBase_.imageExportHeight());
	ui.ImageFormatCombo->setCurrentIndex(uChromaBase_.imageExportFormat());

	int result = exec();
	return (result == 1);
}

/*
 * Slots
 */

void SaveImageDialog::on_SelectFileNameButton_clicked(bool checked)
{
	QString newFile = QFileDialog::getSaveFileName(this, "Choose image save file name", currentDirectory_.absolutePath(), QString(uChromaBase_.imageFormatFilter((UChromaBase::ImageFormat) ui.ImageFormatCombo->currentIndex())) + ";;All files (*.*)");
	if (!newFile.isEmpty()) ui.FileNameEdit->setText(newFile);
}

void SaveImageDialog::on_ImageWidthSpin_valueChanged(int value)
{
	if (ui.MaintainAspectRatioCheck->isChecked()) ui.ImageHeightSpin->setValue(value / aspectRatio_);
}

void SaveImageDialog::on_MaintainAspectRatioCheck_toggled(bool checked)
{
	ui.ImageHeightSpin->setDisabled(checked);
	if (checked) ui.ImageHeightSpin->setValue(ui.ImageWidthSpin->value() / aspectRatio_);
}

void SaveImageDialog::on_GetCurrentButton_clicked(bool checked)
{
	ui.MaintainAspectRatioCheck->setChecked(true);
	aspectRatio_ = double(uChromaBase_.viewer()->contextWidth()) / double(uChromaBase_.viewer()->contextHeight());
	ui.ImageWidthSpin->setValue(uChromaBase_.viewer()->contextWidth());
	ui.ImageHeightSpin->setValue(uChromaBase_.viewer()->contextHeight());	
}

void SaveImageDialog::on_SaveImageButton_clicked(bool checked)
{
	// Set updated values...
	uChromaBase_.setImageExportFileName(qPrintable(ui.FileNameEdit->text()));
	uChromaBase_.setImageExportWidth(ui.ImageWidthSpin->value());
	uChromaBase_.setImageExportHeight(ui.ImageHeightSpin->value());
	uChromaBase_.setImageExportMaintainAspect(ui.MaintainAspectRatioCheck->isChecked());
	uChromaBase_.setImageExportFormat((UChromaBase::ImageFormat) ui.ImageFormatCombo->currentIndex());

	accept();
}

void SaveImageDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}
