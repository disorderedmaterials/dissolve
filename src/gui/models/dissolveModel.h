#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include "main/dissolve.h"
#include <vector>

class DissolveModel : public QAbstractItemModel
{
    Q_OBJECT

    private:
    OptionalReferenceWrapper<Dissolve> dissolve_;

    public:
    DissolveModel(Dissolve& dissolve);
    ~DissolveModel() = default;
    void setDissolve(Dissolve& dissolve);
    const QVariant rawData(const QModelIndex index) const;
    QVariant rawData(const QModelIndex index);
    void reset();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};