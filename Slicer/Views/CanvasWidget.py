from PySide6 import QtWidgets, QtCore
from PySide6.QtCore import Qt, Slot, QStandardPaths

from PySide6.QtGui import (
    QMouseEvent,
    QPaintEvent,
    QPen,
    QAction,
    QPainter,
    QColor,
    QPixmap,
    QImage,
    QIcon,
    QKeySequence
)

class CanvasWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()


        self.setFixedSize(680, 480)
        self.pixmap = QPixmap(self.size())

        self.pixmap.fill(Qt.GlobalColor.white)

        self.painter = QPainter()

        self.image = QImage()

        # self.layout.addWidget(self.text)
        # self.layout.addWidget(self.button)

    def load_image(self, path):
        self.image.load(path)

