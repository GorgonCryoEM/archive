from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from libpytoolkit import RendererBase
from .libs import Vec3
from ui_common import Ui_Common

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Shape(BaseViewer):

    def __init__(self, main):
        super(Shape, self).__init__(main)
        
        self.renderer = Display()
        self.s = RendererBase()
        
#         self.dock = Ui_Common()
#         self.dock.setupUi(main)
#         self.dock.setVisible(True)
#         self.show()

    def setColor(self, r, g, b, a):
        self.color = QtGui.QColor(r, g, b, a)

    def setLoc(self, x, y, z):
        self.loc = Vec3(x, y, z)

    def setLocationV(self, v):
        self.setLocation(v[0], v[1], v[2])

    def setLocation(self, x, y, z):
        self.renderer.setOrigin(x, y, z)
        self.app.mainCamera.updateGL()

    def move(self, v):
        loc = Vec3(self.renderer.getOriginX(),
                   self.renderer.getOriginY(),
                   self.renderer.getOriginZ())
        loc += v
        self.setLocationV(loc)

    def selectionMove(self, v):
        print "     In: selectionMove", self
        self.loc += v

    def getCOM(self):
        return self.loc

    def selectionRotate(self, com, axis, angle):
        print "  In selectionRotate: ", self
        print angle
        axis.Print()
        
        self.angle = self.angle + angle
        self.axis  = axis
        
