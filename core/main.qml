import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
  id: container
  width: 800
  height: 800

  Button {
    id: cmdQuit
    text: qsTr("Quit")
    width: 64
    height: 32
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 20
        color: "blue"
        text: control.text
      }
    }
  }
}
