import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import QtQuick 2.15
import Dissolve 1.0
import "../widgets" as D

ColumnLayout {
    id: root
    signal atomTypesClicked
    signal configurationClicked(int index)
    signal masterTermsClicked
    signal moduleLayerClicked(int index)
    signal speciesClicked(int index)
    anchors.fill: parent


    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 100
        AtomTypes {
            nAtomTypes: dissolveModel.nAtomTypes
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200

            Layout.maximumWidth: 200
            Layout.maximumHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true
            MouseArea {
                anchors.fill: parent

                onClicked: root.atomTypesClicked()
            }
        }
        Bonds {
            nBonds: dissolveModel.nMasterBonds
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200

            Layout.maximumWidth: 200
            Layout.maximumHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true
            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }
        Angles {
            nAngles: dissolveModel.nMasterAngles
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200

            Layout.maximumWidth: 200
            Layout.maximumHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true
            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }
        Torsions {
            nTorsions: dissolveModel.nMasterTorsions
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200

            Layout.maximumWidth: 200
            Layout.maximumHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true
            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }
        Impropers {
            nImpropers: dissolveModel.nMasterImpropers
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200

            Layout.maximumWidth: 200
            Layout.maximumHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true
            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }

    }

    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 100
        Repeater {
            id: species
            model: dissolveModel.speciesModel

            delegate: Species {
                model: dissolveModel.speciesModel
                Layout.preferredHeight: 100
                Layout.preferredWidth: 200

                Layout.maximumWidth: 200
                Layout.maximumHeight: 200
                Layout.fillWidth: true
                Layout.fillHeight: true
                MouseArea {
                    anchors.fill: parent

                    onClicked: root.speciesClicked(index)
                }
            }
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 100
        Repeater {
            id: configurations
            model: dissolveModel.configurationsModel

            delegate: Configuration {
                model: dissolveModel.configurationsModel
                Layout.preferredHeight: 100
                Layout.preferredWidth: 200

                Layout.maximumWidth: 200
                Layout.maximumHeight: 200
                Layout.fillWidth: true
                Layout.fillHeight: true
                MouseArea {
                    anchors.fill: parent

                    onClicked: root.configurationClicked(index)
                }
            }
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 100
        Repeater {
            id: layers
            model: dissolveModel.moduleLayersModel

            delegate: ModuleLayer {
                model: dissolveModel.moduleLayersModel
                Layout.preferredHeight: 100
                Layout.preferredWidth: 200

                Layout.maximumWidth: 200
                Layout.maximumHeight: 200
                Layout.fillWidth: true
                Layout.fillHeight: true
                MouseArea {
                    anchors.fill: parent

                    onClicked: root.moduleLayerClicked(index)
                }
            }
        }
    }
}

