#include "run.h"
#include "MersenneTwisterRandomizer.hpp"
#include "AvFoundationPlayers.h"
#include "AppKitView.h"
#include "Foundation-utility.h"
#include "AppKit-utility.h"
#include <presentation/SessionControllerImpl.hpp>
#include <presentation/TestSettingsInterpreter.hpp>
#include <recognition-test/Model.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/AdaptiveMethod.hpp>
#include <recognition-test/FixedLevelMethod.hpp>
#include <recognition-test/OutputFile.hpp>
#include <recognition-test/OutputFilePath.hpp>
#include <recognition-test/ResponseEvaluator.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderSimplified.hpp>
#include <target-playlists/RandomizedTargetPlaylists.hpp>
#include <target-playlists/FileFilterDecorator.hpp>
#include <adaptive-track/AdaptiveTrack.hpp>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <utility>
#include <functional>

@interface ResizesToContentsViewController : NSTabViewController
@end

@implementation ResizesToContentsViewController
- (instancetype)init {
    if ((self = [super init]) != nullptr) {
        [self setTabStyle:NSTabViewControllerTabStyleUnspecified];
    }
    return self;
}
- (void)viewWillAppear {
    [super viewWillAppear];
    self.preferredContentSize = self.view.fittingSize;
}
@end

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate
- (void)windowWillClose:(NSNotification *)__unused notification {
    [NSApp terminate:self];
}
@end

@interface MenuActions : NSObject
@end

@implementation MenuActions {
  @public
    NSWindow *preferencesWindow;
}

- (void)notifyThatPreferencesHasBeenClicked {
    [preferencesWindow makeKeyAndOrderFront:nil];
}
@end

@interface CallbackScheduler : NSObject
@end

namespace av_speech_in_noise {
namespace {
class TimerImpl : public Timer {
  public:
    TimerImpl();
    void attach(Observer *e) override;
    void scheduleCallbackAfterSeconds(double x) override;
    void timerCallback();

  private:
    Observer *listener{};
    CallbackScheduler *scheduler{[[CallbackScheduler alloc] init]};
};
}
}

@implementation CallbackScheduler {
  @public
    av_speech_in_noise::TimerImpl *controller;
}

- (void)scheduleCallbackAfterSeconds:(double)x {
    [NSTimer scheduledTimerWithTimeInterval:x
                                     target:self
                                   selector:@selector(timerCallback)
                                   userInfo:nil
                                    repeats:NO];
}

- (void)timerCallback {
    controller->timerCallback();
}
@end

namespace av_speech_in_noise {
namespace {
auto contents(NSString *parent) -> NSArray<NSString *> * {
    return [[NSFileManager defaultManager] contentsOfDirectoryAtPath:parent
                                                               error:nil];
}

auto collectContentsIf(const LocalUrl &directory,
    const std::function<bool(NSString *)> &predicate) -> std::vector<LocalUrl> {
    std::vector<LocalUrl> items{};
    const auto parent{nsString(directory.path).stringByExpandingTildeInPath};
    for (NSString *item in contents(parent)) {
        const auto path{[parent stringByAppendingPathComponent:item]};
        if (predicate(path))
            items.push_back({[item UTF8String]});
    }
    return items;
}

auto isDirectory(NSString *path) -> bool {
    BOOL isDir{NO};
    [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    return isDir == YES;
}

auto notADirectory(NSString *path) -> bool { return !isDirectory(path); }

class MacOsDirectoryReader : public DirectoryReader {
    auto filesIn(const LocalUrl &directory) -> std::vector<LocalUrl> override {
        return collectContentsIf(directory, notADirectory);
    }

    auto subDirectories(const LocalUrl &directory)
        -> std::vector<LocalUrl> override {
        return collectContentsIf(directory, isDirectory);
    }
};

class FileWriter : public Writer {
    std::ofstream file{};

  public:
    void write(const std::string &s) override { file << s; }

    void open(const std::string &s) override { file.open(s); }

    auto failed() -> bool override { return file.fail(); }

    void close() override { file.close(); }

    void save() override { file.flush(); }
};

class UnixFileSystemPath : public FileSystemPath {
    auto homeDirectory() -> std::string override { return std::getenv("HOME"); }

    void createDirectory(std::string s) override {
        mkdir(s.c_str(), ACCESSPERMS);
    }
};

class TimeStampImpl : public TimeStamp {
    tm dummyTime{};
    tm *time{&dummyTime};

  public:
    auto year() -> int override {
        // https://en.cppreference.com/w/c/chrono/tm
        return time->tm_year + 1900;
    }

    auto month() -> int override { return time->tm_mon; }

    auto dayOfMonth() -> int override { return time->tm_mday; }

    auto hour() -> int override { return time->tm_hour; }

    auto minute() -> int override { return time->tm_min; }

    auto second() -> int override { return time->tm_sec; }

    void capture() override {
        const auto now{std::time(nullptr)};
        time = std::localtime(&now);
        if (time == nullptr)
            time = &dummyTime;
    }
};

class TextFileReaderImpl : public TextFileReader {
  public:
    auto read(const LocalUrl &s) -> std::string override {
        std::ifstream file{s.path};
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }
};

TimerImpl::TimerImpl() { scheduler->controller = this; }

void TimerImpl::attach(Observer *e) { listener = e; }

void TimerImpl::scheduleCallbackAfterSeconds(double x) {
    [scheduler scheduleCallbackAfterSeconds:x];
}

void TimerImpl::timerCallback() { listener->callback(); }
}

static void addChild(NSTabViewController *parent, NSTabViewController *child) {
    [parent.view addSubview:child.view];
}

void initializeAppAndRunEventLoop(EyeTracker &eyeTracker,
    AppKitTestSetupUIFactory &testSetupViewFactory,
    OutputFileNameFactory &outputFileNameFactory,
    SessionController::Observer *sessionControllerObserver,
    const std::string &relativeOutputDirectory) {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    AvFoundationVideoPlayer videoPlayer{subjectScreen};
    AvFoundationBufferedAudioReaderFactory bufferedReaderFactory;
    AudioReaderSimplified audioReader{bufferedReaderFactory};
    TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    AvFoundationAudioPlayer audioPlayer;
    TimerImpl timer;
    MaskerPlayerImpl maskerPlayer{&audioPlayer, &audioReader, &timer};
    maskerPlayer.setRampFor(Duration{0.02});
    FileWriter fileWriter;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    const auto outputFileName{outputFileNameFactory.make(timeStamp)};
    OutputFilePathImpl outputFilePath{*outputFileName, systemPath};
    outputFilePath.setRelativeOutputDirectory(relativeOutputDirectory);
    OutputFileImpl outputFile{fileWriter, outputFilePath};
    adaptive_track::AdaptiveTrack::Factory snrTrackFactory;
    ResponseEvaluatorImpl responseEvaluator;
    TextFileReaderImpl textFileReader;
    MersenneTwisterRandomizer randomizer;
    AdaptiveMethodImpl adaptiveMethod{
        snrTrackFactory, responseEvaluator, randomizer};
    MacOsDirectoryReader directoryReader;
    FileExtensionFilter targetFileExtensionFilter{
        {".mov", ".avi", ".wav", ".mp4"}};
    FileFilterDecorator onlyIncludesTargetFileExtensions{
        &directoryReader, &targetFileExtensionFilter};
    RandomizedTargetPlaylistWithReplacement targetsWithReplacement{
        &onlyIncludesTargetFileExtensions, &randomizer};
    FileIdentifierExcluderFilter
        excludesTargetsThatHave100_200_300Or400InTheirName{
            {"100", "200", "300", "400"}};
    FileIdentifierFilter targetsThatHave100InTheirName{"100"};
    FileIdentifierFilter targetsThatHave200InTheirName{"200"};
    FileIdentifierFilter targetsThatHave300InTheirName{"300"};
    FileIdentifierFilter targetsThatHave400InTheirName{"400"};
    FileFilterDecorator allButSilentIntervalTargets{
        &onlyIncludesTargetFileExtensions,
        &excludesTargetsThatHave100_200_300Or400InTheirName};
    FileFilterDecorator oneHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave100InTheirName};
    FileFilterDecorator twoHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave200InTheirName};
    FileFilterDecorator threeHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave300InTheirName};
    FileFilterDecorator fourHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave400InTheirName};
    RandomSubsetFiles passesThirtyRandomFiles{&randomizer, 30};
    FileFilterDecorator thirtyRandomAllButSilentIntervalTargets{
        &allButSilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomOneHundred_ms_SilentIntervalTargets{
        &oneHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomTwoHundred_ms_SilentIntervalTargets{
        &twoHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomThreeHundred_ms_SilentIntervalTargets{
        &threeHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomFourHundred_ms_SilentIntervalTargets{
        &fourHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    DirectoryReaderComposite silentIntervalTargetsDirectoryReader{
        {&thirtyRandomAllButSilentIntervalTargets,
            &thirtyRandomOneHundred_ms_SilentIntervalTargets,
            &thirtyRandomTwoHundred_ms_SilentIntervalTargets,
            &thirtyRandomThreeHundred_ms_SilentIntervalTargets,
            &thirtyRandomFourHundred_ms_SilentIntervalTargets}};
    RandomizedTargetPlaylistWithoutReplacement silentIntervalTargets{
        &silentIntervalTargetsDirectoryReader, &randomizer};
    RandomizedTargetPlaylistWithoutReplacement everyTargetOnce{
        &onlyIncludesTargetFileExtensions, &randomizer};
    EachTargetPlayedOnceThenShuffleAndRepeat allTargetsNTimes{
        &onlyIncludesTargetFileExtensions, &randomizer};
    FixedLevelMethodImpl fixedLevelMethod{responseEvaluator};
    RecognitionTestModelImpl recognitionTestModel{targetPlayer, maskerPlayer,
        responseEvaluator, outputFile, randomizer, eyeTracker};
    RandomizedTargetPlaylistWithReplacement::Factory
        targetsWithReplacementFactory{
            &onlyIncludesTargetFileExtensions, &randomizer};
    SubdirectoryTargetPlaylistReader targetsWithReplacementReader{
        &targetsWithReplacementFactory, &directoryReader};
    CyclicRandomizedTargetPlaylist::Factory cyclicTargetsFactory{
        &onlyIncludesTargetFileExtensions, &randomizer};
    SubdirectoryTargetPlaylistReader cyclicTargetsReader{
        &cyclicTargetsFactory, &directoryReader};
    ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervalTargets, everyTargetOnce,
        allTargetsNTimes, recognitionTestModel, outputFile};
    const auto viewController{nsTabViewControllerWithoutTabControl()};
    const auto window{
        [NSWindow windowWithContentViewController:viewController]};
    [window makeKeyAndOrderFront:nil];
    const auto app{[NSApplication sharedApplication]};
    app.mainMenu = [[NSMenu alloc] init];
    auto appMenu{[[NSMenuItem alloc] init]};
    auto appSubMenu{[[NSMenu alloc] init]};
    auto preferencesMenuItem {
        [appSubMenu
            addItemWithTitle:@"Preferences..."
                      action:@selector(notifyThatPreferencesHasBeenClicked)
               keyEquivalent:@","]
    };
    auto menuActions{[[MenuActions alloc] init]};
    const auto preferencesViewController{
        [[ResizesToContentsViewController alloc] init]};
    const auto preferencesWindow{
        [NSWindow windowWithContentViewController:preferencesViewController]};
    preferencesWindow.styleMask =
        NSWindowStyleMaskClosable | NSWindowStyleMaskTitled;
    menuActions->preferencesWindow = preferencesWindow;
    preferencesMenuItem.target = menuActions;
    [appSubMenu addItemWithTitle:@"Quit"
                          action:@selector(stop:)
                   keyEquivalent:@"q"];
    [appMenu setSubmenu:appSubMenu];
    [app.mainMenu addItem:appMenu];
    AppKitView view{app, preferencesViewController};
    const auto testSetupViewController{nsTabViewControllerWithoutTabControl()};
    addChild(viewController, testSetupViewController);
    testSetupViewController.view.translatesAutoresizingMaskIntoConstraints = NO;
    [NSLayoutConstraint activateConstraints:@[
        [testSetupViewController.view.topAnchor
            constraintEqualToAnchor:viewController.view.topAnchor
                           constant:8],
        [testSetupViewController.view.bottomAnchor
            constraintEqualToAnchor:viewController.view.bottomAnchor
                           constant:-8],
        [testSetupViewController.view.leadingAnchor
            constraintEqualToAnchor:viewController.view.leadingAnchor
                           constant:8],
        [testSetupViewController.view.trailingAnchor
            constraintEqualToAnchor:viewController.view.trailingAnchor
                           constant:-8]
    ]];
    const auto testSetupView{
        testSetupViewFactory.make(testSetupViewController)};
    const auto experimenterViewController{
        nsTabViewControllerWithoutTabControl()};
    addChild(viewController, experimenterViewController);
    const auto chooseKeywordsUIController{
        nsTabViewControllerWithoutTabControl()};
    const auto freeResponseUIController{nsTabViewControllerWithoutTabControl()};
    const auto correctKeywordsUIController{
        nsTabViewControllerWithoutTabControl()};
    const auto passFailUIController{nsTabViewControllerWithoutTabControl()};
    addChild(experimenterViewController, chooseKeywordsUIController);
    addChild(experimenterViewController, freeResponseUIController);
    addChild(experimenterViewController, correctKeywordsUIController);
    addChild(experimenterViewController, passFailUIController);
    AppKitTestUI experimenterView{experimenterViewController};
    [window center];
    [window setDelegate:[[WindowDelegate alloc] init]];
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenOrigin{subjectScreenFrame.origin};
    const auto subjectScreenSize{subjectScreenFrame.size};
    const auto subjectViewHeight{subjectScreenSize.height / 4};
    const auto subjectScreenWidth{subjectScreenSize.width};
    const auto subjectViewWidth{subjectScreenWidth / 3};
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x + (subjectScreenWidth - subjectViewWidth) / 2;
    AppKitConsonantUI consonantView{
        NSMakeRect(subjectScreenOrigin.x + subjectScreenWidth / 4,
            subjectScreenOrigin.y + subjectScreenSize.height / 12,
            subjectScreenWidth / 2, subjectScreenSize.height / 2)};
    AppKitCoordinateResponseMeasureUI coordinateResponseMeasureView{
        NSMakeRect(subjectViewLeadingEdge, subjectScreenOrigin.y,
            subjectViewWidth, subjectViewHeight)};
    TestSettingsInterpreterImpl testSettingsInterpreter;
    ConsonantTaskPresenter consonantPresenter{consonantView};
    FreeResponseUI freeResponseUI{freeResponseUIController};
    FreeResponsePresenter freeResponsePresenter{
        experimenterView, freeResponseUI};
    ChooseKeywordsUI chooseKeywordsUI{chooseKeywordsUIController};
    ChooseKeywordsPresenterImpl chooseKeywordsPresenter{model, experimenterView,
        chooseKeywordsUI,
        sentencesWithThreeKeywords(
            TextFileReaderImpl{}.read(resourceUrl("mlst-c", "txt")))};
    CorrectKeywordsUI correctKeywordsUI{correctKeywordsUIController};
    CorrectKeywordsPresenter correctKeywordsPresenter{
        experimenterView, correctKeywordsUI};
    PassFailUI passFailUI{passFailUIController};
    PassFailPresenter passFailPresenter{experimenterView, passFailUI};
    CoordinateResponseMeasurePresenter coordinateResponseMeasurePresenter{
        coordinateResponseMeasureView};
    TestSetupControllerImpl testSetupController{model, view,
        *(testSetupView.get()), testSettingsInterpreter, textFileReader};
    TestSetupPresenterImpl testSetupPresenter{*(testSetupView.get())};
    TestControllerImpl testController{model, view, experimenterView};
    ChooseKeywordsController chooseKeywordsController{
        testController, model, chooseKeywordsUI, chooseKeywordsPresenter};
    CorrectKeywordsController correctKeywordsController{
        testController, model, view, correctKeywordsUI};
    FreeResponseController freeResponseController{
        testController, model, freeResponseUI};
    PassFailController passFailController{testController, model, passFailUI};
    ConsonantTaskController consonantTaskController{
        testController, model, consonantView};
    CoordinateResponseMeasureController coordinateResponseMeasureController{
        testController, model, coordinateResponseMeasureView};
    consonantTaskController.attach(&consonantPresenter);
    coordinateResponseMeasureController.attach(
        &coordinateResponseMeasurePresenter);
    UninitializedTaskPresenterImpl taskPresenter;
    TestPresenterImpl experimenterPresenter{model, experimenterView,
        {{Method::adaptiveCoordinateResponseMeasure,
             coordinateResponseMeasurePresenter},
            {Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker,
                coordinateResponseMeasurePresenter},
            {Method::adaptiveCoordinateResponseMeasureWithDelayedMasker,
                coordinateResponseMeasurePresenter},
            {Method::adaptiveCoordinateResponseMeasureWithEyeTracking,
                coordinateResponseMeasurePresenter},
            {Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
                coordinateResponseMeasurePresenter},
            {Method::
                    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
                coordinateResponseMeasurePresenter},
            {Method::
                    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
                coordinateResponseMeasurePresenter},
            {Method::fixedLevelFreeResponseWithAllTargets,
                freeResponsePresenter},
            {Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
                freeResponsePresenter},
            {Method::fixedLevelFreeResponseWithSilentIntervalTargets,
                freeResponsePresenter},
            {Method::fixedLevelFreeResponseWithTargetReplacement,
                freeResponsePresenter},
            {Method::fixedLevelChooseKeywordsWithAllTargets,
                chooseKeywordsPresenter},
            {Method::adaptiveCorrectKeywords, correctKeywordsPresenter},
            {Method::adaptiveCorrectKeywordsWithEyeTracking,
                correctKeywordsPresenter},
            {Method::fixedLevelConsonants, consonantPresenter},
            {Method::adaptivePassFail, passFailPresenter},
            {Method::adaptivePassFailWithEyeTracking, passFailPresenter},
            {Method::unknown, passFailPresenter}},
        &taskPresenter};
    SessionControllerImpl sessionController{
        model, view, testSetupPresenter, experimenterPresenter};
    sessionController.attach(sessionControllerObserver);
    testSetupController.attach(&sessionController);
    testSetupController.attach(&testSetupPresenter);
    testController.attach(&sessionController);
    testController.attach(&experimenterPresenter);
    [app run];
}
}
