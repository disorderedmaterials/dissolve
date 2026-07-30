import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve

Dialog {
    id: root

    required property variant graphModel
    required property int posx
    required property int posy

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: "Define Forcefield: " + sp.name

    onAccepted: {
        sp.create(root.posx, root.posy);
    }

    SimpleForcefieldModel {
        id: sp

        graphModel: root.graphModel
        name: options.currentValue
    }
    ComboBox {
        id: options

        editable: true
        model: sp.library
    }
}
