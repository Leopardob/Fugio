#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T13:14:29
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-ffmpeg)
TEMPLATE = lib
CONFIG += plugin c++11

linux {
	QMAKE_CFLAGS += -std=c11
}

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	mediaaudioprocessor.cpp \
	audiobuffer.cpp \
	medianode.cpp \
	imageconvertnode.cpp \
	ffmpegplugin.cpp \
	mediasegment.cpp \
	mediarecorderform.cpp \
	mediarecordernode.cpp \
	mediapreset/mediapresetmanager.cpp \
	mediaprocessornode.cpp \
    hap/source/hap.c

HEADERS += \
	../../include/fugio/ffmpeg/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	mediaaudioprocessor.h \
	audiobuffer.h \
	medianode.h \
	imageconvertnode.h \
	ffmpegplugin.h \
	mediasegment.h \
	segmentinterface.h \
	mediarecorderform.h \
	mediarecordernode.h \
	mediapreset/mediadvdpal.h \
	mediapreset/mediapresetmanager.h \
	mediapreset/mediapresetmp4.h \
	mediapreset/mediaProxy.h \
	mediapreset/mediaWav44.h \
	mediapreset/mediaYouTube240p.h \
	mediapreset/mediaYouTube240pWide.h \
	mediapreset/mediaYouTube360pWide.h \
	mediapreset/mediaYouTube480pWide.h \
	mediapreset/mediaYouTube720pWide.h \
	mediapreset/mediaYouTube1080pWide.h \
	mediaprocessornode.h \
	processoraudiobuffer.h \
	mediapreset/mediapresetinterface.h \
	mediapreset/media360_2048.h \
    hap/source/hap.h

FORMS += \
	mediarecorderform.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_ffmpeg_de.ts \
	translations/fugio_ffmpeg_es.ts \
	translations/fugio_ffmpeg_fr.ts

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
	DEFINES += TARGET_OS_MAC
	CONFIG -= x86
	CONFIG += lib_bundle

	BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
	INSTALLDEST  = $$INSTALLDATA/plugins
	INCLUDEDEST  = $$INSTALLDATA/include/fugio
	FRAMEWORKDIR = $$BUNDLEDIR/Contents/Frameworks

	DESTDIR = $$BUNDLEDIR/Contents/MacOS
	DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		isEmpty( CASKBASE ) {
			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$FRAMEWORKDIR
			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

#------------------------------------------------------------------------------
# Windows

windows {
	INSTALLDEST  = $$INSTALLDATA/plugins/ffmpeg

	plugin.path  = $$INSTALLDEST
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin

	win32 {
		FFMPEGDIR = $$(LIBS)/ffmpeg-3.2-win32
	}

	win64 {
		FFMPEGDIR = $$(LIBS)/ffmpeg-3.2-win64
	}

	libraries.path  = $$INSTALLDEST
	libraries.files = $$FFMPEGDIR/bin/*.dll

	INSTALLS += libraries

	exists( $$FFMPEGDIR ) {
		DEFINES += FFMPEG_SUPPORTED
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

exists( $$PWD/../../../FugioPlugins/include ) {
	INCLUDEPATH += $$PWD/../../../FugioPlugins/include

	SOURCES += mediatimelinenode.cpp \
		mediaplayervideopreview.cpp

	HEADERS += mediatimelinenode.h \
		mediaplayervideopreview.h

	FORMS += mediaplayervideopreview.ui

	DEFINES += TIMELINE_SUPPORTED
}

#------------------------------------------------------------------------------
# ffmpeg

linux {
	LIBS += -L/usr/local/lib

	DEFINES += FFMPEG_SUPPORTED
}

windows:contains( DEFINES, FFMPEG_SUPPORTED ) {
	LIBS += -L$$FFMPEGDIR/lib
	LIBS += -L$$FFMPEGDIR/bin
	INCLUDEPATH += $$FFMPEGDIR/include
	QMAKE_LFLAGS += /OPT:NOREF
}

macx {
	isEmpty( CASKBASE ) {
		FFMPEG_PATH = $$(LIBS)/ffmpeg-build

		exists( $$FFMPEG_PATH ) {
			INCLUDEPATH += $$FFMPEG_PATH/include

			LIBS += -L$$FFMPEG_PATH/lib

			DEFINES += FFMPEG_SUPPORTED
		}
	} else {
		exists( /usr/local/opt/ffmpeg ) {
			INCLUDEPATH += /usr/local/opt/ffmpeg/include

			LIBS += -L/usr/local/opt/ffmpeg/lib

			DEFINES += FFMPEG_SUPPORTED
		}
	}
}

contains( DEFINES, FFMPEG_SUPPORTED ) {
	LIBS += -lavcodec -lavdevice -lavformat -lavutil -lswscale

#	linux:!exists( /usr/include/libswresample/swresample.h ) {
#		DEFINES += TL_USE_LIB_AV
#	}

	!contains( DEFINES, TL_USE_LIB_AV ) {
		LIBS += -lswresample -lavfilter
	}
}

#------------------------------------------------------------------------------
# snappy

contains( DEFINES, FFMPEG_SUPPORTED ) {
	macx {
		isEmpty( CASKBASE ) {
			SNAPPY_PATH = $$(LIBS)/snappy-build

			INCLUDEPATH += $$SNAPPY_PATH/include

			LIBS += -L$$SNAPPY_PATH/lib -lsnappy
		} else {
			INCLUDEPATH += /usr/local/opt/snappy/include

			LIBS += -L/usr/local/opt/snappy/lib
		}
	}

	unix {
	#    INCLUDEPATH += $$(LIBS)/snappy

	#    SOURCES += $$(LIBS)/snappy/snappy-c.cc \
	#        $$(LIBS)/snappy/snappy.cc
	#    HEADERS += $$(LIBS)/snappy/snappy.h \
	#        $$(LIBS)/snappy/snappy-c.h

#		LIBS += -lsnappy
	}

	windows {
		DEFINES += SNAPPY_STATIC

		INCLUDEPATH += $$(LIBS)/snappy-1.1.1.8

		SOURCES += $$(LIBS)/snappy-1.1.1.8/snappy.cc \
			$$(LIBS)/snappy-1.1.1.8/snappy-c.cc \
			$$(LIBS)/snappy-1.1.1.8/snappy-sinksource.cc \
			$$(LIBS)/snappy-1.1.1.8/snappy-stubs-internal.cc

		HEADERS += $$(LIBS)/snappy-1.1.1.8/snappy.h \
			$$(LIBS)/snappy-1.1.1.8/snappy-c.h \
			$$(LIBS)/snappy-1.1.1.8/snappy-internal.h \
			$$(LIBS)/snappy-1.1.1.8/snappy-sinksource.h \
			$$(LIBS)/snappy-1.1.1.8/snappy-stubs-internal.h \
			$$(LIBS)/snappy-1.1.1.8/snappy-stubs-public.h
	}
}

!contains( DEFINES, FFMPEG_SUPPORTED ) {
	warning( "FFMPEG not supported" )
}
