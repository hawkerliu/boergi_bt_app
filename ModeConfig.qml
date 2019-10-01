import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Material 2.12
import QtQml.Models 2.12

ApplicationWindow {
    id: confDialog
    visible: false
    title: "Custom sensor configuration"
    modality: Qt.ApplicationModal
    flags: Qt.Dialog
    height: root.height/3
    width: root.width/3

    Material.theme: globalTheme
    Material.accent: globalAccent

    property int count: modeContainer.count
    property color confColor: globalTheme==Material.Light ? "black" : "white"
    property int fontSize: 20
    property variant oldConfigs: new Array(0)
    property alias list: modeContainer

    onVisibleChanged: {
        if (visible==true)
        {
            oldConfigs = new Array (0)
            for (var i=0;i<modeContainer.count;i++)
            {
                oldConfigs.push(modeContainer.itemAtIndex(i).currentText)
            }
        }
        else {
            for (var j=0;j<confDialog.oldConfigs.length;j++)
            {
                var idxToReset = modeContainer.itemAtIndex(j).cb.find(confDialog.oldConfigs[j])
                if (modeContainer.itemAtIndex(j).cb.count === 1)
                {
                    modeContainer.itemAtIndex(j).cb.currentIndex = 0
                    modeContainer.itemAtIndex(j).cb.editText = confDialog.oldConfigs[j]
                }
                else if (idxToReset >= 0)
                    modeContainer.itemAtIndex(j).cb.currentIndex = idxToReset
            }
        }
    }

    Connections {
        target: cppInterface
        onConfigNotLoaded: {
            console.log("config not loaded",name)
            notloadedMessage.name = name
            notloadedMessage.visible = true
        }

        onConfigSaved: {
            savedMessage.location = path
            savedMessage.open()
        }
    }


    MessageDialog {
        id: notloadedMessage
         property string name: ""
        title: "Error loading data!"
        text: "Could not load data from file."
    }

    MessageDialog {
        id: savedMessage
        property string location: ""
        title: "Data saved!"
        text: "Data successfully saved to:\n"+location
    }

    ListView {
        id: modeContainer
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        Material.theme: globalTheme
        Material.accent: globalAccent
        model: cppInterface.configData

        Text {
            id: idleText
            //anchors.fill: parent
            anchors.centerIn: parent
            font.pointSize: fontSize
            text: "Not connected to a device."
            visible: modeContainer.count > 0
        }
        onModelChanged: {
            if (modeContainer.count == 0
                    || cppInterface.socketState == 0
                    || cppInterface.socketState == 6)
            {
                idleText.visible = true
            }
            else idleText.visible = false
        }

        delegate: Rectangle {
            id: delegateRect
            width: modeContainer.width
            height: modename.font.pixelSize*2
            color: "transparent"
            property string cppConfig: modelData.currentConfig
            property string name: modelData.modeName
            property string command: modelData.modeCommand
            property string edittext: dropDownMenu.editText
            property string currentText: dropDownMenu.currentText
            property alias cb: dropDownMenu

            onCppConfigChanged: {
                if (dropDownMenu.editable)
                {
                    dropDownMenu.editText = cppConfig
                }
                else {
                    var i =  dropDownMenu.find(cppConfig)
                    if ( i >= 0 )
                        dropDownMenu.currentIndex = i
                    else console.log("Config:",cppConfig,"isn't equal to an element")
                }
            }
            Text {
                id: modename
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: delegateRect.name
                font.pointSize: fontSize
                color: Material.theme == Material.Light ? "black" : "white"
            }
            ComboBox {
                id: dropDownMenu
                width: parent.width/4
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                model: modelData.modeValues
                editable: (dropDownMenu.count === 1)
            }
        }
    }

    RoundButton {
        id: applyButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        width: applyText.width*2
        height: applyText.height*2
        property variant configs: new Array(0)
        property bool configChanged: false

        onClicked: {
            for (var i=0;i<modeContainer.count;i++) {
                cppInterface.appendConfigCommand(modeContainer.itemAtIndex(i).command,
                                                 modeContainer.itemAtIndex(i).edittext)
            }
            cppInterface.mainState = state_REQUESTED
        }
        Text {
            id: applyText
            anchors.centerIn: parent
            text: "Apply"
            color: confColor
            font.pixelSize: fontSize-2
            enabled: !cppInterface.sendingActive
        }
    }

    RoundButton {
        id: cancelButton
        anchors.right: applyButton.left
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        width: applyButton.width
        height: applyButton.height
        onClicked: {
            confDialog.visible = false
        }
        Text {
            anchors.centerIn: parent
            text: "Close"
            color: confColor
            font.pixelSize: fontSize-2
        }
    }

    RoundButton {
        id: saveButton
        anchors.right: cancelButton.left
        anchors.bottom: parent.bottom
        anchors.rightMargin: 30
        anchors.bottomMargin: 10
        width: applyButton.width
        height: applyButton.height
        onClicked: {
            for (var i=0;i<modeContainer.count;i++)
                cppInterface.saveConfigSetting(modeContainer.itemAtIndex(i).command,modeContainer.itemAtIndex(i).edittext)
        }
        Text {
            anchors.centerIn: parent
            text: "Save"
            color: confColor
            font.pixelSize: fontSize-2
        }
    }

    RoundButton {
        id: loadButton
        anchors.right: saveButton.left
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        width: applyButton.width
        height: applyButton.height
        onClicked: {
            for (var i=0;i<modeContainer.count;i++)
                cppInterface.loadConfigSetting(modeContainer.itemAtIndex(i).command)
            //cppInterface.mainState = state_REQUESTED
        }
        Text {
            anchors.centerIn: parent
            text: "Load"
            color: confColor
            font.pixelSize: fontSize-2
        }
    }
}
