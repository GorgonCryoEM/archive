from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from libpytoolkit import RendererBase
from libpytoolkit import Vec3F
from .libs import Vec3

from base_dock_widget import BaseDockWidget
from ui_common import Ui_Common
from ui_dialog_volume_surface_editor import Ui_DialogVolumeSurfaceEditor
from ui_dialog_model_visualization import Ui_DialogModelVisualization

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


# class Sphere(BaseViewer, BaseDockWidget):
class Sphere(BaseViewer):
    
    def __init__(self, main, parent=None):
        super(Sphere, self).__init__(main, parent)
#         BaseDockWidget.__init__(self,
#                                 main,
#                                 self,
#                                 "&Sphere Editor",
#                                 QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea,
#                                 QtCore.Qt.LeftDockWidgetArea)
        self.title = "Sphere"
        
        self.renderer = Display()
        self.s = RendererBase()
#         self.color = QtGui.QColorDialog().getColor()
        self.color = QtGui.QColor(120, 18, 80, 150)
        
#         self.dock = QtGui.QDockWidget(self.title, main)
#         main.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.dock)
        
        self.ui = Ui_Common()
        self.ui.setupUi(self)
        
    def draw(self):
        self.setMaterials(self.color)
        self.s.drawSphere(Vec3(30., 10., 10.), 10.)
        
    def performElementSelection(self, hitStack):
        print "In: performElementSelection"
        
        color = QtGui.QColorDialog.getColor(self.color, self, '', QtGui.QColorDialog.ShowAlphaChannel)
        
        if color.isValid():
            self.color = color
