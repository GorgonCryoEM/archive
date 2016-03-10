from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class VolumeViewer(BaseViewer):

    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Volume"
        self.shortTitle = "VOL"

        self.renderer = Display()
        
        self.renderer.enableDraw(True)
        
        self.connect(self, QtCore.SIGNAL("modelLoadedPreDraw()"), self.modelLoadedPreDraw)

    def modelLoadedPreDraw(self):
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()

        defaultDensity = (minDensity + maxDensity) / 2
        maxRadius = int(max(self.renderer.getMax(0)/2, self.renderer.getMax(1)/2, self.renderer.getMax(2)/2));
        
        self.renderer.setSampleInterval(1)
        self.renderer.setSurfaceValue(defaultDensity)
        self.renderer.setDisplayRadius(maxRadius)
