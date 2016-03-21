from shape import Shape
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Line(Shape):
    
    def __init__(self, main, p1):
        super(Line, self).__init__(main)
        
        self.title = "Line"
        
        self.setColor(40, 70, 100, 150)
        
        self.p1 = p1
        self.p2 = Vec3( 0.,  0.,  0.)
        
        self.loc = self.getCOM()
        
    def draw(self):
        self.setMaterials(self.color)
        glLineWidth(5.)
        self.s.drawLine(self.p1, self.p2)

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
