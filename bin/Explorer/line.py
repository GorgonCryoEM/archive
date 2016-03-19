from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from libpytoolkit import RendererBase
from libpytoolkit import Vec3F
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Line(BaseViewer):
    
    def __init__(self, main, p1, parent=None):
        super(Line, self).__init__(main, parent)
        
        self.title = "Line"
        
        self.renderer = Display()
        self.s = RendererBase()
#         self.color = QtGui.QColorDialog().getColor()
        self.color = QtGui.QColor(40, 70, 100, 150)
        
        self.main = main
        
        self.p1 = p1
#         self.p1 = Vec3(30., 10., 10.)
        self.p2 = Vec3( 0.,  0.,  0.)
        
        self.loc = self.getCOM()
        
        self.selectEnabled    = True
        self.mouseMoveEnabled = True
        self.depthEnabled = False
        
    def draw(self):
        self.setMaterials(self.color)
        glLineWidth(5.)
#         if not self.depthEnabled:
#             glDisable(GL_DEPTH_TEST)
#             glClear(GL_DEPTH_BUFFER_BIT)
        self.s.drawLine(self.p1, self.p2)
#         glEnable(GL_DEPTH_TEST)

    def selectionMove(self, v):
        print "     In: selectionMove", self
        self.p1 += v
        self.p2 += v
        self.draw()
        
    def getCOM(self):
        return (self.p1+self.p2)/2.

    def redraw(self, p1, p2):
        self.p1 = p1
        self.p2 = p2

#     def redraw(self, p):
#         self.p1 = self.p2
#         self.p2 = p
#         self.draw()
