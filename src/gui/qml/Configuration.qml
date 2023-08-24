import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {

    function generateConfigurationMarkup(index) {
        if (!model)
            return "";
        const configurationData = model.data;
        const configurationIndex = model.index(index, 0);
        const configurationName = configurationData(configurationIndex, Qt.DisplayRole);
        const molecules = configurationData(configurationIndex, Qt.UserRole + 1);
        const atoms = configurationData(configurationIndex, Qt.UserRole + 2);
        const markup = `
        <div style="font-size: 16px;">
            ${configurationName} <img src="qrc:/general/icons/configuration.svg" width="15" height="15"> <br/>
        </div>
        <div style="font-size: 14px; text-align: center;">
            Molecules: ${molecules} <br/>
            Atoms: ${atoms}
        </div>
    `;
        return markup;
    }

    id: root
    border.color: "steelblue"
    color: "lightblue"
    height: 100
    radius: 5
    width: 100

    property variant modelData
    property variant model

    Text {
        text: generateConfigurationMarkup(index)
    }
}