#-------------------------------------------------
#
# Project created by QtCreator 2014-02-23T12:36:39
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = l2informer_H5
TEMPLATE = app


SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/models/l2charclass.cpp \
    src/db/charclassdb.cpp \
    src/tab_classes.cpp \
    src/models/skilltreeentry.cpp \
    src/db/skilltree.cpp \
    src/models/l2skill.cpp \
    src/models/l2skilltable.cpp \
    src/models/l2skillset.cpp \
    src/db/skillgrp_loader.cpp \
    src/db/skillsdb.cpp \
    src/ui_utils.cpp \
    src/tab_search.cpp \
    src/db/utils_db.cpp \
    src/models/l2skilleffect.cpp \
    src/models/l2skillstatmodifier.cpp \
    src/skillinfownd.cpp \
    src/skillinfopane.cpp

HEADERS  += \
    src/mainwindow.h \
    src/models/l2charclass.h \
    src/db/charclassdb.h \
    src/tab_classes.h \
    src/models/skilltreeentry.h \
    src/db/skilltree.h \
    src/models/l2skill.h \
    src/models/l2skilltable.h \
    src/models/l2skillset.h \
    src/db/skillgrp_loader.h \
    src/db/skillsdb.h \
    src/ui_utils.h \
    src/tab_search.h \
    src/db/utils_db.h \
    src/models/l2skilleffect.h \
    src/models/l2skillstatmodifier.h \
    src/skillinfownd.h \
    src/skillinfopane.h

FORMS    += \
    src/mainwindow.ui \
    src/tab_classes.ui \
    src/tab_search.ui \
    src/skillinfownd.ui \
    src/skillinfopane.ui
