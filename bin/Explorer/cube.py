from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from libpytoolkit import RendererBase
from libpytoolkit import Vec3F
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Cube(BaseViewer):
    
    def __init__(self, main):
        super(Cube, self).__init__(main)
        
        self.title = "Cube"
        
        self.renderer = Display()
        self.s = RendererBase()
#         self.color = QtGui.QColorDialog().getColor()
        self.color = QtGui.QColor(80, 18, 130, 150)
        
        self.main = main
        
        self.loc = Vec3(-30., 10., 10.)
        
        self.L = self.width()/4
        self.L = 10
        
        self.selectEnabled    = True
        self.mouseMoveEnabled = True
        
    def draw(self):
        self.setMaterials(self.color)
        glLineWidth(5.)
        
        ax = self.axis
        
        glPushMatrix()
        glTranslate(self.loc[0],self.loc[1],self.loc[2])
        glRotatef(self.angle, ax[0], ax[1], ax[2])
        
        L = self.L
        glBegin(GL_QUADS)
#         glVertex(self.loc[0]+0,self.loc[1]+L,self.loc[2]+L)
#         glVertex(self.loc[0]+0,self.loc[1]+L,self.loc[2]-L)
#         glVertex(self.loc[0]+0,self.loc[1]-L,self.loc[2]-L)
#         glVertex(self.loc[0]+0,self.loc[1]-L,self.loc[2]+L)
        
        glColor(.4, .4, .6, 150)
        glVertex(-L,+L,+L)
        glVertex(-L,+L,-L)
        glVertex(-L,-L,-L)
        glVertex(-L,-L,+L)
        
        glVertex(L,+L,+L)
        glVertex(L,+L,-L)
        glVertex(L,-L,-L)
        glVertex(L,-L,+L)

        glEnd()
        
        glBegin(GL_LINES)
        
        glColor(.8, .8, .6, 150)
        glVertex(-L,+L,+L)
        glVertex(L,+L,+L)
        
        glVertex(-L,+L,-L)
        glVertex(L,+L,-L)
        
        glVertex(-L,-L,-L)
        glVertex(L,-L,-L)
        
        glVertex(-L,-L,+L)
        glVertex(L,-L,+L)

        glEnd()
        
        glPopMatrix()
