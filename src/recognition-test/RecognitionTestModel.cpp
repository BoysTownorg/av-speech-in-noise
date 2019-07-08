#include "RecognitionTestModel.hpp"
#include <cmath>

namespace av_speech_in_noise {
    class NullTrack : public Track {
        void pushDown() override {}
        void pushUp() override {}
        int x() override { return {}; }
        bool complete() override { return {}; }
        int reversals() override { return {}; }
    };
    
    class NullTargetList : public TargetList {
        void loadFromDirectory(std::string) override {}
        std::string next() override { return {}; }
        std::string current() override { return {}; }
    };
    
    static NullTargetList nullTargetList;
    static NullTrack nullTrack;
    
    AdaptiveMethod::AdaptiveMethod(
        TargetListReader *targetListSetReader,
        TrackFactory *snrTrackFactory,
        ResponseEvaluator *evaluator,
        Randomizer *randomizer
    ) :
        targetListSetReader{targetListSetReader},
        snrTrackFactory{snrTrackFactory},
        evaluator{evaluator},
        randomizer{randomizer},
        currentSnrTrack{&nullTrack},
        currentTargetList{&nullTargetList} {}
    
    void AdaptiveMethod::store(const AdaptiveTest &p) {
        trackSettings.ceiling = p.ceilingSnr_dB;
        trackSettings.rule = p.targetLevelRule;
        trackSettings.startingX = p.startingSnr_dB;
    }
    
    int AdaptiveMethod::snr_dB() {
        return currentSnrTrack->x();
    }
    
    void AdaptiveMethod::loadTargets(const std::string &p) {
        lists = targetListSetReader->read(p);
        prepareSnrTracks();
    }
    
    void AdaptiveMethod::submitIncorrectResponse() {
        pushUpTrack();
    }
    
    void AdaptiveMethod::submitCorrectResponse() {
        pushDownTrack();
    }
    
    bool AdaptiveMethod::complete() {
        return std::all_of(
            targetListsWithTracks.begin(),
            targetListsWithTracks.end(),
            [](const TargetListWithTrack &t) {
                return t.track->complete();
            }
        );
    }
    
    std::string AdaptiveMethod::next() {
        return currentTargetList->next();
    }
    
    std::string AdaptiveMethod::current() {
        return currentTargetList->current();
    }
    
    void AdaptiveMethod::writeTrial(
        OutputFile *file,
        const coordinate_response_measure::SubjectResponse &response
    ) {
        coordinate_response_measure::AdaptiveTrial trial;
        trial.trial.subjectColor = response.color;
        trial.trial.subjectNumber = response.number;
        trial.reversals = currentSnrTrack->reversals();
        trial.trial.correctColor = evaluator->correctColor(current());
        trial.trial.correctNumber = evaluator->correctNumber(current());
        trial.SNR_dB = snr_dB();
        trial.trial.correct = correct(response);
        file->writeTrial(trial);
    }
    
    void AdaptiveMethod::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        if (correct(response))
            pushDownTrack();
        else
            pushUpTrack();
    }

    bool AdaptiveMethod::correct(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        return evaluator->correct(current(), response);
    }
    
    void AdaptiveMethod::pushUpTrack() {
        currentSnrTrack->pushUp();
        selectNextList();
    }
    
    void AdaptiveMethod::pushDownTrack() {
        currentSnrTrack->pushDown();
        selectNextList();
    }

    void AdaptiveMethod::prepareSnrTracks() {
        targetListsWithTracks.clear();
        for (auto list : lists)
            makeTrackWithList(list.get());
        selectNextList();
    }
    
    void AdaptiveMethod::makeTrackWithList(
        TargetList *list
    ) {
        targetListsWithTracks.push_back({
            list,
            snrTrackFactory->make(trackSettings)
        });
    }

    void AdaptiveMethod::selectNextList() {
        removeCompleteTracks();
        auto remainingListCount = gsl::narrow<int>(targetListsWithTracks.size());
        size_t n = randomizer->randomIntBetween(0, remainingListCount - 1);
        if (n < targetListsWithTracks.size()) {
            currentSnrTrack = targetListsWithTracks.at(n).track.get();
            currentTargetList = targetListsWithTracks.at(n).list;
        }
    }
    
    void AdaptiveMethod::removeCompleteTracks() {
        targetListsWithTracks.erase(
            std::remove_if(
                targetListsWithTracks.begin(),
                targetListsWithTracks.end(),
                [](const TargetListWithTrack &t) {
                    return t.track->complete();
                }
            ),
            targetListsWithTracks.end()
        );
    }
    
    FixedLevelMethod::FixedLevelMethod(
        FiniteTargetList *targetList,
        ResponseEvaluator *evaluator
    ) :
        targetList{targetList},
        evaluator{evaluator} {}
    
    void FixedLevelMethod::store(const FixedLevelTest &p) {
        snr_dB_ = p.snr_dB;
    }
    
    int FixedLevelMethod::snr_dB() {
        return snr_dB_;
    }
    
    void FixedLevelMethod::loadTargets(const std::string &p) {
        targetList->loadFromDirectory(p);
        complete_ = targetList->empty();
    }
    
    std::string FixedLevelMethod::next() {
        return targetList->next();
    }
    
    bool FixedLevelMethod::complete() {
        return complete_;
    }
    
    std::string FixedLevelMethod::current() {
        return targetList->current();
    }
    
    void FixedLevelMethod::submitIncorrectResponse() {
        
    }
    
    void FixedLevelMethod::submitCorrectResponse() {
        
    }
    
    void FixedLevelMethod::writeTrial(
        OutputFile *file,
        const coordinate_response_measure::SubjectResponse &response
    ) {
        coordinate_response_measure::FixedLevelTrial trial;
        trial.trial.subjectColor = response.color;
        trial.trial.subjectNumber = response.number;
        auto current_ = current();
        trial.trial.correctColor = evaluator->correctColor(current_);
        trial.trial.correctNumber = evaluator->correctNumber(current_);
        trial.trial.correct = evaluator->correct(current_, response);
        file->writeTrial(trial);
    }
    
    void FixedLevelMethod::submitResponse(
        const coordinate_response_measure::SubjectResponse &
    ) {
        complete_ = targetList->empty();
    }

    class NullTestMethod : public TestMethod {
        bool complete() override { return {}; }
        std::string next() override { return {}; }
        std::string current() override { return {}; }
        void loadTargets(const std::string &) override {}
        int snr_dB() override { return {}; }
        void submitCorrectResponse() override {}
        void submitIncorrectResponse() override {}
        void writeTrial(OutputFile *, const coordinate_response_measure::SubjectResponse &) override {}
        void submitResponse(const coordinate_response_measure::SubjectResponse &) override {}
    };
    
    static NullTestMethod nullTestMethod;
    
    RecognitionTestModel::RecognitionTestModel(
        AdaptiveMethod *adaptiveMethod,
        FixedLevelMethod *fixedLevelMethod,
        TargetPlayer *targetPlayer,
        MaskerPlayer *maskerPlayer,
        ResponseEvaluator *evaluator,
        OutputFile *outputFile,
        Randomizer *randomizer
    ) :
        adaptiveMethod{adaptiveMethod},
        fixedLevelMethod{fixedLevelMethod},
        maskerPlayer{maskerPlayer},
        targetPlayer{targetPlayer},
        evaluator{evaluator},
        outputFile{outputFile},
        randomizer{randomizer},
        testMethod{&nullTestMethod}
    {
        targetPlayer->subscribe(this);
        maskerPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::subscribe(Model::EventListener *listener) {
        listener_ = listener;
    }
    
    void RecognitionTestModel::initializeTest(const FixedLevelTest &p) {
        throwIfTrialInProgress();
        
        fixedLevelMethod->store(p);
        testMethod = fixedLevelMethod;
        prepareCommonTest(p.common);
        tryOpeningOutputFile(p.information);
        outputFile->writeTest(p);
    }
    
    void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
        throwIfTrialInProgress();
        
        adaptiveMethod->store(p);
        testMethod = adaptiveMethod;
        prepareCommonTest(p.common);
        tryOpeningOutputFile(p.information);
        outputFile->writeTest(p);
    }
    
    void RecognitionTestModel::throwIfTrialInProgress() {
        if (trialInProgress())
            throw RequestFailure{"Trial in progress."};
    }
    
    bool RecognitionTestModel::trialInProgress() {
        return maskerPlayer->playing();
    }
    
    void RecognitionTestModel::prepareCommonTest(const CommonTest &common) {
        storeLevels(common);
        prepareMasker(common.maskerFilePath);
        prepareVideo(common.condition);
        testMethod->loadTargets(common.targetListDirectory);
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::storeLevels(const CommonTest &common) {
        fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
        maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
    }
    
    void RecognitionTestModel::prepareMasker(const std::string &p) {
        loadMaskerFile(p);
        maskerPlayer->setLevel_dB(maskerLevel_dB());
    }
    
    void RecognitionTestModel::loadMaskerFile(const std::string &p) {
        maskerPlayer->loadFile(p);
    }
    
    static double dB(double x) {
        return 20 * std::log10(x);
    }
    
    double RecognitionTestModel::maskerLevel_dB() {
        return desiredMaskerLevel_dB() - dB(maskerPlayer->rms());
    }
    
    int RecognitionTestModel::desiredMaskerLevel_dB() {
        return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
    }
    
    void RecognitionTestModel::prepareVideo(const Condition &p) {
        if (auditoryOnly(p))
            targetPlayer->hideVideo();
        else
            targetPlayer->showVideo();
    }

    bool RecognitionTestModel::auditoryOnly(const Condition &c) {
        return c == Condition::auditoryOnly;
    }
    
    void RecognitionTestModel::preparePlayersForNextTrial() {
        prepareTargetPlayer();
        seekRandomMaskerPosition();
    }
    
    void RecognitionTestModel::prepareTargetPlayer() {
        loadTargetFile(testMethod->next());
        setTargetLevel_dB(targetLevel_dB());
        targetPlayer->subscribeToPlaybackCompletion();
    }
    
    void RecognitionTestModel::loadTargetFile(std::string s) {
        targetPlayer->loadFile(std::move(s));;
    }
    
    void RecognitionTestModel::setTargetLevel_dB(double x) {
        targetPlayer->setLevel_dB(x);
    }
    
    double RecognitionTestModel::targetLevel_dB() {
        return
            desiredMaskerLevel_dB() +
            testMethod->snr_dB() -
            unalteredTargetLevel_dB();
    }
    
    double RecognitionTestModel::unalteredTargetLevel_dB() {
        return dB(targetPlayer->rms());
    }
    
    void RecognitionTestModel::seekRandomMaskerPosition() {
        auto upperLimit =
            maskerPlayer->durationSeconds() -
            2 * maskerPlayer->fadeTimeSeconds() -
            targetPlayer->durationSeconds();
        maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
    }
    
    void RecognitionTestModel::tryOpeningOutputFile(const TestInformation &p) {
        outputFile->close();
        tryOpeningOutputFile_(p);
    }
    
    void RecognitionTestModel::tryOpeningOutputFile_(const TestInformation &p) {
        try {
            outputFile->openNewFile(p);
        } catch (const OutputFile::OpenFailure &) {
            throw RequestFailure{"Unable to open output file."};
        }
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        throwIfTrialInProgress();
        
        preparePlayersToPlay(settings);
        startTrial();
    }
    
    void RecognitionTestModel::preparePlayersToPlay(const AudioSettings &p) {
        setAudioDevices(p);
    }
    
    void RecognitionTestModel::setAudioDevices(const AudioSettings &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setAudioDevices_,
            p.audioDevice
        );
    }
    
    void RecognitionTestModel::throwInvalidAudioDeviceOnErrorSettingDevice(
        void (RecognitionTestModel::*f)(const std::string &),
        const std::string &device
    ) {
        try {
            (this->*f)(device);
        } catch (const InvalidAudioDevice &) {
            throw RequestFailure{
                "'" + device + "' is not a valid audio device."
            };
        }
    }
    
    void RecognitionTestModel::setAudioDevices_(const std::string &device) {
        maskerPlayer->setAudioDevice(device);
        setTargetPlayerDevice_(device);
    }
    
    void RecognitionTestModel::setTargetPlayerDevice_(const std::string &device) {
        targetPlayer->setAudioDevice(device);
    }
    
    void RecognitionTestModel::startTrial() {
        maskerPlayer->fadeIn();
    }
    
    void RecognitionTestModel::fadeInComplete() {
        playTarget();
    }
    
    void RecognitionTestModel::playTarget() {
        targetPlayer->play();
    }
    
    void RecognitionTestModel::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    void RecognitionTestModel::fadeOutComplete() {
        listener_->trialComplete();
    }
    
    void RecognitionTestModel::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        writeTrial(response);
        submitResponse_(response);
    }
    
    void RecognitionTestModel::writeTrial(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        testMethod->writeTrial(outputFile, response);
        outputFile->save();
    }
    
    void RecognitionTestModel::submitResponse_(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        testMethod->submitResponse(response);
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitCorrectResponse() {
        submitCorrectResponse_();
    }
    
    void RecognitionTestModel::submitCorrectResponse_() {
        testMethod->submitCorrectResponse();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitIncorrectResponse() {
        submitIncorrectResponse_();
    }
    
    void RecognitionTestModel::submitIncorrectResponse_() {
        testMethod->submitIncorrectResponse();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::submitResponse(const FreeResponse &p) {
        writeTrial(p);
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::writeTrial(const FreeResponse &p) {
        FreeResponseTrial trial;
        trial.response = p.response;
        trial.target = evaluator->fileName(testMethod->current());
        outputFile->writeTrial(trial);
        outputFile->save();
    }
    
    void RecognitionTestModel::playCalibration(const Calibration &p) {
        throwIfTrialInProgress();
        
        playCalibration_(p);
    }
    
    void RecognitionTestModel::playCalibration_(const Calibration &p) {
        setTargetPlayerDevice(p);
        loadTargetFile(p.filePath);
        trySettingTargetLevel(p);
        prepareVideo(p.condition);
        playTarget();
    }
    
    void RecognitionTestModel::setTargetPlayerDevice(const Calibration &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setTargetPlayerDevice_,
            p.audioSettings.audioDevice
        );
    }
    
    void RecognitionTestModel::trySettingTargetLevel(const Calibration &p) {
        try {
            setTargetLevel_dB(calibrationLevel_dB(p));
        } catch (const InvalidAudioFile &) {
            throw RequestFailure{"unable to read " + p.filePath};
        }
    }
    
    double RecognitionTestModel::calibrationLevel_dB(const Calibration &p) {
        return
            p.level_dB_SPL -
            p.fullScaleLevel_dB_SPL -
            unalteredTargetLevel_dB();
    }

    bool RecognitionTestModel::testComplete() {
        return testMethod->complete();
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return maskerPlayer->outputAudioDeviceDescriptions();
    }
}

