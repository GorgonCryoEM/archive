from PyQt4 import QtCore, QtGui
from camera import Camera
from volume_viewer import VolumeViewer
from sphere import Sphere
from line import Line

import sys, os


class MainWindowForm(QtGui.QMainWindow):

    def __init__(self, version):
        super(MainWindowForm, self).__init__()

#         self.volumeViewer = VolumeViewer(self)
#         self.skeletonViewer = SkeletonViewer(self)
        self.volumeViewer1 = VolumeViewer(self)
        self.sphere = Sphere(self)
        self.line  = Line(self)
        
#         self.scenes = [self.volumeViewer, self.skeletonViewer, self.volumeViewer1, self.sphere]
        self.scenes = [self.volumeViewer1, self.sphere, self.line]
#         self.scenes = [self.sphere]

        self.mainCamera = Camera(self.scenes, self)
        self.setCentralWidget(self.mainCamera)
        
        self.statusBar().showMessage(self.tr("Gorgon: Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon Explorer - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
            
#     def closeEvent(self, event):
#         exitText = "This will close Gorgon, you will lose all unsaved data.\nAre you sure?"
#
#         if (QtGui.QMessageBox.warning (self, self.tr("Exit Gorgon?"), self.tr(exitText), QtGui.QMessageBox.Yes | QtGui.QMessageBox.Cancel) == QtGui.QMessageBox.Yes):
#             event.accept()
#         else:
#             event.ignore()
