#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T16:22:54
#
#-------------------------------------------------

QT       -= core gui

TARGET = qak
TEMPLATE = lib

DEFINES += QAK_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../../libqak/atomic.cxx \
    ../../../../libqak/host_info.cxx \
    ../../../../libqak/mutex.cxx \
    ../../../../libqak/now.cxx \
    ../../../../libqak/rptr.cxx \
    ../../../../libqak/static_data.cxx \
    ../../../../libqak/stopwatch.cxx \
    ../../../../libqak/thread.cxx \
    ../../../../libqak/ucs.cxx
# Also:
#libqak/permutation.cxx
#libqak/rotate_sequence.cxx
#libqak/threadls.cxx superceded by thread_local
#libqak/thread_group.cxx

INCLUDEPATH += $$PWD/../../../include
HEADERS += \
    ../../../../include/qak/host_info.hxx \
    ../../../../include/qak/abs.hxx \
    ../../../../include/qak/alignof.hxx \
    ../../../../include/qak/atomic.hxx \
    ../../../../include/qak/bitsizeof.hxx \
    ../../../../include/qak/config.hxx \
    ../../../../include/qak/fail.hxx \
    ../../../../include/qak/hash.hxx \
    ../../../../include/qak/host_info.hxx \
    ../../../../include/qak/io.hxx \
    ../../../../include/qak/is_memcpyable.hxx \
    ../../../../include/qak/macros.hxx \
    ../../../../include/qak/min_max.hxx \
    ../../../../include/qak/mutex.hxx \
    ../../../../include/qak/now.hxx \
    ../../../../include/qak/optional.hxx \
    ../../../../include/qak/permutation.hxx \
    ../../../../include/qak/prng64.hxx \
    ../../../../include/qak/rotate_sequence_vector.hxx \
    ../../../../include/qak/rotate_sequence.hxx \
    ../../../../include/qak/rptr.hxx \
    ../../../../include/qak/static_data.hxx \
    ../../../../include/qak/stopwatch.hxx \
    ../../../../include/qak/test_app_post.hxx \
    ../../../../include/qak/test_app_pre.hxx \
    ../../../../include/qak/test_macros.hxx \
    ../../../../include/qak/thread_group.hxx \
    ../../../../include/qak/thread.hxx \
    ../../../../include/qak/threadls.hxx \
    ../../../../include/qak/ucs.hxx \
    ../../../../include/qak/vector.hxx \
    ../../../../include/qak/zz_imp_pthread.hxx \
    ../../../../include/qak/imp/pthread.hxx \
    ../../../../include/qak/io/io.hxx \
    ../../../../include/qak/workarounds/alignof_operator.hxx

unix {
    target.path = /usr/lib
    INSTALLS += target
}

*-g++* {
    QMAKE_CXXFLAGS += -std=c++17
}

INCLUDEPATH += $$PWD/../../../../include
