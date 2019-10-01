import QtQuick 2.12
import QtQuick.Controls 2.12

import QtQuick.Controls.Material 2.13

import QtQuick.Dialogs 1.3



ApplicationWindow {
    id: fileroot
    visible: false
    title: "File dialog"
    modality: Qt.ApplicationModal
    flags: Qt.Dialog
    height: if (fileList.count > 0) root.height*3/5//(elementHeight+fileList.spacing)*fileList.count+
    // headerRect.height+(fileList.count-1)*margin+2*margin
            else headerRect.height+2*margin+btnHeight+2*margin
    width: root.width/5
    Material.theme: globalTheme
    Material.accent: globalAccent

    property string defaultName
    property string defaultPath
    property int fontSize: 20
    property int margin: 10
    property int elementHeight: 50
    property int btnHeight: 30
    property alias explorer: fileExplorer

    Rectangle {
        id: headerRect
        width: fileroot.width
        height: fontSize*2
        color: Qt.darker("steelblue")
        radius: 10
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: margin
        Text {
            id: headertxt
            anchors.centerIn: headerRect
            color: "white"
            text: if (fileList.count > 0) "Choose which data should be saved:"
                  else "Not connected to a device."
            font.pixelSize: fontSize
        }
    }

    ListView {
        id: fileList
        model: cppInterface.graphData
        orientation: Qt.Vertical
        anchors.top: headerRect.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: okbtn.top
        width: parent.width
        height: parent.height-headerRect.height-fileroot.btnHeight
        //fileList.count*(fileroot.elementHeight+fileList.spacing)
        anchors.margins: margin
        anchors.topMargin: 10
        // clip: true
        spacing: margin/2

        delegate: Rectangle {
            id: delegateRect
            width: fileList.width
            height: visible ? fileroot.elementHeight : 0
            color: Material.theme == Material.Light ? "lightgray" : "black"
            radius: headerRect.radius/2
            visible: (cppInterface.mainState === state_RUN ||
                      cppInterface.mainState === state_UPDATED ||
                      cppInterface.mainState === state_REQUESTED ||
                      cppInterface.mainState === state_STOP)
            property bool isChecked: delegateCB.checked
            property string listName: modelData.dataName

            CheckBox {
                id: delegateCB
                anchors.left: delegateRect.left
                anchors.leftMargin: 20
                anchors.verticalCenter: delegateRect.verticalCenter
                text: listName

                checkable: true
                checked: true
                font.pixelSize: fontSize
            }
        }
    }

    Rectangle {
        id: cancelbtn
        height: fileroot.btnHeight
        width: parent.width/3
        anchors.top: if (fileList.count == 0) headerRect.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: margin
        anchors.leftMargin: 20
        color: Qt.darker("steelblue")
        radius: margin
        Text {
            anchors.centerIn: cancelbtn
            font.pixelSize: fontSize
            text: qsTr("Cancel")
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("cancel clicked")
                fileroot.visible = false
            }
        }
    }
    Rectangle {
        id: okbtn
        height: fileroot.btnHeight
        width: parent.width/3
        anchors.right: parent.right
        anchors.top: if (fileList.count == 0) headerRect.bottom
        anchors.bottom: parent.bottom
        anchors.margins: margin
        anchors.rightMargin: 20
        color: Qt.darker("steelblue")
        radius: margin
        Text {
            anchors.centerIn: okbtn
            font.pixelSize: fontSize
            text: qsTr("Ok")
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("ok clicked")
                fileExplorer.open();
            }
        }
    }

    MessageDialog {
        id: messageDialog
        property string location: ""

        title: "Data saved!"
        text: "Data successfully saved to: "+location
        onAccepted: {
            console.log("Data successfully saved to:",fileExplorer.fileUrl)
            fileroot.visible = false
        }
    }

    FileDialog {
        id: fileExplorer
        title: "Please choose a location for saving all data"
        folder: shortcuts.home
        selectFolder: true
        visible: false



        onAccepted: {
            console.log("You chose: " + fileExplorer.fileUrl) // fileUrls
            for (var i=0;i<fileList.count;i++)
            {
                if (fileList.itemAtIndex(i).isChecked)
                {
                    for (var j=0;j<dragGrid.count;j++)
                    {
                        if (fileList.itemAtIndex(i).listName === dragGrid.itemAtIndex(j).identifier) {
                            cppInterface.saveDataToFile(fileExplorer.fileUrl,
                                                        fileList.itemAtIndex(i).listName,
                                                        dragGrid.itemAtIndex(j).series.count)
                        }
                    }
                    root.viewStopped = false
                    var path = fileExplorer.fileUrl.toString();
                    path = path.replace(/^(file:\/{3})/,"");

                    messageDialog.location = decodeURIComponent(path)
                    messageDialog.visible = true

                }
            }
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
