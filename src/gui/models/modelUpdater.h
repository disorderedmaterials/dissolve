#include <QAbstractItemModel>
#include <QObject>

class ModelUpdater : public QObject
{
    Q_Object

        public : ModelUpdater() = default;
    explicit ModelUpdater(const QAbstractItemModel);

    void connectModelSignals(QAbstractItemModel *model);

    signals:
    void modelsUpdated();

    private slots:
    void update();
}