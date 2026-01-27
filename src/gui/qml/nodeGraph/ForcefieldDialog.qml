import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve

Dialog {
    id: root

    required property int posx
    required property int posy
    required property variant graphModel

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: "Define Forcefield: " + sp.name

    onAccepted: {
        sp.create(root.posx, root.posy);
    }

    SimpleForcefieldModel {
        id: sp

        name: options.currentValue

        graphModel: root.graphModel
    }
    ComboBox {
        id: options
        editable: true

        model: sp.library 
    }
}
