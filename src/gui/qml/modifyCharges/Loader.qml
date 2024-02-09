import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve

Page {
    id: root
    height: 166
    title: {
        switch (modify) {
        case ModifyChargesModel.Scaling:
            return "Scale Charges";
        case ModifyChargesModel.Smoothing:
            return "Smooth Charges";
        case ModifyChargesModel.ReduceSigFig:
            return "Reduce Significant Figures";
        default:
            break;
        }
    }
    visible: true
    width: 400

    Loader {
        id: loader
        anchors.fill: parent
        anchors.margins: 10
        objectName: "loader"
        sourceComponent: {
            var path = modify === ModifyChargesModel.Scaling ? "ScaleLayout.qml" : "Layout.qml";
            loader.setSource(qsTr("qrc:/dialogs/qml/modifyCharges/") + path, {
                    "dialogModel": dialogModel,
                    "modify": modify
                });
        }
    }
}
