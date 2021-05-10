#include "AppKitTestSetupUIFactory.h"
#include "Foundation-utility.h"
#import "HelloWorldObjc.h"
#include "run.h"
#include "AppKit-utility.h"
#include "EyeTrackerStub.hpp"
#import <Foundation/Foundation.h>
#include <algorithm>
#include <string>
#include <sstream>

@interface TestSetupUIObserverImpl : NSObject <TestSetupUIObserver>
@end

@implementation TestSetupUIObserverImpl {
  @public
    av_speech_in_noise::TestSetupControl::Observer *observer;
}

- (void)notifyThatConfirmButtonHasBeenClicked {
    observer->notifyThatConfirmButtonHasBeenClicked();
}

- (void)notifyThatPlayCalibrationButtonHasBeenClicked {
    observer->notifyThatPlayCalibrationButtonHasBeenClicked();
}
@end

@interface TestUIObserverImpl : NSObject <TestUIObserver>
@end

@implementation TestUIObserverImpl {
  @public
    av_speech_in_noise::TestControl::Observer *observer;
}

- (void)playTrial {
    observer->playTrial();
}

- (void)exitTest {
    observer->exitTest();
}

- (void)declineContinuingTesting {
    observer->declineContinuingTesting();
}

- (void)acceptContinuingTesting {
    observer->acceptContinuingTesting();
}
@end

@interface FreeResponseUIObserverImpl : NSObject <FreeResponseUIObserver>
@end

@implementation FreeResponseUIObserverImpl {
  @public
    av_speech_in_noise::FreeResponseControl::Observer *observer;
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

namespace av_speech_in_noise {
class TestSetupUIImpl : public TestSetupUI {
  public:
    explicit TestSetupUIImpl(NSObject<TestSetupUI> *testSetupUI)
        : testSetupUI{testSetupUI} {}

    void show() override { return [testSetupUI show]; }

    void hide() override { return [testSetupUI hide]; }

    auto testerId() -> std::string override {
        return [testSetupUI testerId].UTF8String;
    }

    auto subjectId() -> std::string override {
        return [testSetupUI subjectId].UTF8String;
    }

    auto session() -> std::string override {
        return [testSetupUI session].UTF8String;
    }

    auto testSettingsFile() -> std::string override {
        return [testSetupUI testSettingsFile].UTF8String;
    }

    auto startingSnr() -> std::string override {
        return [testSetupUI startingSnr].UTF8String;
    }

    auto transducer() -> std::string override {
        return [testSetupUI transducer].UTF8String;
    }

    auto rmeSetting() -> std::string override {
        return [testSetupUI rmeSetting].UTF8String;
    }

    void populateTransducerMenu(std::vector<std::string> v) override {
        id nsstrings = [NSMutableArray new];
        for_each(v.begin(), v.end(), [&nsstrings](const std::string &str) {
            id nsstr = [NSString stringWithUTF8String:str.c_str()];
            [nsstrings addObject:nsstr];
        });
        [testSetupUI populateTransducerMenu:nsstrings];
    }

    void attach(Observer *a) override {
        const auto adapted{[[TestSetupUIObserverImpl alloc] init]};
        adapted->observer = a;
        [testSetupUI attach:adapted];
    }

  private:
    NSObject<TestSetupUI> *testSetupUI;
};

class TestSetupUIFactoryImpl : public AppKitTestSetupUIFactory {
  public:
    explicit TestSetupUIFactoryImpl(
        NSObject<TestSetupUIFactory> *testSetupUIFactory)
        : testSetupUIFactory{testSetupUIFactory} {}

    auto make(NSViewController *c) -> std::unique_ptr<TestSetupUI> override {
        return std::make_unique<TestSetupUIImpl>([testSetupUIFactory make:c]);
    }

  private:
    NSObject<TestSetupUIFactory> *testSetupUIFactory;
};

class TestUIImpl : public TestUI {
  public:
    explicit TestUIImpl(NSObject<TestUI> *testUI) : testUI{testUI} {}

    void attach(TestControl::Observer *a) override {
        const auto adapted{[[TestUIObserverImpl alloc] init]};
        adapted->observer = a;
        [testUI attach:adapted];
    }

    void showExitTestButton() override { [testUI showExitTestButton]; }

    void hideExitTestButton() override { [testUI hideExitTestButton]; }

    void show() override { [testUI show]; }

    void hide() override { [testUI hide]; }

    void display(std::string s) override { [testUI display:nsString(s)]; }

    void secondaryDisplay(std::string s) override {
        [testUI secondaryDisplay:nsString(s)];
    }

    void showNextTrialButton() override { [testUI showNextTrialButton]; }

    void hideNextTrialButton() override { [testUI hideNextTrialButton]; }

    void showContinueTestingDialog() override {
        [testUI showContinueTestingDialog];
    }

    void hideContinueTestingDialog() override {
        [testUI hideContinueTestingDialog];
    }

    void setContinueTestingDialogMessage(const std::string &s) override {
        [testUI setContinueTestingDialogMessage:nsString(s)];
    }

    void showSheet(std::string_view s) override {
        [testUI showSheet:nsString(std::string{s})];
    }

  private:
    NSObject<TestUI> *testUI;
};

class SessionUIImpl : public SessionUI {
  public:
    explicit SessionUIImpl(NSObject<SessionUI> *sessionUI)
        : sessionUI{sessionUI} {}

    void eventLoop() override { [sessionUI eventLoop]; }

    void showErrorMessage(std::string_view s) override {
        [sessionUI showErrorMessage:nsString(std::string{s})];
    }

    auto audioDevice() -> std::string override {
        return [sessionUI audioDevice].UTF8String;
    }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        id nsstrings = [NSMutableArray new];
        for_each(v.begin(), v.end(), [&nsstrings](const std::string &str) {
            id nsstr = [NSString stringWithUTF8String:str.c_str()];
            [nsstrings addObject:nsstr];
        });
        [sessionUI populateAudioDeviceMenu:nsstrings];
    }

  private:
    NSObject<SessionUI> *sessionUI;
};

class FreeResponseUIImpl : public FreeResponseUI {
  public:
    explicit FreeResponseUIImpl(NSObject<FreeResponseUI> *freeResponseUI)
        : freeResponseUI{freeResponseUI} {}

    void attach(Observer *a) override {
        const auto adapted{[[FreeResponseUIObserverImpl alloc] init]};
        adapted->observer = a;
        [freeResponseUI attach:adapted];
    }

    void showFreeResponseSubmission() override {
        [freeResponseUI showFreeResponseSubmission];
    }

    void hideFreeResponseSubmission() override {
        [freeResponseUI hideFreeResponseSubmission];
    }

    auto freeResponse() -> std::string override {
        return [freeResponseUI freeResponse].UTF8String;
    }

    auto flagged() -> bool override { return [freeResponseUI flagged] == YES; }

    void clearFreeResponse() override { [freeResponseUI clearFreeResponse]; }

    void clearFlag() override { [freeResponseUI clearFlag]; }

  private:
    NSObject<FreeResponseUI> *freeResponseUI;
};

static void main(NSObject<TestSetupUIFactory> *testSetupUIFactory,
    NSObject<SessionUI> *sessionUI, NSObject<TestUI> *testUI,
    NSObject<FreeResponseUI> *freeResponseUI) {
    static EyeTrackerStub eyeTracker;
    static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
    static DefaultOutputFileNameFactory outputFileNameFactory;
    const auto aboutViewController{nsTabViewControllerWithoutTabControl()};
    static SessionUIImpl sessionUIAdapted{sessionUI};
    static TestUIImpl testUIAdapted{testUI};
    static FreeResponseUIImpl freeResponseUIAdapted{freeResponseUI};
    initializeAppAndRunEventLoop(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, aboutViewController, nullptr,
        "Documents/AvSpeechInNoise Data", &sessionUIAdapted, &testUIAdapted,
        &freeResponseUIAdapted);
}
}

@implementation HelloWorldObjc
+ (void)doEverything:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
         withSessionUI:(NSObject<SessionUI> *)sessionUI
            withTestUI:(NSObject<TestUI> *)testUI
    withFreeResponseUI:(NSObject<FreeResponseUI> *)freeResponseUI {
    av_speech_in_noise::main(
        testSetupUIFactory, sessionUI, testUI, freeResponseUI);
}
@end
