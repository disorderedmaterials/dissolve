#include "main/dissolve.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

class DissolveModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
    DissolveModel(Dissolve &dissolve);
    ~DissolveModel() = default;

    /*
     * Data
     */
    private:
    // Reference to Dissolve
    OptionalReferenceWrapper<Dissolve> dissolve_;

    public:
    // Set reference to Dissolve
    void setDissolve(Dissolve &dissolve);
    const QVariant rawData(const QModelIndex index) const;
    QVariant rawData(const QModelIndex index);
    // Update the model
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};