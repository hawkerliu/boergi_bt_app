//import btmanager 1.0
//import "."

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Window 2.13
import QtCharts 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls.Material 2.13
import QtQml.Models 2.13
import QtQuick.Dialogs 1.3

// balint
//import QtQuick 2.9
//import QtQuick.Controls 2.2
//import QtQuick.Window 2.3
//import QtCharts 2.0
//import QtQuick.Controls.Styles 1.4
//import QtQuick.Layouts 1.3

ApplicationWindow {
    id: root
    visible: true
    width: 1920
    height: 1080
    title: qsTr("Infineon Sensor Vision")
    visibility: "Maximized"//"FullScreen"
    modality: Qt.WindowModal
    font.capitalization: Font.MixedCase
    color: "darkgray"

    Material.theme: globalTheme
    Material.accent: globalAccent
    Material.primary: Material.LightBlue
    Material.elevation: 6

    background: bgImg

    property int state_UNCONNECTED: 0
    property int state_INIT:        1
    property int state_RUN:         2
    property int state_UPDATED:     3
    property int state_REQUESTED:   4
    property int state_STOP:        5
    property int state_FAILED:      6

    property int themeEnum: ChartView.ChartThemeLight
    property int globalTheme: Material.Light
    property int globalAccent: Material.LightBlue

    property int fs: 0
    property bool theme: true
    property bool bgVisible: true

    property bool viewStopped: false

    onClosing: cppInterface.exitApplication()

    Image {
        id: bgImg
        height: root.height/2
        width: root.width/2
        anchors.centerIn: parent
        visible: root.bgVisible
        source: "pics/Infineon-Logo.png"
    }

    header: Rectangle {
        id: buttonBG
        height: 50
        width: parent.width
        color: root.themeEnum === ChartView.ChartThemeLight ? "lightgray" : Qt.darker( "darkgray" )


        ListView {
            id: buttonList
            anchors.fill: parent
            model: cppInterface.graphNames
            orientation: Qt.Horizontal

            delegate: CheckBox {
                id: delegatedCB
                width: root.width/buttonList.count//100
                height: buttonBG.height
                text: modelData
                checkable: true
                checked: true

                onClicked: {

                    if (checked) {
                        // graphData -> graphDataToShow
                        cppInterface.showData(modelData)

                        //grid.itemAtIndex(0).visible = true
                    }
                    else {
                        // graphDataToShow -> graphData
                        cppInterface.hideData(modelData)

                        //grid.itemAtIndex(0).visible = false
                    }
                    //grid.update()
                    //console.log("dimension:",grid.itemAtIndex(0).width,grid.itemAtIndex(0).height)
                }
            }
        }
    }

    GridView {
        id: dragGrid
        property int visibleItems: dragGrid.count
        property int space: 5
        property int updateCounter: 0
        property bool graphsVisible: true
        anchors.fill: parent
        flow: GridView.FlowLeftToRight
        layoutDirection: Qt.LeftToRight
        verticalLayoutDirection: GridView.TopToBottom
        interactive: false
        cellWidth:
            if (dragGrid.visibleItems === 0) { 0 }
            else if (dragGrid.visibleItems === 1) {
                parent.width
            }
            else if (dragGrid.visibleItems <= 3) {
                parent.width/(dragGrid.visibleItems)
            }
            else {
                parent.width/Math.ceil(dragGrid.visibleItems/2)
            }
        cellHeight:
            if (dragGrid.visibleItems === 0) { 0 }
            else if (dragGrid.visibleItems <= 3) {
                parent.height
            }
            else {
                parent.height/Math.ceil(dragGrid.visibleItems/Math.ceil(dragGrid.visibleItems/2))
            }

        onModelChanged: {
            console.log("GridView (new) item count:",dragGrid.count)
        }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
        }

        model: DelegateModel {
            id: visualModel
            model: cppInterface.graphData
            delegate: DropArea {
                id: dataDelegate
                property string identifier: modelData.dataName
                property alias series: graph.seriesData
                width: if (!visible) {0}
                       else dragGrid.cellWidth
                height: if (!visible) {0}
                        else  dragGrid.cellHeight
                visible: dragGrid.graphsVisible

                onEntered: visualModel.items.move(drag.source.visualIndex, graph.visualIndex)
                property int visualIndex: DelegateModel.itemsIndex
                Binding { target: graph; property: "visualIndex"; value: visualIndex }

                DataView {
                    id: graph
                    //                    property string identifier: modelData.dataName
                    property int visualIndex: 0
                    width: dragGrid.cellWidth-dragGrid.space
                    height: dragGrid.cellHeight-dragGrid.space
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        verticalCenter: parent.verticalCenter
                    }
                    radius: dragGrid.space

                    // seriesData.color: model.color
//                    onLineStyleChanged: {
//                        seriesData.color = modelData.lineColor
//                        console.log(identifier,"line color changed to",modelData.lineColor)
//                    }



//                    onLineColorChanged: {
//                        seriesData.color = modelData.lineColor
//                        //lineColor = sigcolor//modelData.color
//                        console.log(identifier,"line color changed to",modelData.lineColor)
//                    }




                    dataChart.title: if (modelData.dataFrequency > 0)
                                         "Sampling frequency: "+Math.round(modelData.dataFrequency)+" Hz"
                                     else " "
                    titleX: "Time [s]"
                    titleY: modelData.dataName+" ["+modelData.dataUnit+"]"
                    miny: modelData.minimumY
                    maxy: modelData.maximumY
                    minx: modelData.minimumX
                    maxx: modelData.maximumX
                    //            onMaxyChanged: console.log("Max Y of",modelData.dataName,"changed to:",modelData.maximumY)
                    //            onMinyChanged: console.log("Min Y of",modelData.dataName,"changed to:",modelData.minimumY)
                    //            onMaxxChanged: console.log("Max X of",modelData.dataName,"changed to:",modelData.maximumX)
                    //            onMinxChanged: console.log("Min X of",modelData.dataName,"changed to:",modelData.minimumX)

                    DragHandler {
                        id: dragHandler
                    }

                    Drag.active: dragHandler.active
                    Drag.source: graph
                    Drag.hotSpot.x: graph.width/2
                    Drag.hotSpot.y: graph.height/2


                    states: [
                        State {
                            when: graph.Drag.active
                            ParentChange {
                                target: graph
                                parent: dragGrid
                            }

                            AnchorChanges {
                                target: graph
                                anchors.horizontalCenter: undefined
                                anchors.verticalCenter: undefined
                            }
                        }
                    ]
                }
            }
        }
    }


    /* CONNECTION TO CPP */
    Connections {
        id: connectBlock
        target: cppInterface
        onNewPointCollected: {
            if (!root.viewStopped)
            {
                for (var i=0;i<dragGrid.count;i++)
                {
                    if (modelName === dragGrid.itemAtIndex(i).identifier)
                    {
                        cppInterface.updateData(dragGrid.itemAtIndex(i).series,modelName)
                    }
                }
            }
        }

        //        onOpenFileDialog: {
        //            console.log("file dialog",name,path)
        //            fileDialog.defaultName = name
        //            fileDialog.defaultPath = path
        //            fileDialog.visible = true
        //        }

        onUpdateFrequency: {
            root.fs = Math.round(f)
            console.log("Frequency:",root.fs)
        }

        onMainStateChanged: {
            console.log("Main State changed to:",cppInterface.mainState)
            if (cppInterface.mainState === state_RUN)
            {
                console.log("state to RUN")
                root.bgVisible = false
                dragGrid.graphsVisible = true
            }
            if (cppInterface.mainState === state_UPDATED)
            {
                console.log("state to UPDATED")
                //                // Modes got updated, give values to sliders

                //                // clear chartview and reset booleans
            }
        }

        //        onSomeBufferHasFreshDataChanged:
        //        {
        //            if (cppInterface.someBufferHasFreshData === TRUE)
        //            {
        //                //there is stuff -> push them each as signals..
        //                cppInterface.getAllFromSomeBuffer()
        //                //give me only the last one
        //                var tmpString = cppInterface.getLastFromSomeBuffer();
        //                //put one on log
        //                //console.log(cppInterface.getLastFromSomeBuffer()); // ***********
        //            }
        //        }

    }


    DeviceScan {
        id: deviceDialog
        visible: false
    }

    ModeConfig {
        id: configDialog
        visible: false

    }

    GraphSettings {
        id: settingsDialog
        visible: false

    }

    CustomFileDialog {
        id: fileDialog
        visible: false
        onVisibleChanged: {
            if (!visible) root.viewStopped = false;
        }
    }

    menuBar: MenuBar {
        id: mainBar
        font.pixelSize: 18
        property string cntState: "Not connected"
        Menu {
            title: qsTr("&File")
            Action { text: qsTr("&Save ...")
                onTriggered: {
                    root.viewStopped = true;
                    mainBar.menuAt(3).actionAt(0).checked = true
                    fileDialog.visible = true
                }
            }

            //Action { text: qsTr("Save &As...") }
            MenuSeparator { }
            Action { text: qsTr("&Quit")
                onTriggered: root.close()
            }

        }
        Menu {
            id: connectMenu
            title: qsTr("&Connection")

            Action { text: qsTr("&Connection Manager")
                onTriggered: deviceDialog.visible = true
            }
            MenuSeparator { }



            Text {
                id: connectState
                x: (parent.width-implicitWidth)/2
                // y: parent.height//-implicitHeight
                anchors.leftMargin: 5

                color:
                    if (mainBar.cntState === "Connected") "green"
                    else if (mainBar.cntState === "Disconnected") "red"
                    else "black"

                text: qsTr(mainBar.cntState)
            }

        }
        Menu {
            title: qsTr("&Edit")
            Action { text: qsTr("&Configuration")
                onTriggered: configDialog.visible = true
            }
        }
        Menu {
            title: qsTr("&View")
            id: viewMenu

            Action { text: qsTr("&Pause view")
                id: pauseAct
                checkable: true
                enabled:  if (mainBar.cntState === "Connected") true
                          else false
                onTriggered: {
                    if (checked) root.viewStopped = true;
                    else root.viewStopped = false;
                }
            }

            Action { text: qsTr("&Reset data")
                id: resetAct
                enabled: pauseAct.enabled
                onTriggered: {
                    cppInterface.resetData()
                }
            }
        }
        Menu {
            title: qsTr("&Layout")
            ActionGroup {
                id: themeActions
            }
            Action { text: qsTr("&Light Theme")
                ActionGroup.group: themeActions
                checkable: true
                checked: true
                onTriggered: {
                    themeEnum = ChartView.ChartThemeLight
                    globalTheme = Material.Light
                    globalAccent = Material.LightBlue
                    if (!checked) checked = true
                }
            }
            Action { text: qsTr("&Dark Theme")
                ActionGroup.group: themeActions
                checkable: true
                checked: false
                onTriggered: {
                    themeEnum = ChartView.ChartThemeDark
                    globalTheme = Material.Dark
                    globalAccent = Material.Green
                    if (!checked) checked = true
                }
            }
            MenuSeparator {}
            Action { text: qsTr("&Graph settings")
                onTriggered: {
                    settingsDialog.visible = true
                }
            }
        }
    }






}
