import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
import QmlStyle 1.0
import StartDiagnosticWindow_copy 1.0
import LoginDialog_copy 1.0
import "../Stylesheets"
import "../Authentication"
//import TextFieldDoubleValidator 1.0

ApplicationWindow {
    id:window
    objectName: "startWindow"
    visible: true
    width: 1366
    height: 768
    title: qsTr("Главное окно")
    Material.theme:model.getTheme()
    onClosing:{
        model.setQuitOnLastWindowClosed(true);

    }

    signal playThisTestClicked()

    signal lowtransferSwitchChanged(variant value)
//    signal caterpillarSwitchChanged(variant value)
//    signal reductorSwitchChanged(variant value)
//    signal dangeroilpressSwitchChanged(variant value)
//    signal antifreezeoverheatSwitchChanged(variant value)
//    signal lowantifreezeSwitchChanged(variant value)
//    signal gmtpressSwitchChanged(variant value)
//    signal turnindicatorSwitchChanged(variant value)

//    signal speedSliderChanged(variant value)
//    signal tachometerSliderChanged(variant value)
//    signal oiltempSliderChanged(variant value)
//    signal oilpressSliderChanged(variant value)
//    signal watertempslider(variant value)

    //Slot 0
    signal moduleSlot0DO0SwitchChanged(variant value)
    signal moduleSlot0DO1SwitchChanged(variant value)
    signal moduleSlot0DO2SwitchChanged(variant value)
    signal moduleSlot0DO3SwitchChanged(variant value)
    signal moduleSlot0DO4SwitchChanged(variant value)
    signal moduleSlot0DO5SwitchChanged(variant value)
    signal moduleSlot0DO6SwitchChanged(variant value)
    signal moduleSlot0DO7SwitchChanged(variant value)
    signal moduleSlot0DO8SwitchChanged(variant value)
    signal moduleSlot0DO9SwitchChanged(variant value)
    signal moduleSlot0DO10SwitchChanged(variant value)
    signal moduleSlot0DO11SwitchChanged(variant value)
    signal moduleSlot0DO12SwitchChanged(variant value)
    signal moduleSlot0DO13SwitchChanged(variant value)
    signal moduleSlot0DO14SwitchChanged(variant value)
    signal moduleSlot0DO15SwitchChanged(variant value)

    //Slot 1
    signal moduleSlot1DO0SwitchChanged(variant value)
    signal moduleSlot1DO1SwitchChanged(variant value)
    signal moduleSlot1DO2SwitchChanged(variant value)
    signal moduleSlot1DO3SwitchChanged(variant value)
    signal moduleSlot1DO4SwitchChanged(variant value)
    signal moduleSlot1DO5SwitchChanged(variant value)
    signal moduleSlot1DO6SwitchChanged(variant value)
    signal moduleSlot1DO7SwitchChanged(variant value)
    signal moduleSlot1DO8SwitchChanged(variant value)
    signal moduleSlot1DO9SwitchChanged(variant value)
    signal moduleSlot1DO10SwitchChanged(variant value)
    signal moduleSlot1DO11SwitchChanged(variant value)
    signal moduleSlot1DO12SwitchChanged(variant value)
    signal moduleSlot1DO13SwitchChanged(variant value)
    signal moduleSlot1DO14SwitchChanged(variant value)
    signal moduleSlot1DO15SwitchChanged(variant value)

    //Slot 3
    signal moduleSlot3DO0SwitchChanged(variant value)
    signal moduleSlot3DO1SwitchChanged(variant value)
    signal moduleSlot3DO2SwitchChanged(variant value)
    signal moduleSlot3DO3SwitchChanged(variant value)
    signal moduleSlot3DO4SwitchChanged(variant value)
    signal moduleSlot3DO5SwitchChanged(variant value)
    signal moduleSlot3DO6SwitchChanged(variant value)
    signal moduleSlot3DO7SwitchChanged(variant value)
    signal moduleSlot3DO8SwitchChanged(variant value)
    signal moduleSlot3DO9SwitchChanged(variant value)
    signal moduleSlot3DO10SwitchChanged(variant value)
    signal moduleSlot3DO11SwitchChanged(variant value)
    signal moduleSlot3DO12SwitchChanged(variant value)
    signal moduleSlot3DO13SwitchChanged(variant value)
    signal moduleSlot3DO14SwitchChanged(variant value)
    signal moduleSlot3DO15SwitchChanged(variant value)

    //Slot 4
    signal moduleSlot4DO0SwitchChanged(variant value)
    signal moduleSlot4DO1SwitchChanged(variant value)
    signal moduleSlot4DO2SwitchChanged(variant value)
    signal moduleSlot4DO3SwitchChanged(variant value)
    signal moduleSlot4DO4SwitchChanged(variant value)
    signal moduleSlot4DO5SwitchChanged(variant value)
    signal moduleSlot4DO6SwitchChanged(variant value)
    signal moduleSlot4DO7SwitchChanged(variant value)
    signal moduleSlot4DO8SwitchChanged(variant value)
    signal moduleSlot4DO9SwitchChanged(variant value)
    signal moduleSlot4DO10SwitchChanged(variant value)
    signal moduleSlot4DO11SwitchChanged(variant value)
    signal moduleSlot4DO12SwitchChanged(variant value)
    signal moduleSlot4DO13SwitchChanged(variant value)
    signal moduleSlot4DO14SwitchChanged(variant value)
    signal moduleSlot4DO15SwitchChanged(variant value)


    //AFC
    signal delErrChanged(variant value)
    signal afcStartStopSwitchChanged(variant value)
    signal installedFreequencyInputChanged(variant value)
    signal installedUInputChanged(variant value)

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);

        if(model.getAccess() === "Администратор"){
             adminStartWindow.close()
        }

        if(model.getOpenedWindow() === "LoginDialog"){
             logindialogwindow.close()
        }

        model.setQuitOnLastWindowClosed(false)
    }

    function delay(duration) { // In milliseconds
        var timeStart = new Date().getTime();

        while (new Date().getTime() - timeStart < duration) {
            // Do nothing
        }

        // Duration has passed
    }

    function afcDo(){
        delErrChanged(delErrSwitch.checked);
        afcStartStopSwitchChanged(afcStartStopSwitch.checked)

        installedFreequencyInputChanged(installedFreequencyInput.text)
        installedUInputChanged(installedUInput.text)

       // if()
    }

    QmlStyle{
        id: styles
    }
    StartDiagnosticWindow_copy{
        id: controller
    }
    LoginDialog_copy{
      id: model
      onEnablePasswordChanged: {
          if(enablePasswordChange == true){
              logindialogwindow.height = 400
              newPassword.visible = true
              confirmNewPassword.visible = true
          }else{
              newPassword.visible = false
              confirmNewPassword.visible = false
              logindialogwindow.height = 300
          }

      }
      onLogged: {
          logModel.append({message:model.getLogRecord()})
      }
    }

    Pane{
        id:panel
        height: 271
        width: 1366
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: model.getPasswordCorrect()
        ColumnLayout{
            id: transmission
            spacing: 20

            Item{
                id:lowtransfer
                width: 200
                height:20

                RowLayout{
                    width: 200


                    StatusIndicator {
                        id: lowtransferIndicator
                        objectName: "lowtransferIndicator"
                        anchors.left:parent.left
                    }
                    Label{
                        text:qsTr("низш. передача")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: lowtransferIndicator.right
                        anchors.leftMargin: 10
                    }
                }
            }

            Item{
                id:caterpillar
                width: 200
                height:20
                RowLayout{
                    width: 200

                    StatusIndicator {
                        id: catepillarIndicator
                        anchors.left:parent.left
                    }
                    Label{
                        text:qsTr("натяжение гусеницы")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: catepillarIndicator.right
                        anchors.leftMargin: 10
                    }
                }


            }
            Item{
                id:reductor
                width: 200
                height:20
                RowLayout{
                    width: 200

                    StatusIndicator {
                        id: reductorIndicator
                        anchors.left:parent.left
                    }
                    Label{
                        text:qsTr("редуктор СЭП")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left:reductorIndicator.right
                        anchors.leftMargin: 10
                    }
                }

            }
        }

        ColumnLayout{
            id:dangerpressure
            x: 209
            y: 4
            width: 209
            height: 93
            spacing:30
            Item{
                id:gmtpressure
                width: 200
                height:20

                RowLayout{
                    width: 200

                    StatusIndicator {
                        id: gmtpressIndicator
                        anchors.left:parent.left
                    }
                    Label{
                        text:qsTr("давление ГМТ")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left:gmtpressIndicator.right
                        anchors.leftMargin: 10
                    }
                }
            }
            Item{
                id:dangeroilpress
                width: 200
                height:20
                RowLayout{
                    width: 200

                    StatusIndicator {
                        id: dangeroilpressIndicator
                        anchors.left:parent.left
                    }

                    Label{
                        text:qsTr("авар. давление масла")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: dangeroilpressIndicator.right
                        anchors.leftMargin: 10
                    }


                }
            }

        }

        CircularGauge{
            id:speedometer
            objectName: "Speedometer"
            stepSize: 0.02
            anchors.centerIn: parent
            minimumValue: 0
            maximumValue: 120
            property bool accelerating: false
            width: 238
            height: 222
            anchors.verticalCenterOffset: 0
            anchors.horizontalCenterOffset: 271
            style:Speedometer{}

            Keys.onSpacePressed: accelerating = true
            Keys.onReleased: {
                if (event.key === Qt.Key_Space) {
                    accelerating = false;
                    event.accepted = true;
                }
            }
            Label {
                id: speedindex
                text: qsTr("км/ч")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 40
            }

            Behavior on value {
                NumberAnimation {
                    duration: 1000
                }
            }
        }

        CircularGauge {
            id: tachometer
            x: 1104
            y: 36
            width: 202
            height: 211
            minimumValue: 0
            maximumValue: 40
            stepSize: 0.02
            style:Tachometer{}
            Label {
                id: tachindex
                text: qsTr("x100 об/мин")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 70
                layer.enabled: true
            }
        }

        CircularGauge {
            id: watertemp
            x: 552
            y: 134
            minimumValue: -7
            maximumValue: 15
            stepSize: 0.5
            width: 154
            height: 134
            style:Termometer{}
            Label {
                id: twaterindex
                text: qsTr("t воды")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 50
            }
        }

        CircularGauge {
            id: oilpress
            x: 257
            y: 134
            width: 161
            height: 129
            minimumValue:0
            maximumValue:15
            style:OilPressure{}
            Label {
                id: poilindex
                text: qsTr("P масла")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 40
            }
        }

        CircularGauge {
            id: oiltemp
            x: 418
            y: 31
            width: 156
            height: 134
            minimumValue: -7
            maximumValue: 15
            style:Termometer{}
            Label {
                id: toilindex
                text: qsTr("t масла")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 50
            }
        }

        CircularGauge {
            id: gmtoil
            x: 19
            y: 134
            width: 169
            height: 129
            minimumValue: 0
            maximumValue: 6
            style:PressureGmt{}
            Label {
                id: gmtoilindex
                text: qsTr("смазка ГМТ")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 30
            }
        }

        Column {
            id: antifreeze
            x: 601
            y: 4
            width: 213
            height: 100
            spacing:30
            RowLayout{
                id:antifreezeoverheat
                width:200
                height: 20

                StatusIndicator {
                    id: antifreezeoverheatiindicator
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left:parent.left
                }
                Label{
                    id:antifreezeoverheatindex
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: antifreezeoverheatiindicator.right
                    anchors.leftMargin: 10
                    text:qsTr("перегрев охл.ж.")
                }
            }
            RowLayout{
                id:lowantifreeze
                width:200
                height: 20
                StatusIndicator{
                    id:lowantifreezeindicator
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left:parent.left
                }
                Label{
                    id:lowantifreezeindex
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: lowantifreezeindicator.right
                    anchors.leftMargin: 10
                    text:qsTr("низкий уровень охл.ж.")
                }
            }
        }

        Item{
            x: 734
            y: 197
            width:50
            height: 50
            StatusIndicator{
                 id:turnindicator
            }
//            Image{
//                id:turnindicatorimg
//                x: 8
//                y: -35
//                visible: false
//                width:60
//                height: 60
//                source: "../pics/Arrow.svg"
//            }
//            Timer{
//                id:turnindicatorTimer
//                function blink(){
//                    if(turnindicatorimg.visible === true){
//                        turnindicatorimg.visible = false;
//                    }else{
//                        turnindicatorimg.visible = true;
//                    }
//                }

//                function checkFalse(){
//                    if(!turnindicatorSwitch.checked)
//                        turnindicatorimg.visible = false;
//                }

//                interval:500
//                running:turnindicatorSwitch.checked
//                repeat:true
//                onTriggered: blink()
//                onRunningChanged: checkFalse()
//            }
        }

    }


    Pane{
        id:navbarcontainer
        anchors.top:panel.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width:300
        enabled: model.getPasswordCorrect()
        Button{
            id:playthistestbutton
            anchors.horizontalCenter: parent.horizontalCenter
            width:200
            height:60

            Label{
                id:playThisTestLabel
                width:100
                text: "Выполнить"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 20
            }
            onClicked: window.playThisTestClicked()

        }
    }

    Pane{
        id: progressbarcontainer
        width:300
        height:70
        anchors.top:navbarcontainer.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: model.getPasswordCorrect()
        ColumnLayout{
            id:prorgesscontainer
            anchors.horizontalCenter: parent.horizontalCenter
            spacing:30

            Label{
                id:testprogresslabel
                text:qsTr("Выполнение теста:  "+Math.round(testprogressSlider.value )+ "%")
            }

            ProgressBar{
                id:testprogress
                width:300
                height:20
                anchors.horizontalCenter: parent.horizontalCenter
                from:0
                to: 100
                value: testprogressSlider.value

            }
        }
    }

    ComboBox{
        id:themebox
        width: 200
        textRole: "key"
        currentIndex: model.getTheme()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: progressbarcontainer.bottom
        enabled: model.getPasswordCorrect()
        model:ListModel{
            id:themeModel
            ListElement{key:"Светлая тема";theme:Material.Light; color:"#dd000000"}
            ListElement{key:"Тёмная тема";theme:Material.Dark; color:"#ffffff"}
        }
        onCurrentIndexChanged:{
            model.changeTheme(themeModel.get(themebox.currentIndex).theme)
            window.Material.theme = model.getTheme()
        }
    }

    GroupBox{
        id: steplistContainer
        anchors.top:panel.bottom
        anchors.left: panel.left
        anchors.leftMargin: 10
        width: 530
        height: 407
        enabled: model.getPasswordCorrect()
        label:Label{
            text:"История"
        }
        ListView {
            id:history
            width: 530
            height: 407
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            model: historyModel
            clip: true
            //flickingHorizontally: true

            snapMode: ListView.SnapToItem
            delegate: Item {
                id: delegateHistory
                width: 400
                height: 20
                Text {
                    color: themebox.onCurrentIndexChanged ? themeModel.get(themebox.currentIndex).color : Speedometer.changeColorByTheme(styles.style)
                    text: message
                }
            }
        }

        ListModel {
            id: historyModel
            objectName: "historyModel"
        }
    }

    GroupBox{
        id: logContainer
        anchors.top: panel.bottom
        anchors.right: panel.right
        anchors.rightMargin: 10
        width: 530
        height: 407
        enabled: model.getPasswordCorrect()
        label:Label{
            text:"Лог обмена данными с контроллером"
        }

        ListView {
            id:log
            width: 530
            height: 407
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            model: logModel
            clip: true
            //flickingHorizontally: true

            snapMode: ListView.SnapToItem
            delegate: Item {
                id: delegateLog
                width: 400
                height: 20
                Text {
                    color: themebox.onCurrentIndexChanged ? themeModel.get(themebox.currentIndex).color : Speedometer.changeColorByTheme(styles.style)
                    text: message
                }
            }
        }

        ListModel {
            id: logModel
            objectName: "logModel"
        }
    }

    Button{
        id: loginButton
        text: "Вход/регистрация"
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.bottomMargin: 20
        anchors.bottom: parent.bottom

        onClicked:{
            model.changeOpenedWindow("StartWindow")
            pageLoader.source = "../Authentication/LoginDialog.qml";
            model.closeExecutorWindow();
        }

    }

    Button{
        id:backButton
        text:"Назад"
        visible: model.getAccess() === "Администратор" && model.getPasswordCorrect() === true ? true : false
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.bottomMargin: 20
        onClicked: {
            model.changeOpenedWindow("StartWindow")
            pageLoader.source = "AdminStartWindow.qml";
            model.closeExecutorWindow();
        }
    }

    Loader{
        id: pageLoader
    }

    Pane{
        id:openContainer
        width:openButton.width + attestationButton.width + 40
        height:40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        Button{
            id: openButton
            text: "Управление панелью"
            anchors.left:parent.left
            onClicked: testcontrols.open()
            enabled: model.getPasswordCorrect()
        }
        Button{
            id:attestationButton
            anchors.right:parent.right
            text:"Аттестация"
            onClicked: attestation.open()
            enabled: model.getPasswordCorrect()
        }

    }

    Drawer{
        id: testcontrols
        dragMargin: 20
        edge: Qt.BottomEdge
        interactive: true
        position : 1.0
        height: 422
        width: parent.width
        dim: false
        enabled: model.getPasswordCorrect()

        Pane{
            id:controlswrapper
            height: 422
            width:  1366
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: -10
            GroupBox{
                id:speedbox
                height:200
                width: 220
                anchors.topMargin: -10
                label:Label{
                    text:"Скорость"
                }
                ColumnLayout{
                    id:speed
                    Slider {
                        id: speedslider
                        objectName: "speedSlider"
                        //value: speedsliderCount.value
                        from:speedometer.minimumValue
                        to:speedometer.maximumValue
                        Label{
                            id:speedsliderindex
                            text:speedindex.text
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Label{
                            text:speedometer.minimumValue
                            anchors.left:parent.left
                        }
                        Label{
                            text:speedometer.maximumValue
                            anchors.right: parent.right
                        }
                        //onValueChanged: window.speedSliderChanged(speedslider.value)
                    }

                    TextField{
                        id: speedsliderCount
                        inputMask: "999"
                        text: Math.round(speedslider.value,-1)
                        anchors.top:speedslider.bottom
                        anchors.topMargin: 10
                        onAccepted: window.speedSliderChanged(speedsliderCount.text)
                    }
                    Button{
                        id:speedButton
                        anchors.top:speedsliderCount.bottom
                        anchors.topMargin: 10
                        text:"применить"
                        onClicked: speedometer.value = speedsliderCount.text //window.speedSliderChanged(speedsliderCount.text)
                    }


                }

            }
            GroupBox{
                id:tachbox
               height:200
                width: 220
                anchors.left:speedbox.right
                anchors.topMargin: -10
                anchors.leftMargin: 10
                label:Label{
                    text:"Обороты"
                }
                ColumnLayout{
                    id: revs
                    Slider {
                        id: tachometerslider
                        value: 0.5
                        from:tachometer.minimumValue
                        to:tachometer.maximumValue
                        Label{
                            id:tachometersliderindex
                            text:qsTr("об/мин")
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Label{
                            text:tachometer.minimumValue
                            anchors.left:parent.left
                        }
                        Label{
                            text:tachometer.maximumValue
                            anchors.right: parent.right
                        }
                    }
                    TextField{
                        id:  tachometersliderCount
                        inputMask: "999"
                        text: Math.round(tachometerslider.value,-1)
                        anchors.top:tachometerslider.bottom
                        anchors.topMargin: 10
                        //onAccepted: window.speedSliderChanged(speedsliderCount.text)
                    }
                    Button{
                        id:revsButton
                        anchors.top:tachometersliderCount.bottom
                        anchors.topMargin: 10
                        text:"применить"
                        onClicked:tachometer.value = tachometersliderCount.text //window.speedSliderChanged(speedsliderCount.text)
                    }

               }
            }

            GroupBox{
                 id:gmtbox
                 height:200
                 width: 220
                 anchors.left:tachbox.right
                 anchors.topMargin: -10
                 anchors.leftMargin: 10
                 label:Label{
                     text:"Смазка ГМТ"
                 }
                 ColumnLayout{
                     id:gmtPress
                     Slider {
                         id: gmtoilslider
                         value: 0.5
                         from: gmtoil.minimumValue
                         to: gmtoil.maximumValue
                         Label{
                             id:gmtoilsliderindex
                             text: "*0.1 MPa"
                             anchors.horizontalCenter: parent.horizontalCenter
                         }
                         Label{
                             text:gmtoil.minimumValue
                             anchors.left:parent.left
                         }
                         Label{
                             text:gmtoil.maximumValue
                             anchors.right: parent.right
                         }
                     }
                     TextField{
                         id:  gmtoilsliderCount
                         inputMask: "999"
                         text: Math.round(gmtoilslider.value,-1)
                         anchors.top:gmtoilslider.bottom
                         anchors.topMargin: 10
                         //onAccepted: window.speedSliderChanged(speedsliderCount.text)
                     }
                     Button{
                         id:gmtoilsliderButton
                         anchors.top:gmtoilsliderCount.bottom
                         anchors.topMargin: 10
                         text:"применить"
                         onClicked: gmtoil.value=gmtoilsliderCount.text//window.speedSliderChanged(speedsliderCount.text)
                     }

                 }
            }

            GroupBox{
                id:oilpressbox
                height:200
                width: 220
                anchors.left:gmtbox.right
                anchors.topMargin: -10
                anchors.leftMargin: 10
                label:Label{
                    text:"Давление масла"
                }
                ColumnLayout{
                   id:pressOil
                   Slider {
                       id: oilpressslider
                       value: 0.5
                       from: oilpress.minimumValue
                       to: oilpress.maximumValue
                       Label{
                           id:oilpresssliderindex
                           text:"*0.1 MPa"
                           anchors.horizontalCenter: parent.horizontalCenter
                       }
                       Label{
                           text:oilpress.minimumValue
                           anchors.left:parent.left
                       }
                       Label{
                           text:oilpress.maximumValue
                           anchors.right: parent.right
                       }

                   }
                   TextField{
                       id:  oilpresssliderCount
                       inputMask: "999"
                       text: Math.round(oilpressslider.value,-1)
                       anchors.top:oilpressslider.bottom
                       anchors.topMargin: 10
                       //onAccepted: window.speedSliderChanged(speedsliderCount.text)
                   }
                   Button{
                       id:oilpresssliderButton
                       anchors.top:oilpresssliderCount.bottom
                       anchors.topMargin: 10
                       text:"применить"
                       onClicked: oilpress.value = oilpresssliderCount.text //window.speedSliderChanged(speedsliderCount.text)
                   }


                }

            }

            GroupBox{
                id:oiltempbox
                height:200
                width: 220
                anchors.left:oilpressbox.right
                anchors.topMargin: -10
                anchors.leftMargin: 10
                label:Label{
                    text:"Температура масла"
                }
                ColumnLayout{
                    id:oiltemperature
                    Slider {
                        id: oiltempslider
                        value: 0.5
                        from: oiltemp.minimumValue
                        to: oiltemp.maximumValue
                        Label{
                            id:oiltempsliderindex
                            text: "*10°C"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Label{
                            text:oiltemp.minimumValue
                            anchors.left:parent.left
                        }
                        Label{
                            text:oiltemp.maximumValue
                            anchors.right: parent.right
                        }
                    }
                    TextField{
                        id:  oiltempsliderCount
                        inputMask: "999"
                        text: Math.round(oiltempslider.value,-1)
                        anchors.top:oiltempslider.bottom
                        anchors.topMargin: 10
                        onAccepted: oiltemp = oilpresssliderCount.text //window.speedSliderChanged(oiltempsliderCount.text)
                    }
                    Button{
                        id:oiltempsliderButton
                        anchors.top:oiltempsliderCount.bottom
                        anchors.topMargin: 10
                        text:"применить"
                        onClicked: oiltemp.value = oiltempsliderCount.text //window.speedSliderChanged(speedsliderCount.text)
                    }

                }
            }

             GroupBox{
                 id:watertempbox
                 height:200
                 width: 220
                 anchors.top:oiltempbox.bottom
                 label:Label{
                     text:"Температура воды"
                 }
                 ColumnLayout{
                     id:watertemperature
                     Slider {
                         id: watertempslider
                         value: 0.5
                         from:watertemp.minimumValue
                         to:watertemp.maximumValue
                         Label{
                             id:watertempsliderindex
                             text:"*10°C"
                             anchors.horizontalCenter: parent.horizontalCenter
                         }
                         Label{
                             text:watertemp.minimumValue
                             anchors.left:parent.left
                         }
                         Label{
                             text:watertemp.maximumValue
                             anchors.right: parent.right
                         }
                     }
                     TextField{
                         id:  watertempsliderCount
                         inputMask: "999"
                         text: Math.round(watertempslider.value,-1)
                         anchors.top:watertempslider.bottom
                         anchors.topMargin: 10
                         //onAccepted: window.speedSliderChanged(oiltempsliderCount.text)
                     }
                     Button{
                         id:watertempsliderButton
                         anchors.top:watertempsliderCount.bottom
                         anchors.topMargin: 10
                         text:"применить"
                         onClicked: watertemp.value = watertempsliderCount.text //window.speedSliderChanged(speedsliderCount.text)
                     }
                 }

             }

             GroupBox{
                 id:testprogressbox
                 height:200
                 width: 220
                 anchors.left:watertempbox.right
                 anchors.leftMargin: 10
                 anchors.top:oiltempbox.bottom
                 label:Label{
                     text:"Прогресс выполнения теста"
                 }
                 Slider {
                     id: testprogressSlider
                     value: 0.5
                     from: testprogress.from
                     to:testprogress.to
                     Label{
                         text:qsTr("%")
                         anchors.horizontalCenter: parent.horizontalCenter
                     }
                     Label{
                         text:testprogress.minimumValue
                         anchors.left:parent.left
                     }
                     Label{
                         text:testprogress.maximumValue
                         anchors.right: parent.right
                     }
                 }
                 TextField{
                     id:  testprogressSliderCount
                     inputMask: "999"
                     text: Math.round(testprogressSlider.value,-1)
                     anchors.top:testprogressSlider.bottom
                     anchors.topMargin: 10
                     //onAccepted: window.speedSliderChanged(oiltempsliderCount.text)
                 }
                 Button{
                     id:testprogressSliderButton
                     anchors.top:testprogressSliderCount.bottom
                     anchors.topMargin: 10
                     text:"применить"
                     onClicked: testprogress.value = testprogressSliderCount.text //window.speedSliderChanged(speedsliderCount.text)
                 }
             }
             GroupBox{
                 id:switchbox
                 anchors.left:testprogressbox.right
                 anchors.leftMargin: 10
                 anchors.top:oiltempbox.bottom
                 height:200
                 width:650
                 label:Label{
                    text:"Управление индикаторами"
                 }
                 ColumnLayout{
                     id:switchCol
                     width:200
                     Switch {
                         id: lowtransferSwitch
                         objectName: "lowtransferSwitch"
                         text: qsTr("низш. передача")
                         //checked:lowtransferIndicator.active
                         onCheckedChanged: lowtransferIndicator.active = lowtransferSwitch.checked
                     }

                     Switch {
                         id: caterpillarSwitch
                         text: qsTr("натяжение гусеницы")
                         onCheckedChanged: catepillarIndicator.active = caterpillarSwitch.checked
                     }

                     Switch {
                         id: reductorSwitch
                         text: qsTr("редуктор СЭП.")
                         onCheckedChanged: reductorIndicator.active = reductorSwitch.checked
                     }




                 }
                 ColumnLayout{
                     id:switchCol2
                     width:200
                     anchors.left: switchCol.right
                     anchors.leftMargin: 10
                     Switch {
                         id: gmtpressSwitch
                         text: qsTr("давление ГМТ")
                         onCheckedChanged: gmtpressIndicator.active = gmtpressSwitch.checked
                     }
                     Switch {
                         id: dangeroilpressSwitch
                         text: qsTr("авар. давление масла")
                         onCheckedChanged: dangeroilpressIndicator.active = dangeroilpressSwitch.checked
                     }

                     Switch{
                         id:antifreezeoverheatSwitch
                         text:antifreezeoverheatindex.text
                         onCheckedChanged: antifreezeoverheatiindicator.active = antifreezeoverheatSwitch.checked
                     }
                 }
                 ColumnLayout{
                     id:switchCol3
                     anchors.left: switchCol2.right
                     width:200
                     Switch{
                         id:lowantifreezeSwitch
                         text:lowantifreezeindex.text
                         onCheckedChanged: lowantifreezeindicator.active = lowantifreezeSwitch.checked
                     }
                     Switch{
                         id:turnindicatorSwitch
                         text:qsTr("сигнал поворота")
                         onCheckedChanged: turnindicator.active = turnindicatorSwitch.checked
                     }
                 }

             }
             }

    }

    Drawer{
        id:attestation
        dragMargin: 20
        edge:Qt.BottomEdge
        interactive: true
        position: 1.0
        height:497
        width:parent.width
        dim:false
        enabled: model.getPasswordCorrect()

        Pane{
            id:attestationcontrolswrapper
            width:1366
            height:497
            anchors.horizontalCenter: parent.horizontalCenter
            GroupBox{
                id:moduleSlot0
                height:475
                width:185
                label:Label{
                    text:"DO0"
                }
                ColumnLayout{
                    id: moduleSlot0_0
                    height:447
                    width:90
                    anchors.left: parent.left
                    anchors.leftMargin: -5

                    Switch{
                        id:moduleSlot0DO0Switch
                        objectName: "moduleSlot0DO0Switch"
                        text:"ch0"
                        onCheckedChanged: moduleSlot0DO0SwitchChanged(moduleSlot0DO0Switch.checked)
                    }

                    Switch{
                        id:moduleSlot0DO1Switch
                        objectName: "moduleSlot0DO1Switch"
                        text:"ch1"
                        onCheckedChanged: moduleSlot0DO1SwitchChanged(moduleSlot0DO1Switch.checked)
                    }

                    Switch{
                        id:moduleSlot0DO2Switch
                        objectName: "moduleSlot0DO2Switch"
                        text:"ch2"
                        onCheckedChanged: moduleSlot0DO2SwitchChanged(moduleSlot0DO2Switch.checked)
                    }

                    Switch{
                        id:moduleSlot0DO3Switch
                        objectName: "moduleSlot0DO3Switch"
                        text:"ch3"
                        onCheckedChanged: moduleSlot0DO3SwitchChanged(moduleSlot0DO3Switch.checked)
                    }

                    Switch{
                        id:moduleSlot0DO4Switch
                        objectName: "moduleSlot0DO4Switch"
                        text:"ch4"
                        onCheckedChanged: moduleSlot0DO4SwitchChanged(moduleSlot0DO4Switch.checked)
                    }

                    Switch{
                        id:moduleSlot0DO5Switch
                        objectName: "moduleSlot0DO5Switch"
                        text:"ch5"
                        onCheckedChanged: moduleSlot0DO5SwitchChanged(moduleSlot0DO5Switch.checked)
                    }

                    Switch{
                        id:moduleSlot0DO6Switch
                        objectName: "moduleSlot0DO6Switch"
                        text:"ch6"
                        onCheckedChanged: moduleSlot0DO6SwitchChanged(moduleSlot0DO6Switch.checked)
                    }

                    Switch{
                        id:moduleSlot0DO7Switch
                        objectName: "moduleSlot0DO7Switch"
                        text:"ch7"
                        onCheckedChanged: moduleSlot0DO7SwitchChanged(moduleSlot0DO7Switch.checked)
                    }
                }
                ColumnLayout{
                    id:moduleSlot0_1
                    height:447
                    width:90
                    anchors.left: moduleSlot0_0.right
                    Switch{
                        id:moduleSlot0DO8Switch
                        objectName: "moduleSlot0DO8Switch"
                        text:"ch8"
                        onCheckedChanged: moduleSlot0DO8SwitchChanged(moduleSlot0DO8Switch.checked)
                    }
                    Switch{
                        id:moduleSlot0DO9Switch
                        objectName: "moduleSlot0DO9Switch"
                        text:"ch9"
                        onCheckedChanged: moduleSlot0DO9SwitchChanged(moduleSlot0DO9Switch.checked)
                    }
                    Switch{
                        id:moduleSlot0DO10Switch
                        objectName: "moduleSlot0DO10Switch"
                        text:"ch10"
                        onCheckedChanged: moduleSlot0DO10SwitchChanged(moduleSlot0DO10Switch.checked)
                    }
                    Switch{
                        id:moduleSlot0DO11Switch
                        objectName: "moduleSlot0DO11Switch"
                        text:"ch11"
                        onCheckedChanged: moduleSlot0DO11SwitchChanged(moduleSlot0DO11Switch.checked)
                    }
                    Switch{
                        id:moduleSlot0DO12Switch
                        objectName: "moduleSlot0DO12Switch"
                        text:"ch12"
                        onCheckedChanged: moduleSlot0DO12SwitchChanged(moduleSlot0DO12Switch.checked)
                    }
                    Switch{
                        id:moduleSlot0DO13Switch
                        objectName: "moduleSlot0DO13Switch"
                        text:"ch13"
                        onCheckedChanged: moduleSlot0DO13SwitchChanged(moduleSlot0DO13Switch.checked)
                    }
                    Switch{
                        id:moduleSlot0DO14Switch
                        objectName: "moduleSlot0DO14Switch"
                        text:"ch14"
                        onCheckedChanged: moduleSlot0DO14SwitchChanged(moduleSlot0DO14Switch.checked)
                    }
                    Switch{
                        id:moduleSlot0DO15Switch
                        objectName: "moduleSlot0DO15Switch"
                        text:"ch15"
                        onCheckedChanged: moduleSlot0DO15SwitchChanged(moduleSlot0DO15Switch.checked)
                    }
                }

            }
            GroupBox{
                id:moduleSlot1
                height:475
                width:185
                anchors.left:moduleSlot0.right
                anchors.leftMargin: 10
                label:Label{
                    text:"DO1"
                }
                ColumnLayout{
                    id: moduleSlot1_0
                    height:447
                    width:90
                    anchors.left: parent.left
                    anchors.leftMargin: -5
                    Switch{
                        id:moduleSlot1DO0Switch
                        objectName: "moduleSlot1DO0Switch"
                        text:"ch0"
                        onCheckedChanged: moduleSlot1DO0SwitchChanged(moduleSlot1DO0Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO1Switch
                        objectName: "moduleSlot1DO1Switch"
                        text:"ch1"
                        onCheckedChanged: moduleSlot1DO1SwitchChanged(moduleSlot1DO1Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO2Switch
                        objectName: "moduleSlot1DO2Switch"
                        text:"ch2"
                        onCheckedChanged: moduleSlot1DO2SwitchChanged(moduleSlot1DO2Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO3Switch
                        objectName: "moduleSlot1DO3Switch"
                        text:"ch3"
                        onCheckedChanged: moduleSlot1DO3SwitchChanged(moduleSlot1DO3Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO4Switch
                        objectName: "moduleSlot1DO4Switch"
                        text:"ch4"
                        onCheckedChanged: moduleSlot1DO4SwitchChanged(moduleSlot1DO4Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO5Switch
                        objectName: "moduleSlot1DO5Switch"
                        text:"ch5"
                        onCheckedChanged: moduleSlot1DO5SwitchChanged(moduleSlot1DO5Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO6Switch
                        objectName: "moduleSlot1DO6Switch"
                        text:"ch6"
                        onCheckedChanged: moduleSlot1DO6SwitchChanged(moduleSlot1DO6Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO7Switch
                        objectName: "moduleSlot1DO7Switch"
                        text:"ch7"
                        onCheckedChanged: moduleSlot1DO7SwitchChanged(moduleSlot1DO7Switch.checked)
                    }
                }
                ColumnLayout{
                    id: moduleSlot1_1
                    height:447
                    width:90
                    anchors.left: moduleSlot1_0.right
                    Switch{
                        id:moduleSlot1DO8Switch
                        objectName: "moduleSlot1DO8Switch"
                        text:"ch8"
                        onCheckedChanged: moduleSlot1DO8SwitchChanged(moduleSlot1DO8Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO9Switch
                        objectName: "moduleSlot1DO9Switch"
                        text:"ch9"
                        onCheckedChanged: moduleSlot1DO9SwitchChanged(moduleSlot1DO9Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO10Switch
                        objectName: "moduleSlot1DO10Switch"
                        text:"ch10"
                        onCheckedChanged: moduleSlot1DO10SwitchChanged(moduleSlot1DO10Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO11Switch
                        objectName: "moduleSlot1DO11Switch"
                        text:"ch11"
                        onCheckedChanged: moduleSlot1DO11SwitchChanged(moduleSlot1DO11Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO12Switch
                        objectName: "moduleSlot1DO12Switch"
                        text:"ch12"
                        onCheckedChanged: moduleSlot1DO12SwitchChanged(moduleSlot1DO12Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO13Switch
                        objectName: "moduleSlot1DO13Switch"
                        text:"ch13"
                        onCheckedChanged: moduleSlot1DO13SwitchChanged(moduleSlot1DO13Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO14Switch
                        objectName: "moduleSlot1DO14Switch"
                        text:"ch14"
                        onCheckedChanged: moduleSlot1DO14SwitchChanged(moduleSlot1DO14Switch.checked)
                    }
                    Switch{
                        id:moduleSlot1DO15Switch
                        objectName: "moduleSlot1DO15Switch"
                        text:"ch15"
                        onCheckedChanged: moduleSlot1DO15SwitchChanged(moduleSlot1DO15Switch.checked)
                    }
                }
            }
            GroupBox{
                id:inputSlot2
                height:475
                width:185
                anchors.left: moduleSlot1.right
                anchors.leftMargin: 10
                label:Label{
                    text:"DI2"
                }
                ColumnLayout{
                    id: inputSlot2_0
                    height:442
                    width:90
                    anchors.left: parent.left
                    anchors.leftMargin: -5

                    RowLayout{
                        id:inputSlot2DI0
                        StatusIndicator{
                            id:inputSlot2DI0Switch
                            objectName: "inputSlot2DI0Switch"
                            color: Material.primary
                        }
                        Label{
                            text:"ch0"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI1
                        StatusIndicator{
                            id:inputSlot2DI1Switch
                            objectName: "inputSlot2DI1Switch"
                            color: Material.primary
                        }
                        Label{
                            text:"ch1"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI2
                        StatusIndicator{
                            id:inputSlot2DI2Switch
                            objectName: "inputSlot2DI2Switch"
                        }
                        Label{
                            text:"ch2"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI3
                        StatusIndicator{
                            id:inputSlot2DI3Switch
                            objectName: "inputSlot2DI3Switch"
                        }
                        Label{
                            text:"ch3"
                        }
                    }

                    RowLayout{
                        id:inputSlot2DI4
                        StatusIndicator{
                            id:inputSlot2DI4Switch
                            objectName: "inputSlot2DI4Switch"
                        }
                        Label{
                            text:"ch4"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI5
                        StatusIndicator{
                            id:inputSlot2DI5Switch
                            objectName: "inputSlot2DI5Switch"
                        }
                        Label{
                            text:"ch5"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI6
                        StatusIndicator{
                            id:inputSlot2DI6Switch
                            objectName: "inputSlot2DI6Switch"
                        }
                        Label{
                            text:"ch6"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI7
                        StatusIndicator{
                            id:inputSlot2DI7Switch
                            objectName: "inputSlot2DI7Switch"
                        }
                        Label{
                            text:"ch7"
                        }
                    }

                }
                ColumnLayout{
                    id: inputSlot2_1
                    height:442
                    width:90
                    anchors.left: inputSlot2_0.right
                    RowLayout{
                        id:inputSlot2DI8
                        StatusIndicator{
                            id:inputSlot2DI8Switch
                            objectName: "inputSlot2DI8Switch"
                        }
                        Label{
                            text:"ch8"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI9
                        StatusIndicator{
                            id:inputSlot2DI9Switch
                            objectName: "inputSlot2DI9Switch"
                        }
                        Label{
                            text:"ch9"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI10
                        StatusIndicator{
                            id:inputSlot2DI10Switch
                            objectName: "inputSlot2DI10Switch"
                        }
                        Label{
                            text:"ch10"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI11
                        StatusIndicator{
                            id:inputSlot2DI11Switch
                            objectName: "inputSlot2DI11Switch"
                        }
                        Label{
                            text:"ch11"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI12
                        StatusIndicator{
                            id:inputSlot2DI12Switch
                            objectName: "inputSlot2DI12Switch"
                        }
                        Label{
                            text:"ch12"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI13
                        StatusIndicator{
                            id:inputSlot2DI13Switch
                            objectName: "inputSlot2DI13Switch"
                        }
                        Label{
                            text:"ch13"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI714
                        StatusIndicator{
                            id:inputSlot2DI14Switch
                            objectName: "inputSlot2DI14Switch"
                        }
                        Label{
                            text:"ch14"
                        }
                    }
                    RowLayout{
                        id:inputSlot2DI715
                        StatusIndicator{
                            id:inputSlot2DI15Switch
                            objectName: "inputSlot2DI15Switch"
                        }
                        Label{
                            text:"ch15"
                        }
                    }
                }
            }
            GroupBox{
                id:moduleSlot3
                height:475
                width:185
                anchors.left: inputSlot2.right
                anchors.leftMargin: 10
                label:Label{
                    text:"DO3"
                }
                ColumnLayout{
                    id: moduleSlot3_0
                    height:447
                    width:90
                    anchors.left: parent.left
                    anchors.leftMargin: -5
                    Switch{
                        id:moduleSlot3DO0Switch
                        objectName: "moduleSlot3DO0Switch"
                        text:"ch0"
                        onCheckedChanged: moduleSlot3DO0SwitchChanged(moduleSlot3DO0Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO1Switch
                        objectName: "moduleSlot3DO1Switch"
                        text:"ch1"
                        onCheckedChanged: moduleSlot3DO1SwitchChanged(moduleSlot3DO1Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO2Switch
                        objectName: "moduleSlot3DO2Switch"
                        text:"ch2"
                        onCheckedChanged: moduleSlot3DO2SwitchChanged(moduleSlot3DO2Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO3Switch
                        objectName: "moduleSlot3DO3Switch"
                        text:"ch3"
                        onCheckedChanged: moduleSlot3DO3SwitchChanged(moduleSlot3DO3Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO4Switch
                        objectName: "moduleSlot3DO4Switch"
                        text:"ch4"
                        onCheckedChanged: moduleSlot3DO4SwitchChanged(moduleSlot3DO4Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO5Switch
                        objectName: "moduleSlot3DO5Switch"
                        text:"ch5"
                        onCheckedChanged: moduleSlot3DO5SwitchChanged(moduleSlot3DO5Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO6Switch
                        objectName: "moduleSlot3DO6Switch"
                        text:"ch6"
                        onCheckedChanged: moduleSlot3DO6SwitchChanged(moduleSlot3DO6Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO7Switch
                        objectName: "moduleSlot3DO7Switch"
                        text:"ch7"
                        onCheckedChanged: moduleSlot3DO7SwitchChanged(moduleSlot3DO7Switch.checked)
                    }

                }
                ColumnLayout{
                    id: moduleSlot3_1
                    height:447
                    width:90
                    anchors.left: moduleSlot3_0.right

                    Switch{
                        id:moduleSlot3DO8Switch
                        objectName: "moduleSlot3DO8Switch"
                        text:"ch8"
                        onCheckedChanged: moduleSlot3DO8SwitchChanged(moduleSlot3DO8Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO9Switch
                        objectName: "moduleSlot3DO9Switch"
                        text:"ch9"
                        onCheckedChanged: moduleSlot3DO9SwitchChanged(moduleSlot3DO9Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO10Switch
                        objectName: "moduleSlot3DO10Switch"
                        text:"ch10"
                        onCheckedChanged: moduleSlot3DO10SwitchChanged(moduleSlot3DO10Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO11Switch
                        objectName: "moduleSlot3DO11Switch"
                        text:"ch11"
                        onCheckedChanged: moduleSlot3DO11SwitchChanged(moduleSlot3DO11Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO12Switch
                        objectName: "moduleSlot3DO12Switch"
                        text:"ch12"
                        onCheckedChanged: moduleSlot3DO12SwitchChanged(moduleSlot3DO12Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO13Switch
                        objectName: "moduleSlot3DO13Switch"
                        text:"ch13"
                        onCheckedChanged: moduleSlot3DO13SwitchChanged(moduleSlot3DO13Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO14Switch
                        objectName: "moduleSlot3DO14Switch"
                        text:"ch14"
                        onCheckedChanged: moduleSlot3DO14SwitchChanged(moduleSlot3DO14Switch.checked)
                    }
                    Switch{
                        id:moduleSlot3DO15Switch
                        objectName: "moduleSlot3DO15Switch"
                        text:"ch15"
                        onCheckedChanged: moduleSlot3DO15SwitchChanged(moduleSlot3DO15Switch.checked)
                    }
                }
            }
            GroupBox{
                id:moduleSlot4
                height:475
                width:185
                anchors.left:moduleSlot3.right
                anchors.leftMargin: 10
                label:Label{
                    text:"DO4"
                }
                ColumnLayout{
                    id: moduleSlot4_0
                    height:447
                    width:90
                    anchors.left: parent.left
                    anchors.leftMargin: -5

                    Switch{
                        id:moduleSlot4DO0Switch
                        objectName: "moduleSlot4DO0Switch"
                        text:"ch0"
                        onCheckedChanged: moduleSlot4DO0SwitchChanged(moduleSlot4DO0Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO1Switch
                        objectName: "moduleSlot4DO1Switch"
                        text:"ch1"
                        onCheckedChanged: moduleSlot4DO1SwitchChanged(moduleSlot4DO1Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO2Switch
                        objectName: "moduleSlot4DO2Switch"
                        text:"ch2"
                        onCheckedChanged: moduleSlot4DO2SwitchChanged(moduleSlot4DO2Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO3Switch
                        objectName: "moduleSlot4DO3Switch"
                        text:"ch3"
                        onCheckedChanged: moduleSlot4DO3SwitchChanged(moduleSlot4DO3Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO4Switch
                        objectName: "moduleSlot4DO4Switch"
                        text:"ch4"
                        onCheckedChanged: moduleSlot4DO4SwitchChanged(moduleSlot4DO4Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO5Switch
                        objectName: "moduleSlot4DO5Switch"
                        text:"ch5"
                        onCheckedChanged: moduleSlot4DO5SwitchChanged(moduleSlot4DO5Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO6Switch
                        objectName: "moduleSlot4DO6Switch"
                        text:"ch6"
                        onCheckedChanged: moduleSlot4DO6SwitchChanged(moduleSlot4DO6Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO7Switch
                        objectName: "moduleSlot4DO7Switch"
                        text:"ch7"
                        onCheckedChanged: moduleSlot4DO7SwitchChanged(moduleSlot4DO7Switch.checked)
                    }
                }
                ColumnLayout{
                    id: moduleSlot4_1
                    height:447
                    width:90
                    anchors.left: moduleSlot4_0.right
                    Switch{
                        id:moduleSlot4DO8Switch
                        objectName: "moduleSlot4DO8Switch"
                        text:"ch8"
                        onCheckedChanged: moduleSlot4DO8SwitchChanged(moduleSlot4DO8Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO9Switch
                        objectName: "moduleSlot4DO9Switch"
                        text:"ch9"
                        onCheckedChanged: moduleSlot4DO9SwitchChanged(moduleSlot4DO9Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO10Switch
                        objectName: "moduleSlot4DO10Switch"
                        text:"ch10"
                        onCheckedChanged: moduleSlot4DO10SwitchChanged(moduleSlot4DO10Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO11Switch
                        objectName: "moduleSlot4DO11Switch"
                        text:"ch11"
                        onCheckedChanged: moduleSlot4DO11SwitchChanged(moduleSlot4DO11Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO12Switch
                        objectName: "moduleSlot4DO12Switch"
                        text:"ch12"
                        onCheckedChanged: moduleSlot4DO12SwitchChanged(moduleSlot4DO12Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO13Switch
                        objectName: "moduleSlot4DO13Switch"
                        text:"ch13"
                        onCheckedChanged: moduleSlot4DO13SwitchChanged(moduleSlot4DO13Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO14Switch
                        objectName: "moduleSlot4DO14Switch"
                        text:"ch14"
                        onCheckedChanged: moduleSlot4DO14SwitchChanged(moduleSlot4DO14Switch.checked)
                    }
                    Switch{
                        id:moduleSlot4DO15Switch
                        objectName: "moduleSlot4DO15Switch"
                        text:"ch15"
                        onCheckedChanged: moduleSlot4DO15SwitchChanged(moduleSlot4DO15Switch.checked)
                    }
                }
            }

            GroupBox{
                id:afcCol
                width:360
                height:475
                anchors.left: moduleSlot4.right
                anchors.leftMargin: 10
                label:Label{
                    text:"AFC"
                }


                RowLayout{
                    id:deleteError
                    width:320
                    Label{
                        id:delErr
                        text:"Стереть сообщение об аварии"
                    }
                    Switch{
                        id:delErrSwitch
                        objectName: "delErrSwitch"
                        anchors.right: parent.right
                    }
                }
                RowLayout{
                    id:startAndStop
                    anchors.top:deleteError.bottom
                    width:320
                    Label{
                        id:startStop
                        text:"Старт/Стоп"
                    }
                    Switch{
                        id:afcStartStopSwitch
                        anchors.right: parent.right
                        objectName: "afcStartStopSwitch"
                    }
                }
                RowLayout{
                    id:installFREQ
                    anchors.top:startAndStop.bottom
                    width:320
                    Label{
                        id:installedFreequency
                        text:"Задание частоты"
                    }
                    TextField{
                        id:installedFreequencyInput
                        objectName: "installedFreequencyInput"
                        anchors.right: parent.right
                        validator: DoubleValidator
                        {
                            bottom: -32
                            top: 32
                            decimals:1
                        }

                        property real maximumValue: constants._MAXIMUM_INTEGER
                        property real minimumValue: constants._MINIMUM_INTEGER
                        property string previousText: ""

                        onTextChanged:
                        {
                            var numericValue = getValue()
                            if (numericValue > 32 || numericValue < -32)
                            {
                                text = previousText
                            }
                            previousText = text
                        }

                        function setValue(_value)
                        {
                            text = String(_value)
                        }

                        function getValue()
                        {
                            return Number(text)
                        }
                    }
                }
                RowLayout{
                    id:installU
                    anchors.top:installFREQ.bottom
                    width:320
                    Label{
                        id:installedU
                        text:"Заданние напряжения"
                    }
                    TextField{
                        id:installedUInput
                        objectName:"installedUInput"
                        anchors.right: parent.right
                        validator: DoubleValidator
                        {
                            bottom: 0
                            top: 250
                            decimals:1
                        }

                        property real maximumValue: constants._MAXIMUM_INTEGER
                        property real minimumValue: constants._MINIMUM_INTEGER
                        property string previousText: ""

                        onTextChanged:
                        {
                            var numericValue = getValue()
                            if (numericValue > 250 || numericValue < 0)
                            {
                                text = previousText
                            }
                            previousText = text
                        }

                        function setValue(_value)
                        {
                            text = String(_value)
                        }

                        function getValue()
                        {
                            return Number(text)
                        }
                    }
                }
                RowLayout{
                    id:driveOperat
                    anchors.top: installU.bottom
                    width:320
                    Label{
                        id:driveOperation
                        text:"Работа привода"
                    }

                    StatusIndicator{
                        id:driveOperationIndicator
                        objectName:"driveOperationIndicator"
                        color:Material.primary
                        anchors.right: parent.right
                    }
                }
                RowLayout{
                    id:driveBlocking
                    anchors.top:driveOperat.bottom
                    width:320
                    Label{
                        id:driveBlock
                        text:"Блокировка привода"
                    }

                    StatusIndicator{
                        id:driveBlockIndicator
                        objectName: "driveBlockIndicator"
                        color:Material.primary
                        anchors.right: parent.right
                    }
                }
                RowLayout{
                    id:errorEEPROM
                    anchors.top:driveBlocking.bottom
                    width:320
                    Label{
                        id:errEEPROM
                        text:"Ошибка CRC в EEPROM"
                    }
                    StatusIndicator{
                        id:errEEPROMIndicator
                        objectName: "errEEPROMIndicator"
                        color:Material.primary
                        anchors.right: parent.right
                    }
                }
                RowLayout{
                    id:crashingCode
                    anchors.top: errorEEPROM.bottom
                    width:320
                    Label{
                        id:crashCode
                        text:"Код аварии"
                    }
                    StatusIndicator{
                        id:crashCodeIndicator
                        objectName: "crashCodeIndicator"
                        color:Material.primary
                        anchors.right: parent.right
                    }
                }

                RowLayout{
                    id:crashingValue
                    anchors.top:crashingCode.bottom
                    anchors.topMargin: 5
                    width:320
                    Label{
                        id:crashValue
                        text:"Значение аварии"
                    }
                    Label{
                        id:crashValueLabel
                        objectName: "crashValueLabel"
                        text:"1"
                        anchors.right: parent.right
                    }
                }
                RowLayout{
                    id:inputFREQ
                    anchors.top:crashingValue.bottom
                    anchors.topMargin: 5
                    width:320
                    Label{
                        id:inputFreequency
                        text:"Входящая частота"
                    }
                    Label{
                        id:inputFreequencyLabel
                        objectName: "inputFreequencyLabel"
                        text:"1"
                        anchors.right: parent.right
                        //onTextChanged: Math.round(inputFreequencyLabel.text,-1)
                    }

                }
                RowLayout{
                    id:uInput
                    anchors.top:inputFREQ.bottom
                    anchors.topMargin: 5
                    width:320
                    Label{
                        id:inputU
                        text:"Входящее напряжение"
                    }
                    Label{
                        id:inputULabel
                        objectName: "inputULabel"
                        text:"1"
                        anchors.right: parent.right
                    }

                }
                Button{
                    id:afcButton
                    anchors.top:uInput.bottom
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:"Выполнить"
                    onClicked:pageLoader.source = "ChangeAFCValue.qml"
                }

            }

        }

    }
}









