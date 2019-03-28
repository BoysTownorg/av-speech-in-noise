#include "assert-utility.h"
#include <masker-player/RandomizedMaskerPlayer.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace {
    class AudioPlayerStub : public masker_player::AudioPlayer {
        std::vector<std::string> audioDeviceDescriptions_{10};
        std::string filePath_{};
        std::string deviceDescription_{};
        double sampleRateHz_{};
        int deviceIndex_{};
        int deviceCount_{};
        int deviceDescriptionDeviceIndex_{};
        EventListener *listener_{};
        bool playing_{};
        bool played_{};
        bool stopped_{};
        bool callbackScheduled_{};
    public:
        void clearCallbackCount() {
            callbackScheduled_ = false;
        }
        
        auto callbackScheduled() const {
            return callbackScheduled_;
        }
        
        void timerCallback() {
            listener_->timerCallback();
        }
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioDeviceDescriptions_ = std::move(v);
        }
        
        void stop() override {
            stopped_ = true;
        }
        
        auto stopped() const {
            return stopped_;
        }
        
        double sampleRateHz() override {
            return sampleRateHz_;
        }
        
        void setSampleRateHz(double x) {
            sampleRateHz_ = x;
        }
        
        void setPlaying() {
            playing_ = true;
        }
        
        bool playing() override {
            return playing_;
        }
        
        void loadFile(std::string s) override {
            filePath_ = std::move(s);
        }
        
        void setDevice(int index) override {
            deviceIndex_ = index;
        }
        
        int deviceCount() override {
            return gsl::narrow<int>(audioDeviceDescriptions_.size());
        }
        
        std::string deviceDescription(int index) override {
            deviceDescriptionDeviceIndex_ = index;
            return audioDeviceDescriptions_.at(index);
        }
        
        void play() override {
            played_ = true;
        }
        
        void subscribe(EventListener * listener) override {
            listener_ = listener;
        }
        
        void scheduleCallbackAfterSeconds(double) override {
            callbackScheduled_ = true;
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        auto deviceIndex() const {
            return deviceIndex_;
        }
        
        auto played() const {
            return played_;
        }
        
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
            listener_->fillAudioBuffer(audio);
        }
    };
    
    class MaskerPlayerListenerStub :
        public recognition_test::MaskerPlayer::EventListener
    {
        int fadeInCompletions_{};
        int fadeOutCompletions_{};
        bool fadeInCompleted_{};
        bool fadeOutCompleted_{};
    public:
        void fadeInComplete() override {
            fadeInCompleted_ = true;
            ++fadeInCompletions_;
        }
        
        void fadeOutComplete() override {
            ++fadeOutCompletions_;
            fadeOutCompleted_ = true;
        }
        
        auto fadeInCompleted() const {
            return fadeInCompleted_;
        }
        
        auto fadeOutCompleted() const {
            return fadeOutCompleted_;
        }
        
        auto fadeInCompletions() const {
            return fadeInCompletions_;
        }
        
        auto fadeOutCompletions() const {
            return fadeOutCompletions_;
        }
    };

    class RandomizedMaskerPlayerTests : public ::testing::Test {
    protected:
        std::vector<float> leftChannel{};
        std::vector<float> rightChannel{};
        AudioPlayerStub audioPlayer;
        MaskerPlayerListenerStub listener;
        masker_player::RandomizedMaskerPlayer player{&audioPlayer};
        
        RandomizedMaskerPlayerTests() {
            player.subscribe(&listener);
        }
        
        void fillAudioBufferMono() {
            audioPlayer.fillAudioBuffer({ leftChannel });
        }
        
        void fillAudioBufferStereo() {
            audioPlayer.fillAudioBuffer({ leftChannel, rightChannel });
        }
        
        std::vector<float> halfHannWindow(int N) {
            const auto pi = std::acos(-1);
            std::vector<float> window;
            for (int n = 0; n < (N + 1) / 2; ++n)
                window.push_back((1 - std::cos((2*pi*n)/(N - 1))) / 2);
            return window;
        }
        
        std::vector<float> backHalfHannWindow(int N) {
            auto frontHalf = halfHannWindow(N);
            std::reverse(frontHalf.begin(), frontHalf.end());
            return frontHalf;
        }
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioPlayer.setAudioDeviceDescriptions(std::move(v));
        }
        
        void fadeInToFullLevel() {
            completeFadeCycle(&RandomizedMaskerPlayerTests::fadeIn);
        }
        
        void fadeOutToSilence() {
            completeFadeCycle(&RandomizedMaskerPlayerTests::fadeOut);
        }
        
        void completeFadeCycle(void (RandomizedMaskerPlayerTests::*f)()) {
            player.setFadeInOutSeconds(2);
            audioPlayer.setSampleRateHz(3);
            (this->*f)();
            resizeChannels(2 * 3 + 1);
            fillAudioBufferMono();
        }
        
        void fadeIn() {
            player.fadeIn();
        }
        
        void fadeOut() {
            player.fadeOut();
        }
        
        void resizeChannels(int n) {
            leftChannel.resize(n);
            rightChannel.resize(n);
        }
        
        void timerCallback() {
            audioPlayer.timerCallback();
        }
        
        void assertCallbackScheduled() {
            EXPECT_TRUE(audioPlayer.callbackScheduled());
        }
        
        void assertCallbackNotScheduled() {
            EXPECT_FALSE(audioPlayer.callbackScheduled());
        }
    };

    TEST_F(RandomizedMaskerPlayerTests, playingWhenVideoPlayerPlaying) {
        audioPlayer.setPlaying();
        EXPECT_TRUE(player.playing());
    }

    TEST_F(RandomizedMaskerPlayerTests, loadFileLoadsVideoFile) {
        player.loadFile("a");
        assertEqual("a", audioPlayer.filePath());
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInPlaysVideoPlayer) {
        fadeIn();
        EXPECT_TRUE(audioPlayer.played());
    }

    TEST_F(RandomizedMaskerPlayerTests, fadesInAccordingToHannFunction) {
        player.setFadeInOutSeconds(5);
        audioPlayer.setSampleRateHz(6);
        auto window = halfHannWindow(2 * 5 * 6 + 1);
    
        fadeIn();
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(0) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 9, leftChannel.at(2), 1e-6);
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(3) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 9, leftChannel.at(2), 1e-6);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadesInAccordingToHannFunctionStereo) {
        player.setFadeInOutSeconds(5);
        audioPlayer.setSampleRateHz(6);
        auto window = halfHannWindow(2 * 5 * 6 + 1);
    
        fadeIn();
        leftChannel = { 1, 2, 3 };
        rightChannel = { 7, 8, 9 };
        fillAudioBufferStereo();
        EXPECT_NEAR(window.at(0) * 1, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 2, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 3, leftChannel.at(2), 1e-6);
        EXPECT_NEAR(window.at(0) * 7, rightChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 8, rightChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 9, rightChannel.at(2), 1e-6);
        leftChannel = { 1, 2, 3 };
        rightChannel = { 7, 8, 9 };
        fillAudioBufferStereo();
        EXPECT_NEAR(window.at(3) * 1, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 2, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 3, leftChannel.at(2), 1e-6);
        EXPECT_NEAR(window.at(3) * 7, rightChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 8, rightChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 9, rightChannel.at(2), 1e-6);
    }

    TEST_F(RandomizedMaskerPlayerTests, steadyLevelFollowingFadeIn) {
        fadeInToFullLevel();
        
        leftChannel = { 1, 2, 3 };
        fillAudioBufferMono();
        assertEqual({ 1, 2, 3 }, leftChannel);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadesOutAccordingToHannFunction) {
        fadeInToFullLevel();
        player.setFadeInOutSeconds(5);
        audioPlayer.setSampleRateHz(6);
        auto window = backHalfHannWindow(2 * 5 * 6 + 1);
        
        fadeOut();
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(0) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 9, leftChannel.at(2), 1e-6);
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(3) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 9, leftChannel.at(2), 1e-6);
    }

    TEST_F(RandomizedMaskerPlayerTests, steadyLevelFollowingFadeOut) {
        fadeInToFullLevel();
        fadeOutToSilence();
        
        leftChannel = { 1, 2, 3 };
        fillAudioBufferMono();
        assertEqual({ 0, 0, 0 }, leftChannel, 1e-15f);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInCompleteOnlyAfterFadeTime) {
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(4);
        
        fadeIn();
        resizeChannels(1);
        for (int i = 0; i < 3 * 4; ++i) {
            fillAudioBufferMono();
            timerCallback();
            EXPECT_FALSE(listener.fadeInCompleted());
        }
        fillAudioBufferMono();
        timerCallback();
        EXPECT_TRUE(listener.fadeInCompleted());
    }

    TEST_F(RandomizedMaskerPlayerTests, observerNotifiedOnceForFadeIn) {
        fadeInToFullLevel();
        timerCallback();
        EXPECT_EQ(1, listener.fadeInCompletions());
        fillAudioBufferMono();
        timerCallback();
        EXPECT_EQ(1, listener.fadeInCompletions());
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeOutCompleteOnlyAfterFadeTime) {
        fadeInToFullLevel();
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(4);
        
        fadeOut();
        resizeChannels(1);
        for (int i = 0; i < 3 * 4; ++i) {
            fillAudioBufferMono();
            timerCallback();
            EXPECT_FALSE(listener.fadeOutCompleted());
        }
        fillAudioBufferMono();
        timerCallback();
        EXPECT_TRUE(listener.fadeOutCompleted());
    }

    TEST_F(RandomizedMaskerPlayerTests, observerNotifiedOnceForFadeOut) {
        fadeInToFullLevel();
        audioPlayer.timerCallback();
        
        fadeOutToSilence();
        timerCallback();
        EXPECT_EQ(1, listener.fadeOutCompletions());
        fillAudioBufferMono();
        timerCallback();
        EXPECT_EQ(1, listener.fadeOutCompletions());
    }

    TEST_F(RandomizedMaskerPlayerTests, audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
        fadeInToFullLevel();
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(4);
        
        fadeOut();
        resizeChannels(1);
        for (int i = 0; i < 3 * 4; ++i) {
            fillAudioBufferMono();
            timerCallback();
            EXPECT_FALSE(audioPlayer.stopped());
        }
        fillAudioBufferMono();
        timerCallback();
        EXPECT_TRUE(audioPlayer.stopped());
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInSchedulesCallback) {
        fadeIn();
        assertCallbackScheduled();
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeOutSchedulesCallback) {
        fadeOut();
        assertCallbackScheduled();
    }

    TEST_F(RandomizedMaskerPlayerTests, callbackSchedulesAdditionalCallback) {
        timerCallback();
        assertCallbackScheduled();
    }

    TEST_F(RandomizedMaskerPlayerTests, callbackDoesNotScheduleAdditionalCallbackWhenFadeInComplete) {
        fadeInToFullLevel();
        audioPlayer.clearCallbackCount();
        
        timerCallback();
        assertCallbackNotScheduled();
    }

    TEST_F(RandomizedMaskerPlayerTests, callbackDoesNotScheduleAdditionalCallbackWhenFadeOutComplete) {
        fadeInToFullLevel();
        timerCallback();
        fadeOutToSilence();
        audioPlayer.clearCallbackCount();
        
        timerCallback();
        assertCallbackNotScheduled();
    }

    TEST_F(RandomizedMaskerPlayerTests, setAudioDeviceFindsIndex) {
        setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
        player.setAudioDevice("second");
        EXPECT_EQ(2, audioPlayer.deviceIndex());
    }

    TEST_F(RandomizedMaskerPlayerTests, audioDevicesReturnsDescriptions) {
        setAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, player.audioDeviceDescriptions());
    }
}
