from shape import Shape
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Primitive(Shape):

    def __init__(self, main, n):
        super(Primitive, self).__init__(main)
        
        self.points = [Vec3(0,0,0) for i in range(n)]
        
    def draw(self):
        self.setMaterials(self.color)
#         super(Primitive, self).draw()
        
        glBegin(GL_LINES)
        
        for p in self.points:
            glVertex(p[0], p[1], p[2])
        
        glEnd()
        

class Triangle(Primitive):

    def __init__(self, main, p1, p2, p3):
        super(Triangle, self).__init__(main, 3)
        
        self.points = [p1, p2, p3]
        self.setColor(50, 50, 50, 150)
