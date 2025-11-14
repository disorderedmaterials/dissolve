import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve


Dialog {
    id: root
    required property variant species;

    standardButtons: Dialog.Ok
    title: "Define Species: " + sp.name

    SpeciesModel {
        id: sp
        species: root.species
    }
    GridLayout {
        columns: 2
        anchors.fill: parent
        Text { text: "Name" }
        TextField {
            placeholderText: sp.name
            onTextChanged: sp.name = text
        }
    }
}
