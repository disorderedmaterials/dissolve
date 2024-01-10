// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "gui/addForcefieldTermsDialog.h"
#include "gui/models/addForcefieldDialogModel.h"
#include "templates/algorithms.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <set>

AddForcefieldTermsDialog::AddForcefieldTermsDialog(QWidget *parent, Dissolve &dissolve, Species *sp) : QDialog(parent)
{

    // For the QIcon theme
    // QIcon::setThemeName("personal");

    QQuickWidget *view = new QQuickWidget(QUrl("qrc:/dialogs/qml/AddForcefieldTermsDialog.qml"), this);
    AtomTypeModel atModel(dissolve.coreData());

    atModel.setData(dissolve.coreData().atomTypes());

    view->rootContext()->setContextProperty("atModel", QVariant::fromValue(&atModel));
    view->setMinimumSize(300, 300);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    auto root = view->rootObject();
    auto model = root->findChild<AddForcefieldDialogModel *>("dialogModel");

    model->setDissolve(dissolve);
    model->setSpecies(sp);
    model->ready();

    connect(model, SIGNAL(cancel()), this, SLOT(reject()));
    connect(model, SIGNAL(accept()), this, SLOT(accept()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view);
    setLayout(topLeftLayout);

    // ui->verticalLayout->addWidget(container);
}
