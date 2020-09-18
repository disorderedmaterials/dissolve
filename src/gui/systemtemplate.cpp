/*
    *** System Template
    *** src/gui/systemtemplate.cpp
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

#include "gui/systemtemplate.h"
#include "base/messenger.h"
#include "templates/variantpointer.h"
#include <QDir>
#include <QFile>
#include <QToolButton>
#include <QXmlStreamReader>
#include <stdlib.h>

SystemTemplate::SystemTemplate() { toolButton_ = nullptr; }

SystemTemplate::~SystemTemplate() {}

SystemTemplate::SystemTemplate(const SystemTemplate &source) { (*this) = source; }

void SystemTemplate::operator=(const SystemTemplate &source)
{
    name_ = source.name_;
    description_ = source.description_;
    group_ = source.group_;
    iconResource_ = source.iconResource_;
    inputFileData_ = source.inputFileData_;
    toolButton_ = source.toolButton_;
}

/*
 * Data
 */

// Read data contained in specified dir
bool SystemTemplate::read(const QDir rootResourceDir)
{
    // Set up an XML stream reader on the expected template info file
    QFile infoFile(rootResourceDir.filePath("info.xml"));
    if (!infoFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return Messenger::error("Couldn't open template system info file '{}'.\n", qPrintable(infoFile.fileName()));
    QXmlStreamReader infoReader(&infoFile);

    // Check that we have a file with the correct root node
    if (!infoReader.readNextStartElement())
        return Messenger::error("Failed to do initial read from system template xml.\n");
    if (infoReader.name() != "systemtemplate")
        return Messenger::error("System template file has wrong root node type ({}).\n",
                                qPrintable(infoReader.name().toString()));

    QString inputFileResource;

    // Now loop over root node elements
    while (infoReader.readNextStartElement())
    {
        // Check the name of the root element
        QString token = infoReader.name().toString();

        if (token == "name")
            name_ = infoReader.readElementText();
        else if (token == "icon")
            iconResource_ = rootResourceDir.absoluteFilePath(infoReader.readElementText());
        else if (token == "input")
            inputFileResource = rootResourceDir.absoluteFilePath(infoReader.readElementText());
        else if (token == "group")
            group_ = infoReader.readElementText();
        else if (token == "description")
            description_ = infoReader.readElementText();
        else
        {
            Messenger::error("Unrecognised token '{}' found in system template.\n", qPrintable(token));
            return false;
        }
    }

    // Load and store the input resource data
    if (!inputFileResource.isEmpty())
    {
        QFile file(inputFileResource);
        if (!file.open(QIODevice::ReadOnly))
            return Messenger::error("Failed to open input file data for template '{}'.\n", qPrintable(name_));
        else
            inputFileData_ = file.readAll();
        file.close();
    }

    return true;
}

// Return name of the template
QString SystemTemplate::name() const { return name_; }

// Return group in which the template exists
QString SystemTemplate::group() const { return group_; }

// Return short description of the template
QString SystemTemplate::description() const { return description_; }

// Return resource path to the template's icon
QString SystemTemplate::iconResource() const { return iconResource_; }

// Return the template's input data
QString SystemTemplate::inputFileData() const { return inputFileData_; }

// Return QToolButton created for the template
QToolButton *SystemTemplate::toolButton() const { return toolButton_; }

// Create and return a button for this template
QToolButton *SystemTemplate::createButton()
{
    if (toolButton_)
        Messenger::warn("A QToolButton already exists for the SystemTemplate '{}'. It will be overwritten.\n",
                        qPrintable(name_));
    toolButton_ = new QToolButton;
    toolButton_->setText(name_);
    toolButton_->setMaximumSize(QSize(200, 200));
    toolButton_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolButton_->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolButton_->setIcon(QPixmap(iconResource_));
    toolButton_->setIconSize(QSize(64, 64));
    toolButton_->setToolTip(description_);

    return toolButton_;
}
