import sys
import random
from PySide6 import QtCore, QtWidgets, QtGui
from PySide6.QtCore import Qt, Slot, QStandardPaths

from PySide6.QtWidgets import (
    QWidget,
    QMainWindow,
    QApplication,
    QFileDialog,
    QStyle,
    QColorDialog,
)

from Views.CanvasWidget import CanvasWidget
from Views.UserInterfaceWidget import UserInterfaceWidget


class MainWindow(QMainWindow):

    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)

        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # Create layout for the central widget
        self.layout = QtWidgets.QHBoxLayout(central_widget)

        self.canvas_widget = CanvasWidget()
        self.ui = UserInterfaceWidget()

        self.layout.addWidget(self.ui)
        self.layout.addWidget(self.canvas_widget)


        


if __name__ == "__main__":

    app = QApplication(sys.argv)

    w = MainWindow()
    w.show()
    sys.exit(app.exec())