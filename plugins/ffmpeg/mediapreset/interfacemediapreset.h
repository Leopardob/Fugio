#ifndef INTERFACEMEDIAPRESET_H
#define INTERFACEMEDIAPRESET_H

#include <QString>

struct AVCodecContext;
struct AVStream;

class InterfaceMediaPreset
{
public:
	virtual ~InterfaceMediaPreset( void ) {}

	virtual QString fileExt( void ) const = 0;

	virtual QString filter( void ) const = 0;

	virtual qreal videoFramesPerSecond( void ) const = 0;

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const = 0;

	virtual qreal audioSampleRate( void ) const = 0;

	virtual void fillAudioCodecContext( AVCodecContext *pContext ) const = 0;

	virtual bool hasVideo( void ) const = 0;
	virtual bool hasAudio( void ) const = 0;

	virtual void setQuality( AVCodecContext *pContext, AVStream *pVideoStream, qreal pQuality ) const = 0;
	virtual void setSpeed( AVCodecContext *pContext, AVStream *pVideoStream, qreal pSpeed ) const = 0;
};

#endif // INTERFACEMEDIAPRESET_H
