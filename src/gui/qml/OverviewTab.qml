import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import QtQuick 2.15


ColumnLayout {
    spacing: 5
    RowLayout {
        Rectangle {
            id: atomTypes
            border.color: "steelblue"
            color: "lightblue"
            height: 50
            width: 100

            radius: 5
            Text {
                id: atomTypesText
                anchors.fill: parent
                text: "Atom Types (" + dissolveModel.atomTypes.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Rectangle {
            id: bonds
            border.color: "steelblue"
            color: "lightblue"
            height: 50
            width: 100

            radius: 5
            Text {
                id: bondsText
                anchors.fill: parent
                text: "Master Bonds (" + dissolveModel.bonds.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Rectangle {
            id: angles
            border.color: "steelblue"
            color: "lightblue"
            height: 50
            width: 100

            radius: 5
            Text {
                id: anglesText
                anchors.fill: parent
                text: "Master Angles (" + dissolveModel.angles.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Rectangle {
            id: torsions
            border.color: "steelblue"
            color: "lightblue"
            height: 50
            width: 100

            radius: 5
            Text {
                id: torsionsText
                anchors.fill: parent
                text: "Master Torsions (" + dissolveModel.torsions.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Rectangle {
            id: impropers
            border.color: "steelblue"
            color: "lightblue"
            height: 50
            width: 100

            radius: 5
            Text {
                id: impropersText
                anchors.fill: parent
                text: "Master Impropers (" + dissolveModel.impropers.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    RowLayout {
        Repeater {
            model: dissolveModel.species.rowCount()
            delegate: Rectangle {
                id: species
                border.color: "steelblue"
                color: "lightblue"
                height: 50
                width: 100

                radius: 5
                Text {
                    id: speciesNameText
                    anchors.fill: parent
                    text: '<img src="qrc:/general/icons/species.svg" width="15" height="15">' + dissolveModel.species.data(dissolveModel.species.index(index, 0), Qt.DisplayRole)
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    RowLayout {
        Repeater {
            model: dissolveModel.configurations.rowCount()
            delegate: Rectangle {
                id: configuration
                border.color: "steelblue"
                color: "lightblue"
                height: 50
                width: 100

                radius: 5
                Text {
                    id: configurationNameText
                    anchors.fill: parent
                    text: '<img src="qrc:/general/icons/configuration.svg" width="15" height="15">' + dissolveModel.configurations.data(dissolveModel.configurations.index(index, 0), Qt.DisplayRole)
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

}