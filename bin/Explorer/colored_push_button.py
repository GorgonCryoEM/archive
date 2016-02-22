from PyQt4 import QtCore, QtGui


class ColoredPushButton(QtGui.QPushButton):
    
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.actualColor = QtGui.QColor.fromRgba(QtGui.qRgba(128, 128, 128, 255))
        
        self.connect(self, QtCore.SIGNAL("pressed ()"), self.buttonPressed)
        self.colorPicker = QtGui.QColorDialog(self)
        
    def paintEvent(self, event):
        QtGui.QPushButton.paintEvent(self, event)
        painter = QtGui.QPainter()
        painter.begin(self)
        painter.fillRect(5, 4, self.width()-10, self.height()-8, self.actualColor)
        painter.end()
    
    def setColor(self, color):
        self.actualColor = color;
        self.update()
    
    def buttonPressed(self):
        self.colorPicker.setCurrentColor(self.actualColor)
        self.setColor(self.colorPicker.getColor())
        self.emitColorChanged()
    
    def emitColorChanged(self):
        self.emit(QtCore.SIGNAL("colorChanged()"))

    
