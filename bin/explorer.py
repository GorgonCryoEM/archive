#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from libpytoolkit import *
import libpytoolkit as gtool
import gorg
from gorg.volume_viewer import VolumeViewer
import Explorer


class GLWidget(QtOpenGL.QGLWidget):

    def __init__(self):
        super(GLWidget, self).__init__()
        
        self.setMinimumSize(300, 300)
        self.setWindowTitle('Gorgon Explorer - alpha')
        
    def initializeGL(self):
        glEnable(GL_DEPTH_TEST)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glClearColor(1,0,0,1)
        
    def resizeGL(self, w, h):
        glViewport(0,0,w,h)
        

def main():
    parser = argparse.ArgumentParser(description='Gorgon Explorer')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    window = GLWidget()
    window.showNormal()
    window.show()
    window.raise_()
    window.resize(800, 800)
    window.move(300,50)
    
#     gorg.window.resize(800, 600)
# #     gorg.window.move(300,50)
#     gorg.window.show()
#     gorg.window.raise_()
#     gorg.window.volumeViewer.loadDataFromFile(args.volume)
#     gorg.window.skeletonViewer.loadDataFromFile(args.skeleton)
    
    vol = Explorer.VolumeViewer(app)
    vol.loadDataFromFile(args.volume)
    mainCamera = Explorer.Camera([vol], window)
    
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
