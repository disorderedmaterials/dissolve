// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/coredata.h"
#include "gui/getmodulelayernamedialog.h"
#include "module/layer.h"

GetModuleLayerNameDialog::GetModuleLayerNameDialog(QWidget *parent, const List<ModuleLayer> &layers) : layers_(layers)
{
    ui_.setupUi(this);
}

GetModuleLayerNameDialog::~GetModuleLayerNameDialog() {}

// Run the dialog
bool GetModuleLayerNameDialog::get(const ModuleLayer *moduleLayer, const QString &currentName)
{
    moduleLayer_ = moduleLayer;

    // Set the current text
    ui_.NameEdit->setText(currentName);

    show();

    return (exec() == QDialog::Accepted);
}

// Return the name string
QString GetModuleLayerNameDialog::newName() const { return ui_.NameEdit->text(); }

/*
 * Slots
 */

// ModuleLayer name edited
void GetModuleLayerNameDialog::on_NameEdit_textChanged(const QString text)
{
    // Make sure the name is valid
    auto nameValid = true;
    if (text.isEmpty())
        nameValid = false;
    else
    {
        ListIterator<ModuleLayer> layerIterator(layers_);
        while (ModuleLayer *layer = layerIterator.iterate())
        {
            if (moduleLayer_ == layer)
                continue;

            if (DissolveSys::sameString(layer->name(), qPrintable(text)))
            {
                nameValid = false;
                break;
            }
        }
    }

    // Update indicator
    ui_.NameIndicator->setOK(nameValid);

    // Update dialog buttons
    ui_.OKButton->setEnabled(nameValid);
}

void GetModuleLayerNameDialog::on_CancelButton_clicked(bool checked) { reject(); }

void GetModuleLayerNameDialog::on_OKButton_clicked(bool checked) { accept(); }
