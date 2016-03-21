from PyQt4 import QtGui, QtCore, QtOpenGL
from shape import Shape
from libpytoolkit import Display
from libpytoolkit import RendererBase
from libpytoolkit import Vec3F
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Sphere(Shape):
    
    def __init__(self, main):
        super(Sphere, self).__init__(main)
        
        self.title = "Sphere"
        
        self.s = RendererBase()
#         self.color = QtGui.QColorDialog().getColor()
        self.color = QtGui.QColor(120, 18, 80, 150)
        
        self.loc = Vec3(30., 10., 10.)
        
        self.selectEnabled    = True
        self.mouseMoveEnabled = True
        
    def draw(self):
        self.setMaterials(self.color)
        self.s.drawSphere(self.loc, 10.)
