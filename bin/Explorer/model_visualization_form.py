from PyQt4 import QtCore, QtGui
from ui_dialog_model_visualization import Ui_DialogModelVisualization
from base_dock_widget import BaseDockWidget


class ModelVisualizationForm(BaseDockWidget):

    def __init__(self, main, viewer):
        self.app = main
        self.viewer = viewer
        self.title = viewer.title + " Visualization Options"
        BaseDockWidget.__init__(self,
                                main,
                                self.title,
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea,
                                QtCore.Qt.RightDockWidgetArea)

        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.setWindowTitle(self.title)
        
        self.createUI()

    def createUI(self):
        self.ui = Ui_DialogModelVisualization()
        self.ui.setupUi(self)
        self.connect(self.ui.radioButtonWireframe, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonFlat, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonSmooth, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.checkBoxBoundingBox, QtCore.SIGNAL("toggled (bool)"), self.viewer.setBoundingBox)
        self.connect(self.ui.checkBoxModelVisible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModelVisibility)
        self.connect(self.ui.pushButtonModelColor, QtCore.SIGNAL("colorChanged ()"), self.setModelColor)
        self.connect(self.ui.pushButtonCenter, QtCore.SIGNAL("pressed ()"), self.viewer.emitViewerSetCenterLocal)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.viewer.unload)
        self.connect(self.ui.doubleSpinBoxSizeX, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeY, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeZ, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxLocationX, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationY, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationZ, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
                                                 
    def modelLoaded(self):
        self.showWidget()
    
    def setModelColor(self):
        self.viewer.setModelColor(self.ui.pushButtonModelColor.color())
    
    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonWireframe.isChecked()):
            displayStyle = self.viewer.DisplayStyleWireframe
        elif(self.ui.radioButtonFlat.isChecked()):
            displayStyle = self.viewer.DisplayStyleFlat
        elif(self.ui.radioButtonSmooth.isChecked()):
            displayStyle = self.viewer.DisplayStyleSmooth
        self.viewer.setDisplayStyle(displayStyle)
                                                  
    def scaleChanged(self):
        self.viewer.setScale(1, 1, 1)
    
    def locationChanged(self):
        self.viewer.setLocation(0, 0, 0)
