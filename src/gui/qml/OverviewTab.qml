import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import QtQuick 2.15
import Dissolve

ColumnLayout {
    id: root
    signal atomTypesClicked
    signal masterTermsClicked
    signal speciesClicked(int index)
    signal configurationClicked(int index)

    RowLayout {
        AtomTypes {
            nAtomTypes: dissolveModel.nAtomTypes
            MouseArea {
                anchors.fill: parent
                onClicked: root.atomTypesClicked()
            }
        }
        Bonds {
            nBonds: dissolveModel.nMasterBonds
            MouseArea {
                anchors.fill: parent
                onClicked: root.masterTermsClicked()
            }
        }
        Angles {
            nAngles: dissolveModel.nMasterAngles
            MouseArea {
                anchors.fill: parent
                onClicked: root.masterTermsClicked()
            }
        }
        Torsions {
            nTorsions: dissolveModel.nMasterTorsions
            MouseArea {
                anchors.fill: parent
                onClicked: root.masterTermsClicked()
            }
        }
        Impropers {
            nImpropers: dissolveModel.nMasterImpropers
            MouseArea {
                anchors.fill: parent
                onClicked: root.masterTermsClicked()
            }
        }
    }

    RowLayout {
        Repeater {
            id: species
            model: dissolveModel.species
            delegate: Species {
                model: dissolveModel.species
                modelData: model
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.speciesClicked(index)
                }
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
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.configurationClicked(index)
                }
            }
        }
    }
}