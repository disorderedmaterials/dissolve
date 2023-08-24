import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import QtQuick 2.15
import Dissolve

ColumnLayout {
    id: root

    RowLayout {
        AtomTypes {
            nAtomTypes: dissolveModel.nAtomTypes
        }
        Bonds {
            nBonds: dissolveModel.nMasterBonds
        }
        Angles {
            nAngles: dissolveModel.nMasterAngles
        }
        Torsions {
            nTorsions: dissolveModel.nMasterTorsions
        }
        Impropers {
            nImpropers: dissolveModel.nMasterImpropers
        }
    }

    RowLayout {
        Repeater {
            id: species
            model: dissolveModel.species
            delegate: Species {
                model: dissolveModel.species
                modelData: model
            }
        }
    }

    RowLayout {
        Repeater {
            id: configurations
            model: dissolveModel.configurations
            delegate: Configuration {
                model: dissolveModel.configurations
                modelData: model
            }
        }
    }
}