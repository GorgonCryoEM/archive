from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from libpytoolkit import RendererBase
from libpytoolkit import Vec3F
from .libs import Vec3

# from volume_surface_editor_form import VolumeSurfaceEditorForm
# from model_visualization_form import ModelVisualizationForm
# from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Sphere(BaseViewer):
    
    def __init__(self, parent):
        super(Sphere, self).__init__(parent)
#         BaseViewer.__init__(self, parent)
        self.title = "Sphere"
        
        self.renderer = Display()
        self.s = RendererBase()
#         self.color = QtGui.QColorDialog().getColor()
        self.color = QtGui.QColor(120, 18, 80, 150)
        
        self.loc = Vec3(30., 10., 10.)
        
        self.selectEnabled    = True
        self.mouseMoveEnabled = True
        
    def draw(self):
        self.setMaterials(self.color)
        self.s.drawSphere(self.loc, 10.)
        
    def mousePressEvent(self, e):
        print "Sphere loc:"
        self.loc.Print()
        
        self.app.mainCamera.updateGL()
        
    def performElementSelection(self, hitStack):
#         print self.app.mainCamera.scene
        
#         self.app.mainCamera.scene = self.app.mainCamera.scene[-2:]
        self.app.mainCamera.selectedScene = 3
        
#         self.setLocation(0, 0, 0)
#         self.loc = Vec3(0, 0, 0)
        
        
#         print self.app.mainCamera.scene
        
#         color = QtGui.QColorDialog.getColor(self.color, self, '', QtGui.QColorDialog.ShowAlphaChannel)
#
#         if color.isValid():
#             self.color = color
