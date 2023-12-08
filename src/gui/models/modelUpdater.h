#include <QAbstractItemModel>

class ModelUpdater
{
    Q_Object

        public : explicit ModelUpdater(const QAbstractItemModel);

    void connectModelSignals(QAbstractItemModel *model);

    signals:
    void modelsUpdated();

    private slots:
    void update();
};