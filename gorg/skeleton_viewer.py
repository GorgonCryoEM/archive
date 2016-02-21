from PyQt4 import QtGui, QtCore, QtOpenGL
from libpytoolkit import MeshRenderer
from base_viewer import BaseViewer
from model_visualization_form import ModelVisualizationForm


from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class SkeletonViewer(BaseViewer):

    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Skeleton"
        self.shortTitle = "SKE"
        
        self.menu = self.app.menubar.addMenu('Skeleton')
        
        self.isClosedMesh = False
        self.twoWayLighting = True
        self.lineThickness = 3
        self.renderer = MeshRenderer()
        self.renderer.setLineThickness(self.lineThickness)
        self.createUI()
        self.app.viewers["skeleton"] = self;
        self.volumeViewer = self.app.viewers["volume"]
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
        self.visualizationOptions.ui.spinBoxThickness.setValue(self.lineThickness)
        self.visualizationOptions.ui.spinBoxThickness.setVisible(True)
        self.visualizationOptions.ui.labelThickness.setVisible(True)

        self.model2Visible = True
        self.model3Visible = True
        self.visualizationOptions.ui.checkBoxModelVisible.setText("Show curves colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setText("Show surfaces colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel2Color.setVisible(True)
        self.visualizationOptions.ui.checkBoxModel3Visible.setText("Show surface borders:")
        self.visualizationOptions.ui.checkBoxModel3Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel3Color.setVisible(True)
    
    def createUI(self):
        self.createActions()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        pass
#         self.optionsWindow = SkeletonLaplacianSmoothingForm(self.app, self, self)
        
    def createActions(self):
        self.openAct = QtGui.QAction(self.tr("S&keleton..."), self)
        self.openAct.setShortcut(self.tr("Ctrl+K"))
        self.openAct.setStatusTip(self.tr("Load a skeleton file"))
        self.openAct.triggered.connect(self.loadData)
        
        self.saveAct = QtGui.QAction(self.tr("S&keleton..."), self)
        self.saveAct.setStatusTip(self.tr("Save a skeleton file"))
        self.saveAct.triggered.connect(self.saveData)
        
        self.closeAct = QtGui.QAction(self.tr("S&keleton"), self)
        self.closeAct.setStatusTip(self.tr("Close the loaded skeleton"))
        self.closeAct.triggered.connect(self.unloadData)

        self.menu.addAction(self.openAct)
        self.menu.addAction(self.saveAct)
        self.menu.addAction(self.closeAct)

    def updateActionsAndMenus(self):
        pass
#         if(self.viewerAutonomous):
#             self.actions.getAction("load_Skeleton").setEnabled(True)
#             self.actions.getAction("save_Skeleton").setEnabled(self.loaded)
#             self.actions.getAction("unload_Skeleton").setEnabled(self.loaded)
#             self.menus.getMenu("actions-skeleton").setEnabled(True)
        
    def updateViewerAutonomy(self, autonomous):
        if(not autonomous):
            self.actions.getAction("load_Skeleton").setEnabled(autonomous)
            self.actions.getAction("save_Skeleton").setEnabled(autonomous)
            self.actions.getAction("unload_Skeleton").setEnabled(autonomous)
            self.menus.getMenu("actions-skeleton").setEnabled(autonomous)
        else:
            self.updateActionsAndMenus()
                   
    def loadVolume(self, volume):
        if(self.loaded):
            self.unloadData
        self.renderer.loadVolume(volume)
        self.loaded = True
        self.emitModelLoaded()
        self.emitViewerSetCenter()
