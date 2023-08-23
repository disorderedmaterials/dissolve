import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0
import QtQuick 2.15

ColumnLayout {
    id: root
    signal atomTypesClicked
    signal configurationClicked(int index)
    signal masterTermsClicked
    signal speciesClicked(int index)

    function generateConfigurationMarkup(index) {
        const configurationData = dissolveModel.configurations.data;
        const configurationIndex = dissolveModel.configurations.index(index, 0);
        const configurationName = configurationData(configurationIndex, Qt.DisplayRole);
        const molecules = configurationData(configurationIndex, Qt.UserRole + 1);
        const atoms = configurationData(configurationIndex, Qt.UserRole + 2);
        const markup = `
        <div style="font-size: 16px;">
            ${configurationName} <img src="qrc:/general/icons/configuration.svg" width="15" height="15">
        </div>
        <div style="font-size: 14px; text-align: center;">
            Molecules: ${molecules} <br/>
            Atoms: ${atoms}
        </div>
    `;
        return markup;
    }
    function generateSpeciesMarkup(index) {
        const speciesData = dissolveModel.species.data;
        const speciesIndex = dissolveModel.species.index(index, 0);
        const speciesName = speciesData(speciesIndex, Qt.DisplayRole);
        const bonds = speciesData(speciesIndex, Qt.UserRole + 1);
        const angles = speciesData(speciesIndex, Qt.UserRole + 2);
        const torsions = speciesData(speciesIndex, Qt.UserRole + 3);
        const impropers = speciesData(speciesIndex, Qt.UserRole + 4);
        const markup = `
        <div style="font-size: 16px;">
            ${speciesName} <img src="qrc:/general/icons/species.svg" width="15" height="15">
        </div>
        <div style="font-size: 14px; text-align: center;">
            Bonds: ${bonds}<br/>
            Angles: ${angles}<br/>
            Torsions: ${torsions}<br/>
            Impropers: ${impropers}
        </div>
    `;
        return markup;
    }

    spacing: 5

    Connections {
        function onModelsUpdated() {
            atomTypesText.text = "Atom Types (" + dissolveModel.atomTypes.rowCount() + ")";
            bondsText.text = "Master Bonds (" + dissolveModel.bonds.rowCount() + ")";
            anglesText.text = "Master Angles (" + dissolveModel.angles.rowCount() + ")";
            torsionsText.text = "Master Torsions (" + dissolveModel.torsions.rowCount() + ")";
            impropersText.text = "Master Impropers (" + dissolveModel.impropers.rowCount() + ")";
            species.model = dissolveModel.species.rowCount();
            configurations.model = dissolveModel.configurations.rowCount();
        }

        target: dissolveModel
    }
    RowLayout {
        Rectangle {
            id: atomTypes
            border.color: "steelblue"
            color: "lightblue"
            height: 100
            radius: 5
            width: 100

            Text {
                id: atomTypesText
                anchors.fill: parent
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                text: "Atom Types (" + dissolveModel.atomTypes.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
            }
            MouseArea {
                height: parent.height
                width: parent.width

                onClicked: root.atomTypesClicked()
            }
        }
        Rectangle {
            id: bonds
            border.color: "steelblue"
            color: "lightblue"
            height: 100
            radius: 5
            width: 100

            Text {
                id: bondsText
                anchors.fill: parent
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                text: "Master Bonds (" + dissolveModel.bonds.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
            }
            MouseArea {
                height: parent.height
                width: parent.width

                onClicked: root.masterTermsClicked()
            }
        }
        Rectangle {
            id: angles
            border.color: "steelblue"
            color: "lightblue"
            height: 100
            radius: 5
            width: 100

            Text {
                id: anglesText
                anchors.fill: parent
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                text: "Master Angles (" + dissolveModel.angles.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
            }
            MouseArea {
                height: parent.height
                width: parent.width

                onClicked: root.masterTermsClicked()
            }
        }
        Rectangle {
            id: torsions
            border.color: "steelblue"
            color: "lightblue"
            height: 100
            radius: 5
            width: 100

            Text {
                id: torsionsText
                anchors.fill: parent
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                text: "Master Torsions (" + dissolveModel.torsions.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
            }
            MouseArea {
                height: parent.height
                width: parent.width

                onClicked: root.masterTermsClicked()
            }
        }
        Rectangle {
            id: impropers
            border.color: "steelblue"
            color: "lightblue"
            height: 100
            radius: 5
            width: 100

            Text {
                id: impropersText
                anchors.fill: parent
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                text: "Master Impropers (" + dissolveModel.impropers.rowCount() + ")"
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
            }
            MouseArea {
                height: parent.height
                width: parent.width

                onClicked: root.masterTermsClicked()
            }
        }
    }
    RowLayout {
        Repeater {
            id: species
            model: dissolveModel.species.rowCount()

            delegate: Rectangle {
                border.color: "steelblue"
                color: "lightblue"
                height: 100
                radius: 5
                width: 100

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: generateSpeciesMarkup(index)
                    wrapMode: Text.WordWrap
                }
                MouseArea {
                    height: parent.height
                    width: parent.width

                    onClicked: root.speciesClicked(index)
                }
            }
        }
    }
    RowLayout {
        Repeater {
            id: configurations
            model: dissolveModel.configurations.rowCount()

            delegate: Rectangle {
                border.color: "steelblue"
                color: "lightblue"
                height: 100
                radius: 5
                width: 100

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: generateConfigurationMarkup(index)
                    wrapMode: Text.WordWrap
                }
                MouseArea {
                    height: parent.height
                    width: parent.width

                    onClicked: root.configurationClicked(index)
                }
            }
        }
    }
}
