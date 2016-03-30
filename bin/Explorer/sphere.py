from PyQt4 import QtGui, QtCore, QtOpenGL
from shape import Shape
from ui_common import Ui_Common


class Sphere(Shape):
    
    def __init__(self, main, R=1.0):
        super(Sphere, self).__init__(main)
        
        self.title = "Sphere"
        self.R = R
        
        self.setColor(120, 18, 80, 150)
        
        self.setLoc(30., 10., 10.)
        
#         self.ui = Ui_Common()
#         self.ui.setupUi(self)
#
#         self.dock = QtGui.QDockWidget("Common: Sphere", self)
#         main.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
        
    def draw(self):
        self.setMaterials(self.color)
        self.s.drawSphere(self.loc, self.R)
