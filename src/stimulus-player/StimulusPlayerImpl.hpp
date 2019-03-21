#ifndef stimulus_player_StimulusPlayerImpl_hpp
#define stimulus_player_StimulusPlayerImpl_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace stimulus_player {
    class VideoPlayer {
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void playbackComplete() = 0;
        };
        
        virtual ~VideoPlayer() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void loadFile(std::string) = 0;
        virtual void play() = 0;
        virtual void setDevice(int index) = 0;
    };
    
    class StimulusPlayerImpl :
        public recognition_test::StimulusPlayer,
        public VideoPlayer::EventListener
    {
        VideoPlayer *player;
        StimulusPlayer::EventListener *listener_{};
    public:
        StimulusPlayerImpl(VideoPlayer *);
        void subscribe(StimulusPlayer::EventListener *) override;
        void setDevice(int index) override;
        void play() override;
        void loadFile(std::string filePath) override;
        void hideVideo() override;
        void showVideo() override;
        double rms() override;
        void setLevel_dB(double) override;
        void playbackComplete() override;
    };
}

#endif
