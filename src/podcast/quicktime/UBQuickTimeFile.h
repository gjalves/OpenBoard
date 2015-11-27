/*
 * Copyright (C) 2013 Open Education Foundation
 *
 * Copyright (C) 2010-2013 Groupement d'Intérêt Public pour
 * l'Education Numérique en Afrique (GIP ENA)
 *
 * This file is part of OpenBoard.
 *
 * OpenBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * OpenBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBoard. If not, see <http://www.gnu.org/licenses/>.
 */




#ifndef UBQUICKTIMEFILE_H_
#define UBQUICKTIMEFILE_H_

#include <QtCore>

#include <CoreVideo/CoreVideo.h>
#include <CoreMedia/CoreMedia.h>

#include "UBAudioQueueRecorder.h"



// Trick to get around the fact that the C++ compiler doesn't
// like Objective C code.

#ifdef __OBJC__ // defined by the Objective C compiler
    @class AVAssetWriter;
    @class AVAssetWriterInput;
    @class AVAssetWriterInputPixelBufferAdaptor;

    typedef AVAssetWriter* AssetWriterPTR;
    typedef AVAssetWriterInput* AssetWriterInputPTR;
    typedef AVAssetWriterInputPixelBufferAdaptor* AssetWriterInputAdaptorPTR;
#else
    typedef void* AssetWriterPTR;
    typedef void* AssetWriterInputPTR;
    typedef void* AssetWriterInputAdaptorPTR;
#endif

class UBQuickTimeFile : public QThread
{
    Q_OBJECT;

    public:
        UBQuickTimeFile(QObject * pParent = 0);
        virtual ~UBQuickTimeFile();

        bool init(const QString& videoFileName, const QString& profileData
                , int pFramesPerSecond, const QSize& pFrameSize
                , bool recordAudio = true, const QString& audioDeviceName = QString("Default"));

        void stop();

        CVPixelBufferRef newPixelBuffer();

        bool isCompressionSessionRunning() { return mCompressionSessionRunning; }

        QString lastErrorMessage() const { return mLastErrorMessage; }

        void endSession();

        struct VideoFrame
        {
            CVPixelBufferRef buffer;
            long timestamp;
        };

        static QQueue<VideoFrame> frameQueue;
        static QMutex frameQueueMutex;
        static QWaitCondition frameBufferNotEmpty;

    signals:
        void audioLevelChanged(quint8 level);
        void compressionSessionStarted();

    protected:
        void run();

    private slots:
        void appendAudioBuffer(void* pBuffer, long pLength);

    private:

        bool beginSession();
        void setLastErrorMessage(const QString& error);

        void appendVideoFrame(CVPixelBufferRef pixelBuffer, long msTimeStamp);

        
        QSize mFrameSize;
        QString mVideoFileName;

        AssetWriterPTR mVideoWriter;

        AssetWriterInputPTR mVideoWriterInput;
        AssetWriterInputAdaptorPTR mAdaptor;

        AssetWriterInputPTR mAudioWriterInput;

        QPointer<UBAudioQueueRecorder> mWaveRecorder;
        CFAbsoluteTime mStartTime;

        CMAudioFormatDescriptionRef mAudioFormatDescription;
        
        long mTimeScale;
        bool mRecordAudio;
        
        volatile bool mShouldStopCompression;
        volatile bool mCompressionSessionRunning;

        

        QString mLastErrorMessage;

        QString mAudioRecordingDeviceName;

};

#endif /* UBQUICKTIMEFILE_H_ */
