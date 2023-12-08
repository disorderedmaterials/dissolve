#include <QAbstractItemModel>

class ModelUpdater
{
    Q_Object

    public : ModelUpdater(const QAbstractItemModel);
    ~ModelUpdater();

    void connectModelSignals(QAbstractItemModel *model);

    signals:
    void modelsUpdated();

    private slots:
    void update();
};