import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve

Page {
    id: root
    height: 166
    title: {
        switch(modify) {
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
    width: 400
    visible: true

    Loader {
        id: loader
        anchors.fill: parent
        anchors.margins: 10
        sourceComponent: {
            var path = modify === ModifyChargesModel.Scaling ? "ScaleLayout.qml" : "Layout.qml" 
            loader.setSource(qsTr("qrc:/dialogs/qml/modifyCharges/")+path, { "dialogModel" : dialogModel, "modify" : modify })
        }
    }
}