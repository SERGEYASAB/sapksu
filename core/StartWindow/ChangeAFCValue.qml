import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
import StartDiagnosticWindow_copy 1.0
import LoginDialog_copy 1.0

ApplicationWindow {
    id:changeAFCValue
    width:300
    height: 200
    visible: true
    flags: Qt.FramelessWindowHint
    Material.theme:model.getTheme()
    title: "Подтверждение введённых данных"

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);
    }

    Label{
        id: changeAFCLabel
        width:200
        wrapMode: Text.WordWrap
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 50
        text:qsTr("Вы действительно хотите изменить параметры AFC?")
    }

    Button{
        id: changeAFCYes
        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.bottom:parent.bottom
        anchors.bottomMargin: 20
        text:"Да"
        onClicked:{
            afcDo()
             pageLoader.source = "";
            changeAFCValue.close()
        }
    }
    Button{
        id:changeAFCNo
        anchors.right:parent.right
        anchors.rightMargin: 40
        anchors.bottom:parent.bottom
        anchors.bottomMargin: 20
        text:"Нет"
        onClicked: {
             pageLoader.source = ""
            changeAFCValue.close()
        }
    }
}
