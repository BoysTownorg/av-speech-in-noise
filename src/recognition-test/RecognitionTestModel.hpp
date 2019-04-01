#ifndef recognition_test_Model_hpp
#define recognition_test_Model_hpp

#include <av-coordinated-response-measure/Model.h>
#include <vector>

namespace recognition_test {
    class InvalidAudioDevice {};
    
    class StimulusPlayer {
    public:
        virtual ~StimulusPlayer() = default;
        
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void playbackComplete() = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual void setAudioDevice(std::string) = 0;
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
            virtual void fadeOutComplete() = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual std::vector<std::string> audioDeviceDescriptions() = 0;
        virtual std::vector<std::string> outputAudioDeviceDescriptions() { return {}; }
        virtual void setAudioDevice(std::string) = 0;
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
    
    class OutputFile {
    public:
        virtual ~OutputFile() = default;
        virtual void openNewFile(
            const av_coordinated_response_measure::Model::Test &
        ) = 0;
        virtual void writeTrial(
            const av_coordinated_response_measure::Trial &
        ) = 0;
    };

    class RecognitionTestModel :
        public av_coordinated_response_measure::Model,
        public StimulusPlayer::EventListener,
        public MaskerPlayer::EventListener
    {
        Test test{};
        MaskerPlayer *maskerPlayer;
        StimulusList *list;
        StimulusPlayer *stimulusPlayer;
        OutputFile *outputFile;
        Model::EventListener *listener_{};
    public:
        RecognitionTestModel(
            MaskerPlayer *,
            StimulusList *,
            StimulusPlayer *,
            OutputFile *
        );
        void initializeTest(const Test &) override;
        void playTrial(const AudioSettings &) override;
        void submitResponse(const SubjectResponse &) override;
        bool testComplete() override;
        std::vector<std::string> audioDevices() override;
        void subscribe(Model::EventListener *) override;
        void fadeInComplete() override;
        void fadeOutComplete() override;
        void playbackComplete() override;
    private:
        void loadStimulusList(const Test &);
        void loadMaskerFile(const Test &);
        bool noMoreTrials();
        bool trialInProgress();
        void loadNextStimulus();
        void preparePlayers(const AudioSettings &);
        void startTrial();
        bool auditoryOnly(const Test &);
        void prepareVideo(const Test &);
        double signalLevel_dB();
        void setAudioDevices(const std::string &);
        void trySettingAudioDevices(const AudioSettings &);
        int findDeviceIndex(const AudioSettings &);
    };
}

#endif
