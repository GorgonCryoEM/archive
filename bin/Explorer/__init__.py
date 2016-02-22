import sys, os, inspect

from PyQt4 import QtGui, QtCore
from main_window_form import MainWindowForm

app = QtGui.QApplication(sys.argv)    
app.processEvents()

window = MainWindowForm(__version__)
