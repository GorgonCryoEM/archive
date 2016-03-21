from PyQt4 import QtGui, QtCore, QtOpenGL
from shape import Shape
from libpytoolkit import Display
from libpytoolkit import RendererBase
from libpytoolkit import Vec3F
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Dot(Shape):
    
    def __init__(self, main):
        super(Dot, self).__init__(main)
        
        self.title = "Dot"
        
        self.s = RendererBase()
#         self.color = QtGui.QColorDialog().getColor()
        self.color = QtGui.QColor(120, 0, 0, 150)
        
        self.loc = Vec3(0., 0., 0.)
        
    def draw(self):
        self.setMaterials(self.color)
        self.s.drawSphere(self.loc, 1.)
