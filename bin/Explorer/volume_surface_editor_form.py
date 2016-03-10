from PyQt4 import QtCore, QtGui


class VolumeSurfaceEditorForm(QtGui.QWidget):
    
    def __init__(self, main, volumeViewer, parent=None):
        QtGui.QWidget.__init__(self, parent)

        self.viewer = volumeViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoadedPreDraw()"), self.modelLoadedPreDraw)
     
    def modelLoadedPreDraw(self):
        self.viewer.renderer.enableDraw(False)
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2
        
        maxRadius = int(max(self.viewer.renderer.getMax(0)/2, self.viewer.renderer.getMax(1)/2, self.viewer.renderer.getMax(2)/2));
        self.viewer.renderer.setSampleInterval(1)
        self.viewer.renderer.setSurfaceValue(defaultDensity)
        self.viewer.renderer.setDisplayRadius(maxRadius)
        self.viewer.renderer.enableDraw(True)
