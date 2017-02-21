#ifndef MEDIATIMELINENODE_H
#define MEDIATIMELINENODE_H

#include <QObject>
#include <QImage>
#include <QThread>

#include <qmath.h>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/audio/audio_instance_base.h>

#include <fugio/ffmpeg/uuid.h>

#include <fugio/audio/audio_producer_interface.h>
#include <fugio/playhead_interface.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/interface_timeline_track.h>

#include <fugio/interface_keyframes.h>
#include <fugio/interface_keyframes_time.h>
#include <fugio/interface_keyframes_background.h>

#include "segmentinterface.h"

#include "mediaaudioprocessor.h"

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

class InterfaceSegment;
class InterfaceTimelineControl;
class QPushButton;

typedef struct AudioBuffer3
{
	QVector<QVector<float>>	 AudDat;
	QVector<QVector<float>>	 AudMix;
	QVector<QVector<float>>	 AudBuf;

	void clear( void )
	{
		for( QVector<float> &V : AudDat )
		{
			V.clear();
		}

		for( QVector<float> &V : AudMix )
		{
			V.clear();
		}

		for( QVector<float> &V : AudBuf )
		{
			V.clear();
		}

//		AudDat.clear();
//		AudMix.clear();
//		AudBuf.clear();
	}

	void setChannels( int c )
	{
		AudDat.resize( c );
		AudMix.resize( c );
		AudBuf.resize( c );
	}

	static inline float mix( float f1, float f2, float v )
	{
		return( ( f1 * v ) + ( f2 * ( 1.0f - v ) ) );
	}
} AudioBuffer3;

using namespace fugio;

class MediaTimelineNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface, public fugio::PlayheadInterface, public InterfaceTimelineTrack, public InterfaceKeyFramesBackground
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface fugio::PlayheadInterface InterfaceTimelineTrack InterfaceKeyFramesBackground )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "FFMPEG audio/video timeline player" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Media_Timeline" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MediaTimelineNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MediaTimelineNode( void );

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings & ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings & ) const Q_DECL_OVERRIDE;

	virtual QStringList availableInputPins( void ) const Q_DECL_OVERRIDE;
	virtual QList<NodeControlInterface::AvailablePinEntry> availableOutputPins( void ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::PlayheadInterface

	virtual void playStart( qreal pTimeStamp ) Q_DECL_OVERRIDE
	{
		playheadMove( pTimeStamp );
	}

	virtual void playheadMove( qreal pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool playheadPlay( qreal pTimePrev, qreal pTimeCurr ) Q_DECL_OVERRIDE;

	virtual void setTimeOffset( qreal pTimeOffset ) Q_DECL_OVERRIDE;

	virtual qreal latency( void ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::AudioProducerInterface

	virtual fugio::AudioInstanceBase *audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
//	virtual void audioFreeInstance(void *pInstanceData) Q_DECL_OVERRIDE;
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pInstance )

		return( true );
	}

	//-------------------------------------------------------------------------
	// InterfaceTimelineTrack

	virtual QObject *qobject( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	virtual qreal position( void ) const Q_DECL_OVERRIDE;

	virtual qreal duration( void ) const Q_DECL_OVERRIDE;

	virtual InterfaceKeyFramesWidget *newTimelineGui( void ) Q_DECL_OVERRIDE;

	virtual InterfaceTimelineControl *control( void ) Q_DECL_OVERRIDE
	{
		return( mTimelineControl );
	}

	virtual fugio::NodeControlInterface *nodeControl( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	virtual InterfaceKeyFrames *keyframes( void ) Q_DECL_OVERRIDE
	{
		return( mKF->keyframes() );
	}

	virtual QList<InterfaceKeyFramesControls *> editorControls( void ) Q_DECL_OVERRIDE;

	virtual bool canRecord( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	//-------------------------------------------------------------------------
	// InterfaceTimelineTrack

	virtual void drawBackground( const InterfaceKeyFramesWidget *pTrackWidget, QPainter &pPainter, const QRect &pUpdateRect ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------

	fugio::ImageInterface *image( void ) const
	{
		return( mValImage );
	}

	qreal lastImageUpdate( void ) const
	{
		return( mLastImageUpdate );
	}

	void updateVideo(qreal pTimeCurr);

protected slots:
	void stateChanged( ContextInterface::TimeState pState );

	void buttonPreload( bool pState );

	void aboutToPlay( void );

private:
	bool loadMedia( const QString &pFileName );

	void unloadMedia( void );

	void setVideo( SegmentInterface *pSegment );

	void setInstanceSampleOffset( qint64 pSampleOffset );

private:
	InterfaceTimelineControl						*mTimelineControl;

	QSharedPointer<fugio::PinInterface>				 mPinFilename;
	QSharedPointer<fugio::PinInterface>				 mPinVolume;
//	QSharedPointer<fugio::PinInterface>				 mPinAlpha;
//	QSharedPointer<fugio::PinInterface>				 mPinScratch;

	QSharedPointer<fugio::PinInterface>				 mPinImage;
	fugio::ImageInterface							*mValImage;

	QSharedPointer<fugio::PinInterface>				 mPinAudio;
	fugio::AudioProducerInterface					*mValAudio;

	fugio::SegmentInterface							*mSegment;

	InterfaceKeyFramesTime							*mKF;

	bool											 mAudioMute;

	mutable QImage	 mAudioImage;
	mutable qreal	 mLastAudioViewStart;
	mutable qreal	 mLastAudioViewEnd;

	qreal									 mAudioVolume;
//	qreal									 mVideoAlpha;

	qreal									 mLastImageUpdate;

	QPushButton								*mGUI;

	bool									 mPreloadAudio;

	qreal									 mTimeOffset;
	qreal									 mTimeLast;

	AudioBuffer3							 AB;

	qint64									 mFrameDecodeTime;

	class AudioInstanceData : public fugio::AudioInstanceBase
	{
	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{

		}

		virtual ~AudioInstanceData( void ) {}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) Q_DECL_OVERRIDE
		{
			QSharedPointer<MediaTimelineNode>	API = qSharedPointerCast<MediaTimelineNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
		qint64								mSampleOffset;
	};

	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;

	QMutex									mInstanceMutex;
	QList<AudioInstanceData *>				mInstanceList;
};

#endif // MEDIATIMELINENODE_H