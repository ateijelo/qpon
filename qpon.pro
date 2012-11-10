# This file is part of QPon, a simple interface Qt interface
# for the pon/poff ppp utilities.
#
# Copyright (C) 2007 Andy Teijelo <ateijelo@uh.cu>
# 
# QPon is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2, as published by the Free Software Foundation;
# 
# QPon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# The file 'COPYING' contains the text of the GNU General Public License.
#
# #####################################################################
# Automatically generated by qmake (2.01a) Fri Mar 30 20:38:36 2007
# #####################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# QT += phonon
CONFIG += debug

# Input
HEADERS += connectdialog.h \
    ppp.h
FORMS += connectdialog.ui
SOURCES += main.cpp \
    connectdialog.cpp \
    ppp.cpp
TRANSLATIONS = qpon_es.ts
