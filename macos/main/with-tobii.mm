#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include "../objective-c-bridge.h"
#include "../objective-c-adapters.h"
#include <exception>
#include <tobii_research.h>
#include <tobii_research_streams.h>
#include <gsl/gsl>
#include <vector>
#import <AppKit/AppKit.h>

namespace av_speech_in_noise {
class TobiiEyeTracker : public EyeTracker {
  public:
    TobiiEyeTracker();
    ~TobiiEyeTracker() override;
    void allocateRecordingTimeSeconds(double s) override;
    void start() override;
    void stop() override;
    auto gazeSamples() -> BinocularGazeSamples override;
    auto currentSystemTime() -> EyeTrackerSystemTime override;

  private:
    static void gaze_data_callback(
        TobiiResearchGazeData *gaze_data, void *self);
    void gazeDataReceived(TobiiResearchGazeData *gaze_data);

    std::vector<TobiiResearchGazeData> gazeData{};
    TobiiResearchEyeTrackers *eyeTrackers{};
    std::size_t head{};
};

static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

TobiiEyeTracker::TobiiEyeTracker() {
    tobii_research_find_all_eyetrackers(&eyeTrackers);
}

TobiiEyeTracker::~TobiiEyeTracker() {
    tobii_research_free_eyetrackers(eyeTrackers);
}

void TobiiEyeTracker::allocateRecordingTimeSeconds(double seconds) {
    float gaze_output_frequency_Hz{};
    tobii_research_get_gaze_output_frequency(
        eyeTracker(eyeTrackers), &gaze_output_frequency_Hz);
    gazeData.resize(std::ceil(gaze_output_frequency_Hz * seconds) + 1);
    head = 0;
}

void TobiiEyeTracker::start() {
    tobii_research_subscribe_to_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback, this);
}

void TobiiEyeTracker::stop() {
    tobii_research_unsubscribe_from_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback);
}

void TobiiEyeTracker::gaze_data_callback(
    TobiiResearchGazeData *gaze_data, void *self) {
    static_cast<TobiiEyeTracker *>(self)->gazeDataReceived(gaze_data);
}

void TobiiEyeTracker::gazeDataReceived(TobiiResearchGazeData *gaze_data) {
    if (head < gazeData.size())
        gazeData.at(head++) = *gaze_data;
}

static auto at(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> BinocularGazeSample & {
    return b.at(i);
}

static auto at(const std::vector<TobiiResearchGazeData> &b, gsl::index i)
    -> const TobiiResearchGazeData & {
    return b.at(i);
}

static auto eyeGaze(const TobiiResearchEyeData &d)
    -> const TobiiResearchNormalizedPoint2D & {
    return d.gaze_point.position_on_display_area;
}

static auto leftEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).left_eye);
}

static auto rightEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).right_eye);
}

static auto x(const TobiiResearchNormalizedPoint2D &p) -> float { return p.x; }

static auto y(const TobiiResearchNormalizedPoint2D &p) -> float { return p.y; }

static auto leftEyeGaze(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> EyeGaze & {
    return at(b, i).left;
}

static auto rightEyeGaze(BinocularGazeSamples &b, gsl::index i) -> EyeGaze & {
    return at(b, i).right;
}

static auto x(EyeGaze &p) -> float & { return p.x; }

static auto y(EyeGaze &p) -> float & { return p.y; }

static auto size(const std::vector<BinocularGazeSample> &v) -> gsl::index {
    return v.size();
}

auto TobiiEyeTracker::gazeSamples() -> BinocularGazeSamples {
    BinocularGazeSamples gazeSamples_(head > 0 ? head - 1 : 0);
    for (gsl::index i{0}; i < size(gazeSamples_); ++i) {
        at(gazeSamples_, i).systemTime.microseconds =
            at(gazeData, i).system_time_stamp;
        x(leftEyeGaze(gazeSamples_, i)) = x(leftEyeGaze(gazeData, i));
        y(leftEyeGaze(gazeSamples_, i)) = y(leftEyeGaze(gazeData, i));
        x(rightEyeGaze(gazeSamples_, i)) = x(rightEyeGaze(gazeData, i));
        y(rightEyeGaze(gazeSamples_, i)) = y(rightEyeGaze(gazeData, i));
    }
    return gazeSamples_;
}

auto TobiiEyeTracker::currentSystemTime() -> EyeTrackerSystemTime {
    EyeTrackerSystemTime currentSystemTime{};
    int64_t microseconds = 0;
    tobii_research_get_system_time_stamp(&microseconds);
    currentSystemTime.microseconds = microseconds;
    return currentSystemTime;
}

void main(NSObject<TestSetupUIFactory> *testSetupUIFactory,
    NSObject<SessionUI> *sessionUI, NSObject<TestUI> *testUI,
    NSObject<FreeResponseUI> *freeResponseUI,
    NSObject<SyllablesUI> *syllablesUI,
    NSObject<ChooseKeywordsUI> *chooseKeywordsUI,
    NSObject<CorrectKeywordsUI> *correctKeywordsUI,
    NSObject<PassFailUI> *passFailUI) {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenOrigin{subjectScreenFrame.origin};
    const auto subjectScreenSize{subjectScreenFrame.size};
    const auto subjectViewHeight{subjectScreenSize.height / 4};
    const auto subjectScreenWidth{subjectScreenSize.width};
    const auto subjectViewWidth{subjectScreenWidth / 3};
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x + (subjectScreenWidth - subjectViewWidth) / 2;
    const auto alertWindow{[[NSWindow alloc]
        initWithContentRect:NSMakeRect(
                                subjectScreenOrigin.x + subjectScreenWidth / 4,
                                subjectScreenOrigin.y +
                                    subjectScreenSize.height / 12,
                                subjectScreenWidth / 2,
                                subjectScreenSize.height / 2)
                  styleMask:NSWindowStyleMaskBorderless
                    backing:NSBackingStoreBuffered
                      defer:YES]};
    const auto alert{[[NSAlert alloc] init]};
    [alert setMessageText:@""];
    [alert
        setInformativeText:
            @"This software will store your eye tracking data.\n\nWe do so "
            @"only for the purpose of the current study (17-13-XP). We never "
            @"sell, distribute, or make profit on the collected data. Only "
            @"staff and research personnel on the existing IRB will have "
            @"access to the data. Any data used for publication or "
            @"collaborative and/or learning purposes will be "
            @"deidentified.\n\nThere is no direct benefit to you for doing "
            @"this study. What we learn from this study may help doctors treat "
            @"children who have a hard time hearing when it is noisy."];
    [alert addButtonWithTitle:@"No, I do not accept"];
    [alert addButtonWithTitle:@"Yes, I accept"];
    [alertWindow makeKeyAndOrderFront:nil];
    [alert beginSheetModalForWindow:alertWindow
                  completionHandler:^(NSModalResponse returnCode) {
                    [alertWindow orderOut:nil];
                    [NSApp stopModalWithCode:returnCode];
                  }];
    if ([NSApp runModalForWindow:alertWindow] == NSAlertFirstButtonReturn) {
        const auto terminatingAlert{[[NSAlert alloc] init]};
        [terminatingAlert setMessageText:@""];
        [terminatingAlert setInformativeText:@"User does not accept eye "
                                             @"tracking terms. Terminating."];
        [terminatingAlert runModal];
    } else {

        static TobiiEyeTracker eyeTracker;
        static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
        static DefaultOutputFileNameFactory outputFileNameFactory;
        static SessionUIImpl sessionUIAdapted{sessionUI};
        static TestUIImpl testUIAdapted{testUI};
        static FreeResponseUIImpl freeResponseUIAdapted{freeResponseUI};
        static SyllablesUIImpl syllablesUIAdapted{syllablesUI};
        static ChooseKeywordsUIImpl chooseKeywordsUIAdapted{chooseKeywordsUI};
        static CorrectKeywordsUIImpl correctKeywordsUIAdapted{
            correctKeywordsUI};
        static PassFailUIImpl passFailUIAdapted{passFailUI};
        initializeAppAndRunEventLoop(eyeTracker, outputFileNameFactory,
            testSetupViewFactory, sessionUIAdapted, testUIAdapted,
            freeResponseUIAdapted, syllablesUIAdapted, chooseKeywordsUIAdapted,
            correctKeywordsUIAdapted, passFailUIAdapted);
    }
}
}

@implementation AvSpeechInNoiseMain
+ (void)withTobiiPro:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
            withSessionUI:(NSObject<SessionUI> *)sessionUI
               withTestUI:(NSObject<TestUI> *)testUI
       withFreeResponseUI:(NSObject<FreeResponseUI> *)freeResponseUI
          withSyllablesUI:(NSObject<SyllablesUI> *)syllablesUI
     withChooseKeywordsUI:(NSObject<ChooseKeywordsUI> *)chooseKeywordsUI
    withCorrectKeywordsUI:(NSObject<CorrectKeywordsUI> *)correctKeywordsUI
           withPassFailUI:(NSObject<PassFailUI> *)passFailUI {
    av_speech_in_noise::main(testSetupUIFactory, sessionUI, testUI,
        freeResponseUI, syllablesUI, chooseKeywordsUI, correctKeywordsUI,
        passFailUI);
}
@end
