#include "assert-utility.h"
#include <stimulus-player/StimulusPlayerImpl.hpp>
#include <gtest/gtest.h>

namespace {
    class VideoPlayerStub : public stimulus_player::VideoPlayer {
        std::string filePath_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool shown_{};
        bool hidden_{};
        bool played_{};
    public:
        void playbackComplete() {
            listener_->playbackComplete();
        }
        
        void setDevice(int index) override {
            deviceIndex_ = index;
        }
        
        auto deviceIndex() const {
            return deviceIndex_;
        }
        
        void play() override {
            played_ = true;
        }
        
        auto played() const {
            return played_;
        }
        
        void loadFile(std::string f) override {
            filePath_ = std::move(f);
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        void hide() override {
            hidden_ = true;
        }
        
        auto hidden() const {
            return hidden_;
        }
        
        auto shown() const {
            return shown_;
        }
        
        void show() override {
            shown_ = true;
        }
        
        void subscribe(EventListener *e) override {
            listener_ = e;
        }
        
    };
    
    class StimulusPlayerListenerStub :
        public recognition_test::StimulusPlayer::EventListener
    {
        bool notified_{};
    public:
        void playbackComplete() override {
            notified_ = true;
        }
        
        auto notified() const {
            return notified_;
        }
    };

    class StimulusPlayerTests : public ::testing::Test {
    protected:
        VideoPlayerStub videoPlayer;
        StimulusPlayerListenerStub listener;
        stimulus_player::StimulusPlayerImpl player{&videoPlayer};
        
        StimulusPlayerTests() {
            player.subscribe(&listener);
        }
    };

    TEST_F(StimulusPlayerTests, playPlaysVideo) {
        player.play();
        EXPECT_TRUE(videoPlayer.played());
    }

    TEST_F(StimulusPlayerTests, showVideoShowsVideo) {
        player.showVideo();
        EXPECT_TRUE(videoPlayer.shown());
    }

    TEST_F(StimulusPlayerTests, hideVideoHidesVideo) {
        player.hideVideo();
        EXPECT_TRUE(videoPlayer.hidden());
    }

    TEST_F(StimulusPlayerTests, loadFileLoadsFile) {
        player.loadFile("a");
        assertEqual("a", videoPlayer.filePath());
    }

    TEST_F(StimulusPlayerTests, setDeviceSetsVideoPlayerDevice) {
        player.setDevice(1);
        EXPECT_EQ(1, videoPlayer.deviceIndex());
    }

    TEST_F(StimulusPlayerTests, videoPlaybackCompleteNotifiesSubscriber) {
        videoPlayer.playbackComplete();
        EXPECT_TRUE(listener.notified());
    }
}
