from shape import Shape
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Primitive(Shape):

    def __init__(self, main):
        super(Primitive, self).__init__(main)
        
        self.setLoc(0,0,0)
        self.glmode = GL_POLYGON
        
    def draw(self):
        self.setMaterials(self.color)

        ax = self.axis
        
        glPushMatrix()
        glTranslate(self.loc[0],self.loc[1],self.loc[2])
        glRotatef(self.angle, ax[0], ax[1], ax[2])

        glBegin(self.glmode)

        for p in self.points:
            glVertex(p[0], p[1], p[2])
            p.Print()

        glEnd()
        
        glPopMatrix()


class Triangle(Primitive):

    def __init__(self, main, p1, p2, p3):
        super(Triangle, self).__init__(main)
        
        self.points = [Vec3(p1), Vec3(p2), Vec3(p3)]
        
        self.setColor(50, 50, 200, 150)
        
#         loc = self.setCOM()
#         self.setLoc(loc[0], loc[1], loc[2])
