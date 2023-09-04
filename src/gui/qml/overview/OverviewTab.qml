import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import QtQuick 2.15
import Dissolve 1.0

ColumnLayout {
    id: root
    signal atomTypesClicked
    signal configurationClicked(int index)
    signal masterTermsClicked
    signal speciesClicked(int index)

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
            model: dissolveModel.speciesModel

            delegate: Species {
                model: dissolveModel.speciesModel

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
            model: dissolveModel.configurationsModel

            delegate: Configuration {
                model: dissolveModel.configurationsModel

                MouseArea {
                    anchors.fill: parent

                    onClicked: root.configurationClicked(index)
                }
            }
        }
    }
}
