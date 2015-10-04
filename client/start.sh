#!/bin/sh

export QT_QPA_EGLFS_PHYSICAL_WIDTH=271
export QT_QPA_EGLFS_PHYSICAL_HEIGHT=163
export QT_QPA_EGLFS_DEPTH=32
export QT_STYLE_OVERRIDE=GTK

./armv7l/rotor -platform eglfs 
