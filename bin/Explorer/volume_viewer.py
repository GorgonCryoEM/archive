from PyQt4 import QtGui, QtCore, QtOpenGL
from shape import Shape


class VolumeViewer(Shape):

    def __init__(self, main):
        super(VolumeViewer, self).__init__(main)
        
        self.title = "Volume"
        self.shortTitle = "VOL"

        self.loaded = False
        
    def selectionMove(self, v):
        print "     In: selectionMove", self
        self.move(v)
        
    def getCOM(self):
        return self.renderer.getCenterOfMass()
    
    def preDraw(self):
        self.renderer.enableDraw(False)
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2
        
        maxRadius = int(max(self.renderer.getMax(0)/2, self.renderer.getMax(1)/2, self.renderer.getMax(2)/2));
        self.renderer.setSampleInterval(1)
        self.renderer.setSurfaceValue(defaultDensity)
        self.renderer.setDisplayRadius(maxRadius)
        self.renderer.enableDraw(True)
