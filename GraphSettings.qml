import QtQuick 2.13
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Material 2.13
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.12


import QtQuick.Window 2.13
import QtGraphicalEffects 1.13
import QtQuick.Layouts 1.13




ApplicationWindow {
    id: settingsDialog
    visible: false
    title: "Graph settings"
    modality: Qt.ApplicationModal
    flags: Qt.Dialog
    Material.theme: globalTheme
    Material.accent: globalAccent
    height: txtin.height+applyButton.height*2
    width: root.width/4
    //    height: if (settingsList.count > 0)
    //                root.height/2
    //            else (txtin.height+applyButton.height+10)
    //    width: if (settingsList.count > 0)
    //               container.totalWidth
    //           else (root.width/2)
    property color confColor: "black"// globalTheme == Material.Light ? "black" : "white"
    property int fontSize: 20

    Rectangle {
        id: settingDataCount
        anchors.fill: parent
        anchors.margins: 10
        property string input: txtin.text
        property int lineHeight: settingsDialog.fontSize*1.2//settingsList.height/(settingsList.count+1)
        property int totalWidth: settingsList.columnNameWidth+settingsList.columnColorWidth+(settingsList.columnStyleWidth*1.25)
        color: "transparent"
        TextEdit {
            id: datatxt
            width: datatxt.implicitWidth
            height: datatxt.implicitHeight
            anchors.left: settingDataCount.left
            anchors.verticalCenter: txtin.verticalCenter
            readOnly: true
            focus: false
            selectByMouse: false
            font.pixelSize: settingsDialog.fontSize
            text: qsTr("Maximum datapoints:")
        }
        TextField {
            id: txtin
            placeholderText: "default: 500"
            font.pixelSize: settingsDialog.fontSize
            anchors.left: datatxt.right
            anchors.right: settingDataCount.right
            anchors.leftMargin: 10

            inputMethodHints: Qt.ImhFormattedNumbersOnly

            validator: IntValidator {id: validator; bottom: 10; top: 5000;}
            focus: false
        }



        // tried to change color and style of graphs, seems like both is read-only...
        ListView {
            id: settingsList
            visible: false
            property int columnNameWidth: 150
            property int columnColorWidth: 80
            property int columnStyleWidth: 120
            spacing: 5
            Material.theme: globalTheme
            Material.accent: globalAccent

            header: Rectangle {
                id: headerRect
                width: settingsList.width
                height: 50
                visible: settingsList.count > 0
                anchors.margins: 20
                radius: 10
                color: "transparent"
                Rectangle
                {
                    id: nameRect
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: settingsList.columnNameWidth
                    color: parent.color
                    Text {
                        id: columnName
                        anchors.centerIn: parent
                        font.pixelSize: settingsDialog.fontSize
                        text: qsTr("Name")
                    }
                }

                Rectangle {
                    id: colorRect
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: settingsList.columnColorWidth
                    color: parent.color
                    Text {
                        id: columnColor
                        anchors.centerIn: parent
                        font.pixelSize: settingsDialog.fontSize
                        text: qsTr("Color")
                    }
                }
                Rectangle {
                    id: styleRect
                    width: settingsList.columnStyleWidth
                    anchors.left: colorRect.right
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    color: parent.color
                    Text {
                        id: columnStyle
                        anchors.centerIn: parent
                        font.pixelSize: settingsDialog.fontSize
                        text: qsTr("Style")
                    }
                }
            }
            anchors.top: txtin.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            model: cppInterface.graphData
            delegate: Rectangle {
                id: settingsRect
                width: settingsList.width
                height: settingDataCount.lineHeight
                property string settingName: settingsName.text
                property string settingColor: colorStyleCb.currentText
                property int settingStyle: lineStyleCb.currentStyle
                color: "transparent"
                Rectangle {
                    id: textRect
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: colorStyleCb.left
                    width: settingsList.columnNameWidth
                    height: settingDataCount.lineHeight
                    color: "transparent"
                    // Name
                    Text {
                        id: settingsName
                        text: modelData.dataName
                        font.pixelSize: settingsDialog.fontSize
                        anchors.fill: parent
                    }
                }
                // Color
                ComboBox {
                    id: colorStyleCb
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: settingsList.columnColorWidth*parent.width*0.0025+1 //width: lineStyleCb.width*0.5
                    height: settingDataCount.lineHeight
                    model: ListModel {
                        id: colorModel
                        ListElement {
                            // @disable-check M16
                            color: "Blue" }
                        ListElement {
                            // @disable-check M16
                            color: "Green" }
                        ListElement {
                            // @disable-check M16
                            color: "Red" }
                        ListElement {
                            // @disable-check M16
                            color: "Yellow" }
                        ListElement {
                            // @disable-check M16
                            color: "Orange" }
                        ListElement {
                            // @disable-check M16
                            color: "Purple" }
                        ListElement {
                            // @disable-check M16
                            color: "Cyan" }
                        ListElement {
                            // @disable-check M16
                            color: "Magenta" }
                        ListElement {
                            // @disable-check M16
                            color: "Aquamarine" }
                        ListElement {
                            // @disable-check M16
                            color: "Indigo" }
                        ListElement {
                            // @disable-check M16
                            color: "Black" }
                        ListElement {
                            // @disable-check M16
                            color: "Violet" }
                        ListElement {
                            // @disable-check M16
                            color: "Grey" }
                        ListElement {
                            // @disable-check M16
                            color: "Springgreen" }
                        ListElement {
                            // @disable-check M16
                            color: "Salmon" }
                        ListElement {
                            // @disable-check M16
                            color: "Forestgreen" }
                        ListElement {
                            // @disable-check M16
                            color: "Pink" }
                        ListElement {
                            // @disable-check M16
                            color: "Navy" }
                        ListElement {
                            // @disable-check M16
                            color: "Goldenrod" }
                        ListElement {
                            // @disable-check M16
                            color: "Crimson" }
                        ListElement {
                            // @disable-check M16
                            color: "Teal" }
                    }
                    style: ComboBoxStyle {
                        label: Item {
                            Rectangle {
                                id: rect
                                opacity: 0.8
                                width: colorStyleCb.width*0.6
                                color: colorModel.get(colorStyleCb.currentIndex).color
                                height: colorStyleCb.height*0.65
                            }
                        }
                    }
                }

                // Style
                ComboBox {
                    id: lineStyleCb
                    anchors.left: colorStyleCb.right
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 20
                    width: settingsList.columnStyleWidth
                    height: settingDataCount.lineHeight

                    property int currentStyle: Qt.SolidLine

                    onCurrentIndexChanged: {
                        currentStyle = styleModel.get(currentIndex).style
                        //                        if (lineStyleCb.count == 5)
                        //                            console.log("Style index to",currentIndex,"Name:",styleModel.get(currentIndex).text)
                    }
                    model: ListModel {
                        id: styleModel
                        ListElement {
                            // @disable-check M16
                            text: "Solid line"; imageSource: "pics/solid.png"; style: Qt.SolidLine }
                        ListElement {
                            // @disable-check M16
                            text: "Dashed line"; imageSource: "pics/dash2.png"; style: Qt.DashLine }
                        ListElement {
                            // @disable-check M16
                            text: "Dotted line"; imageSource: "pics/dot2.png"; style: Qt.DotLine }
                        ListElement {
                            // @disable-check M16
                            text: "Dash-Dot line";imageSource: "pics/dashdot.png";  style: Qt.DashDotLine }
                        ListElement {
                            // @disable-check M16
                            text: "Dash-Dot-Dot line"; imageSource: "pics/dashdotdot.png"; style: Qt.DashDotDotLine }
                    }
                    style: ComboBoxStyle {
                        id: cbStyle

                        label: Item {
                            Image {
                                id: img
                                anchors.margins: 10

                                width: lineStyleCb.width*0.75
                                source: styleModel.get(lineStyleCb.currentIndex).imageSource
                                height: settingDataCount.lineHeight*0.8
                            }
                        }
                    }
                }
            }
        }
    }

    RoundButton {
        id: applyButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: applyText.width*2
        height: applyText.height*2
        onClicked: {
            console.log("maxData:",cppInterface.maxData,"input:",settingDataCount.input)
            cppInterface.maxData = settingDataCount.input

            //            for (var i=0;i<settingsList.count;i++)
            //            {
            //                dragGrid.itemAtIndex(i).seriesColor = settingsList.itemAtIndex(i).settingColor
            //                dragGrid.itemAtIndex(i).seriesStyle = settingsList.itemAtIndex(i).settingStyle
            //                console.log("name:",settingsList.itemAtIndex(i).settingName,
            //                            "color:",settingsList.itemAtIndex(i).settingColor,
            //                            "style:",settingsList.itemAtIndex(i).settingStyle)
            //            }
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
        id: resetButton
        anchors.right: cancelButton.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: applyButton.width
        height: applyButton.height
        onClicked: {
            cppInterface.maxData = 500
        }
        Text {
            anchors.centerIn: parent
            id: resetButtonText
            text: "Default"
            color: confColor
            font.pixelSize: fontSize-2
        }
    }

    RoundButton {
        id: cancelButton
        anchors.right: applyButton.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: applyButton.width
        height: applyButton.height
        onClicked: {
            settingsDialog.visible = false
        }
        Text {
            anchors.centerIn: parent
            text: "Close"
            color: confColor
            font.pixelSize: fontSize-2
        }
    }
}
