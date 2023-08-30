import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0

Node {
    property variant model

    ColumnLayout {
        spacing: 0

        RowLayout {
            Text {
                text: `${model.data(model.index(index, 0), Qt.DisplayRole)}`
            }
            Image {
                Layout.preferredHeight: 15
                Layout.preferredWidth: 15
                source: "qrc:/general/icons/species.svg"
            }
        }
        Text {
            text: `Bonds: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.BondsCount)}`
        }
        Text {
            text: `Angles: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.AnglesCount)}`
        }
        Text {
            text: `Torsions: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.TorsionsCount)}`
        }
        Text {
            text: `Impropers: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.ImpropersCount)}`
        }
    }
}
