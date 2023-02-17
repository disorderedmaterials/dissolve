// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/models/atomTypeModel.h"
#include "gui/models/addForcefieldDialogModel.h"
#include "templates/algorithms.h"
#include <QInputDialog>
#include <QQuickItem>
#include <QMessageBox>
#include <set>
#include <QQuickWidget>
#include <QQmlContext>

AddForcefieldTermsDialog::AddForcefieldTermsDialog(QWidget *parent, Dissolve &dissolve) :
  QDialog(parent)
{

    qmlRegisterType<AddForcefieldDialogModel>("Dissolve", 1, 0, "AddForcefieldDialogModel");

    QQuickWidget *view = new QQuickWidget(QUrl("main.qml"), this);
    AtomTypeModel atModel(dissolve.coreData());

    atModel.setData(dissolve.coreData().atomTypes());

    view->rootContext()->setContextProperty("atModel", QVariant::fromValue(&atModel));
    view->setMinimumSize(300, 300);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    auto root = view->rootObject();
    auto model = root->findChild<AddForcefieldDialogModel*>("dialogModel");

    model->setDissolve(dissolve);

    connect(model, SIGNAL(cancel()), this, SLOT(reject()));
    connect(model, SIGNAL(accept()), this, SLOT(accept()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view);
    setLayout(topLeftLayout);

    // ui->verticalLayout->addWidget(container);
}
