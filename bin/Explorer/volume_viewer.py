from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from volume_surface_editor_form import VolumeSurfaceEditorForm
from model_visualization_form import ModelVisualizationForm
from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class VolumeViewer(BaseViewer):

    def __init__(self, main, parent=None):
        super(VolumeViewer, self).__init__(main, parent)
        self.title = "Volume"
        self.shortTitle = "VOL"
        
        self.color = QtGui.QColor(50, 200, 50, 150)

        self.renderer = Display()
        self.loaded = False
