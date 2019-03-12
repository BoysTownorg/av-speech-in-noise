#include "Model.hpp"
#include <gsl/gsl>

namespace recognition_test {
    Model::Model(
        MaskerPlayer *maskerPlayer,
        StimulusList *list,
        StimulusPlayer *stimulusPlayer
    ) :
        maskerPlayer{maskerPlayer},
        list{list},
        stimulusPlayer{stimulusPlayer}
    {
        maskerPlayer->subscribe(this);
        stimulusPlayer->subscribe(this);
    }
    
    void Model::playTrial(const TrialParameters &trial) {
        if (list->empty())
            return;
        
        auto devices_ = audioDevices();
        auto deviceIndex = gsl::narrow<int>(
            std::find(
                devices_.begin(),
                devices_.end(),
                trial.audioDevice
            ) - devices_.begin()
        );
        maskerPlayer->setDevice(deviceIndex);
        stimulusPlayer->setDevice(deviceIndex);
        stimulusPlayer->loadFile(list->next());
        if (maskerPlayer->playing())
            return;
        maskerPlayer->fadeIn();
    }

    void Model::initializeTest(const TestParameters &p) {
        maskerPlayer->loadFile(p.maskerFilePath);
        list->initialize(p.stimulusListDirectory);
    }

    bool Model::testComplete() {
        return list->empty();
    }
    
    void Model::fadeInComplete() {
        stimulusPlayer->play();
    }
    
    void Model::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    std::vector<std::string> Model::audioDevices() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < maskerPlayer->deviceCount(); ++i)
            descriptions.push_back(maskerPlayer->deviceDescription(i));
        return descriptions;
    }
    
    void Model::submitResponse(const ResponseParameters &) { 
        ;
    }
}

