import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import QtQuick 2.15
import Dissolve 1.0
import "../widgets" as D

ColumnLayout {
    id: root
    anchors.fill: parent

    signal atomTypesClicked
    signal configurationClicked(int index)
    signal masterTermsClicked
    signal moduleLayerClicked(int index)
    signal speciesClicked(int index)

    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.preferredHeight: 100

        AtomTypes {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumHeight: 200
            Layout.maximumWidth: 200
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200
            atomTypeModel: dissolveModel.atomTypesModel

            MouseArea {
                anchors.fill: parent

                onClicked: root.atomTypesClicked()
            }
        }
        Bonds {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumHeight: 200
            Layout.maximumWidth: 200
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200
            nBonds: dissolveModel.masterBondsModel.rowCount

            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }
        Angles {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumHeight: 200
            Layout.maximumWidth: 200
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200
            nAngles: dissolveModel.masterAnglesModel.rowCount

            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }
        Torsions {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumHeight: 200
            Layout.maximumWidth: 200
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200
            nTorsions: dissolveModel.masterTorsionsModel.rowCount

            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }
        Impropers {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumHeight: 200
            Layout.maximumWidth: 200
            Layout.preferredHeight: 100
            Layout.preferredWidth: 200
            nImpropers: dissolveModel.masterImpropersModel.rowCount

            MouseArea {
                anchors.fill: parent

                onClicked: root.masterTermsClicked()
            }
        }
    }
    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.preferredHeight: 100

        Repeater {
            id: species
            model: dissolveModel.speciesModel

            delegate: Species {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumHeight: 200
                Layout.maximumWidth: 200
                Layout.preferredHeight: 100
                Layout.preferredWidth: 200
                model: dissolveModel.speciesModel

                MouseArea {
                    anchors.fill: parent

                    onClicked: root.speciesClicked(index)
                }
            }
        }
    }
    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.preferredHeight: 100

        Repeater {
            id: configurations
            model: dissolveModel.configurationsModel

            delegate: Configuration {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumHeight: 200
                Layout.maximumWidth: 200
                Layout.preferredHeight: 100
                Layout.preferredWidth: 200
                model: dissolveModel.configurationsModel

                MouseArea {
                    anchors.fill: parent

                    onClicked: root.configurationClicked(index)
                }
            }
        }
    }
    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.preferredHeight: 100

        Repeater {
            id: layers
            model: dissolveModel.moduleLayersModel

            delegate: ModuleLayer {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumHeight: 200
                Layout.maximumWidth: 200
                Layout.preferredHeight: 100
                Layout.preferredWidth: 200
                model: dissolveModel.moduleLayersModel

                MouseArea {
                    anchors.fill: parent

                    onClicked: root.moduleLayerClicked(index)
                }
            }
        }
    }
}
