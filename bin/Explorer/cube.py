from PyQt4 import QtGui, QtCore, QtOpenGL
from shape import Shape

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from ui_common import Ui_Common


class Cube(Shape):
    
    def __init__(self, main):
        super(Cube, self).__init__(main)
        
        self.title = "Cube"
        
        self.setColor(80, 18, 130, 150)
        
#         self.setLoc(-30., 10., 10.)
        self.setLoc(0, 0, 0)
        
        self.L = 10
        
        self.ui = Ui_Common()
        
        self.dock = QtGui.QDockWidget("Common: Cube", main)
        self.ui.setupUi(self.dock)
        main.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)

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
