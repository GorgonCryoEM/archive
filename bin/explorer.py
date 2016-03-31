#!/usr/bin/env python

from PyQt4 import QtGui

import sys
import argparse

from Explorer import MainWindowForm


def main():
    parser = argparse.ArgumentParser(description='Gorgon Explorer')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)
    
    window = MainWindowForm('2.2.3')
    
    window.resize(800, 600)
    window.show()
    window.volumeViewer1.load("vp6-96o.mrc")
    window.raise_()
    
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
