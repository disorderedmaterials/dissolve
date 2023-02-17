// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/models/atomTypeModel.h"
#include "templates/algorithms.h"
#include "data/ff/library.h"
#include <QInputDialog>
#include <QQuickItem>
#include <QMessageBox>
#include <set>
#include <QQuickWidget>
#include <QQmlContext>

AddForcefieldTermsDialog::AddForcefieldTermsDialog(QWidget *parent, Dissolve &dissolve) :
  QDialog(parent), dissolve_(dissolve), ffModel_(ForcefieldLibrary::forcefields())
{
    QQuickWidget *view = new QQuickWidget;
    AtomTypeModel atModel(dissolve.coreData());

    atModel.setData(dissolve.coreData().atomTypes());

    view->rootContext()->setContextProperty("ffModel", QVariant::fromValue(&ffModel_));
    view->rootContext()->setContextProperty("atModel", QVariant::fromValue(&atModel));
    view->setSource(QUrl("main.qml"));
    view->setParent(this);
    view->setMinimumSize(200,200);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // QWidget *container = QWidget::createWindowContainer(view, this);

    // some set up if needed
    // container->setMinimumSize(1280, 600);
    // container->setMaximumSize(1280, 600);
    // container->setFocusPolicy(Qt::TabFocus);

    // ..

    auto root = view->rootObject();

    connect(root, SIGNAL(qmlCancel()), this, SLOT(reject()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view);
    setLayout(topLeftLayout);

    // ui->verticalLayout->addWidget(container);
}
