#ifndef recognition_test_Model_hpp
#define recognition_test_Model_hpp

#include <presentation/Presenter.h>
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
        virtual void play() = 0;
        virtual void loadFile(std::string filePath) = 0;
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
        virtual void fadeIn() = 0;
        virtual void fadeOut() = 0;
        virtual void loadFile(std::string filePath) = 0;
    };

    class StimulusList {
    public:
        virtual ~StimulusList() = default;
        virtual void initialize(std::string directory) = 0;
        virtual bool empty() = 0;
        virtual std::string next() = 0;
    };

    class Model :
        public presentation::Model,
        public StimulusPlayer::EventListener,
        public MaskerPlayer::EventListener
    {
        MaskerPlayer *maskerPlayer;
        StimulusList *list;
        StimulusPlayer *stimulusPlayer;
    public:
        Model(
            MaskerPlayer *,
            StimulusList *,
            StimulusPlayer *
        );
        void initializeTest(const TestParameters &) override;
        bool testComplete() override;
        void playTrial(const TrialParameters &) override;
        std::vector<std::string> audioDevices() override;
        void fadeInComplete() override;
        void playbackComplete() override;
    };
}
#endif
