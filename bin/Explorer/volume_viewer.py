from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from .libs import Vec3

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class VolumeViewer(BaseViewer):
    
    objID=0

    def __init__(self, main):
        VolumeViewer.objID = VolumeViewer.objID + 1
        super(VolumeViewer, self).__init__(main)
#         BaseViewer.__init__(self, main)
        self.title = "Volume"
        self.shortTitle = "VOL"
        
        self.modelColor = QtGui.QColor(100+40*VolumeViewer.objID,200-40*VolumeViewer.objID,100,150)

        self.renderer = Display()
        
        self.renderer.enableDraw(True)
        
        self.connect(self, QtCore.SIGNAL("modelLoadedPreDraw()"), self.modelLoadedPreDraw)

    def draw(self):
        self.draw_base()

#     def selectionRotate(self, p, axis, angle):
# #         angle = 5.0
# #         axis = [1,1,0]
#         print "   In selectionRotate:", self
#         print "   params: ", angle, axis, axis[0], axis[1]
# #         glPushMatrix();
# #         glRotatef(angle, axis[0], axis[1], axis[2]);
# #         self.draw()
# #         glPopMatrix();
#
#     def selectionMove(self, v):
#         print "   In selectionMove:", self
#         loc = Vec3(self.renderer.getOriginX(),
#                    self.renderer.getOriginY(),
#                    self.renderer.getOriginZ())
#         loc.Print()
#
#         loc += v
#
#         loc.Print()
#
#         self.setLocationV(loc)
# #         self.draw()
# #         self.app.mainCamera.updateGL()

    def modelLoadedPreDraw(self):
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()

        defaultDensity = (minDensity + maxDensity) / 2
        maxRadius = int(max(self.renderer.getMax(0)/2, self.renderer.getMax(1)/2, self.renderer.getMax(2)/2));
        
        self.renderer.setSampleInterval(1)
        self.renderer.setSurfaceValue(defaultDensity)
        self.renderer.setDisplayRadius(maxRadius)
        
#     def draw(self):
#         self.setMaterials(QtGui.QColor(40,40,40,150))
#         super(VolumeViewer, self).draw()
