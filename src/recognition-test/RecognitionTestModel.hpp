#ifndef recognition_test_Model_hpp
#define recognition_test_Model_hpp

#include <presentation/Model.h>
#include <vector>

namespace recognition_test {
    class StimulusPlayer {
    public:
        virtual ~StimulusPlayer() = default;
        
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void playbackComplete() = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual void setDevice(int index) = 0;
        virtual void setDevice_(std::string) = 0;
        virtual void play() = 0;
        virtual void loadFile(std::string filePath) = 0;
        virtual void hideVideo() = 0;
        virtual void showVideo() = 0;
        virtual double rms() = 0;
        virtual void setLevel_dB(double) = 0;
    };

    class MaskerPlayer {
    public:
        virtual ~MaskerPlayer() = default;
        
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void fadeInComplete() = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual int deviceCount() = 0;
        virtual std::string deviceDescription(int index) = 0;
        virtual void setDevice(int index) = 0;
        virtual void setDevice_(std::string) = 0;
        class InvalidAudioDevice {};
        virtual void fadeIn() = 0;
        virtual void fadeOut() = 0;
        virtual void loadFile(std::string filePath) = 0;
        virtual bool playing() = 0;
    };

    class StimulusList {
    public:
        virtual ~StimulusList() = default;
        virtual void loadFromDirectory(std::string directory) = 0;
        virtual bool empty() = 0;
        virtual std::string next() = 0;
    };

    class RecognitionTestModel :
        public presentation::Model,
        public StimulusPlayer::EventListener,
        public MaskerPlayer::EventListener
    {
        Test testParameters{};
        MaskerPlayer *maskerPlayer;
        StimulusList *list;
        StimulusPlayer *stimulusPlayer;
    public:
        RecognitionTestModel(
            MaskerPlayer *,
            StimulusList *,
            StimulusPlayer *
        );
        void initializeTest(const Test &) override;
        void playTrial(const Trial &) override;
        void submitResponse(const SubjectResponse &) override;
        bool testComplete() override;
        std::vector<std::string> audioDevices() override;
        void fadeInComplete() override;
        void playbackComplete() override;
    private:
        int findDeviceIndex(const Trial &trial);
    };
}
#endif
