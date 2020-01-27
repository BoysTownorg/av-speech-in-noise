#include "CocoaView.h"
#include "common-objc.h"
#include <iterator>

@interface SetupViewActions : NSObject
@property av_speech_in_noise::CocoaTestSetupView *controller;
- (void) confirmTestSetup;
- (void) browseForTargetList;
- (void) browseForMasker;
- (void) browseForCalibration;
- (void) browseForTrackSettings;
- (void) playCalibration;
@end

@interface SubjectViewActions : NSObject
@property av_speech_in_noise::CocoaSubjectView *controller;
- (void) respond:(id)sender;
- (void) playTrial;
@end

@interface ExperimenterViewActions : NSObject
@property av_speech_in_noise::CocoaExperimenterView *controller;
- (void) exitTest;
@end

@interface TestingViewActions : NSObject
@property av_speech_in_noise::CocoaTestingView *controller;
- (void) playTrial;
- (void) submitResponse;
- (void) submitPassedTrial;
- (void) submitFailedTrial;
@end

@implementation SetupViewActions
@synthesize controller;

- (void)confirmTestSetup {
    controller->confirm();
}

- (void)browseForTargetList { 
    controller->browseForTargetList();
}

- (void)browseForMasker { 
    controller->browseForMasker();
}

- (void)browseForCalibration {
    controller->browseForCalibration();
}

- (void)browseForTrackSettings {
    controller->browseForTrackSettings();
}

- (void)playCalibration { 
    controller->playCalibration();
}
@end

@implementation SubjectViewActions
@synthesize controller;

- (void)respond:(id)sender {
    controller->respond(sender);
}

- (void)playTrial { 
    controller->playTrial();
}
@end

@implementation ExperimenterViewActions
@synthesize controller;

- (void)exitTest {
    controller->exitTest();
}
@end

@implementation TestingViewActions
@synthesize controller;

- (void)playTrial {
    controller->playTrial();
}

- (void)submitResponse { 
    controller->submitResponse();
}

- (void)submitPassedTrial {
    controller->submitPassedTrial();
}

- (void)submitFailedTrial {
    controller->submitFailedTrial();
}
@end

namespace av_speech_in_noise {
static NSTextField *allocLabel(NSString *label, NSRect frame) {
    const auto text = [[NSTextField alloc] initWithFrame:frame];
    [text setStringValue:label];
    [text setBezeled:NO];
    [text setDrawsBackground:NO];
    [text setEditable:NO];
    [text setSelectable:NO];
    [text setAlignment:NSTextAlignmentRight];
    return text;
}

static constexpr auto labelHeight = 22;
static constexpr auto labelWidth = 140;
static constexpr auto labelToTextFieldSpacing = 5;
static constexpr auto textFieldLeadingEdge =
    labelWidth + labelToTextFieldSpacing;
static constexpr auto normalTextFieldWidth = 150;
static constexpr auto menuWidth = 180;
static constexpr auto filePathTextFieldWidth = 500;
static constexpr auto buttonHeight = 25;
static constexpr auto buttonWidth = 100;

static NSRect normalTextFieldSizeAtHeight(CGFloat y) {
    return NSMakeRect(
        textFieldLeadingEdge,
        y,
        normalTextFieldWidth,
        labelHeight
    );
}

static NSRect filePathTextFieldSizeAtHeight(CGFloat y) {
    return NSMakeRect(
        textFieldLeadingEdge,
        y,
        filePathTextFieldWidth,
        labelHeight
    );
}

CocoaTestSetupView::CocoaTestSetupView(NSRect r) :
    view_{[[NSView alloc] initWithFrame:r]},
    subjectIdLabel{allocLabel(
        @"subject:",
        NSMakeRect(0, 330, labelWidth, labelHeight)
    )},
    subjectId_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(330)
    ]},
    testerIdLabel{allocLabel(
        @"tester:",
        NSMakeRect(0, 300, labelWidth, labelHeight)
    )},
    testerId_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(300)
    ]},
    sessionLabel{allocLabel(
        @"session:",
        NSMakeRect(0, 270, labelWidth, labelHeight)
    )},
    session_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(270)
    ]},
    maskerLevel_dB_SPL_label{allocLabel(
        @"masker level (dB SPL):",
        NSMakeRect(0, 240, labelWidth, labelHeight)
    )},
    maskerLevel_dB_SPL_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(240)
    ]},
    calibrationLevel_dB_SPL_label{allocLabel(
        @"calibration level (dB SPL):",
        NSMakeRect(
            filePathTextFieldWidth + textFieldLeadingEdge + 10,
            60,
            150,
            labelHeight
        )
    )},
    calibrationLevel_dB_SPL_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(
            filePathTextFieldWidth + textFieldLeadingEdge + 150 + 10,
            60,
            80,
            labelHeight
        )
    ]},
    startingSnr_dB_label{allocLabel(
        @"starting SNR (dB):",
        NSMakeRect(0, 210, labelWidth, labelHeight)
    )},
    startingSnr_dB_{[[NSTextField alloc]
        initWithFrame:normalTextFieldSizeAtHeight(210)
    ]},
    targetListDirectoryLabel{allocLabel(
        @"targets:",
        NSMakeRect(0, 180, labelWidth, labelHeight)
    )},
    targetListDirectory_{[[NSTextField alloc]
        initWithFrame:filePathTextFieldSizeAtHeight(180)
    ]},
    maskerFilePath_label{allocLabel(
        @"masker:",
        NSMakeRect(0, 150, labelWidth, labelHeight)
    )},
    maskerFilePath_{[[NSTextField alloc]
        initWithFrame:filePathTextFieldSizeAtHeight(150)
    ]},
    calibrationFilePath_label{allocLabel(
        @"calibration:",
        NSMakeRect(0, 120, labelWidth, labelHeight)
    )},
    calibrationFilePath_{[[NSTextField alloc]
        initWithFrame:filePathTextFieldSizeAtHeight(120)
    ]},
    trackSettingsFile_label{allocLabel(
        @"track settings:",
        NSMakeRect(0, 90, labelWidth, labelHeight)
    )},
    trackSettingsFile_{[[NSTextField alloc]
        initWithFrame:filePathTextFieldSizeAtHeight(90)
    ]},
    condition_label{allocLabel(
        @"condition:",
        NSMakeRect(0, 60, labelWidth, labelHeight)
    )},
    conditionMenu{[[NSPopUpButton alloc]
        initWithFrame:NSMakeRect(
            textFieldLeadingEdge,
            60,
            menuWidth,
            labelHeight
        )
        pullsDown:NO
    ]},
    method_label{allocLabel(
        @"method:",
        NSMakeRect(0, 30, labelWidth, labelHeight)
    )},
    methodMenu{[[NSPopUpButton alloc]
        initWithFrame:NSMakeRect(
            textFieldLeadingEdge,
            30,
            menuWidth,
            labelHeight
        )
        pullsDown:NO
    ]},
    actions{[SetupViewActions alloc]}
{
    actions.controller = this;
    const auto browseForStimulusListButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForTargetList)
    ];
    [browseForStimulusListButton setFrame:NSMakeRect(
        filePathTextFieldWidth + textFieldLeadingEdge + 10,
        180,
        buttonWidth,
        buttonHeight
    )];
    const auto browseForMaskerButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForMasker)
    ];
    [browseForMaskerButton setFrame:NSMakeRect(
        filePathTextFieldWidth + textFieldLeadingEdge + 10,
        150,
        buttonWidth,
        buttonHeight
    )];
    const auto browseForCalibrationButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForCalibration)
    ];
    [browseForCalibrationButton setFrame:NSMakeRect(
        filePathTextFieldWidth + textFieldLeadingEdge + 10,
        120,
        buttonWidth,
        buttonHeight
    )];
    const auto browseForTrackSettingsButton = [NSButton
        buttonWithTitle:@"browse"
        target:actions
        action:@selector(browseForTrackSettings)
    ];
    [browseForTrackSettingsButton setFrame:NSMakeRect(
        filePathTextFieldWidth + textFieldLeadingEdge + 10,
        90,
        buttonWidth,
        buttonHeight
    )];
    const auto confirmButton = [NSButton
        buttonWithTitle:@"Confirm"
        target:actions
        action:@selector(confirmTestSetup)
    ];
    [confirmButton setFrame:NSMakeRect(
        r.size.width - buttonWidth,
        0,
        buttonWidth,
        buttonHeight
    )];
    const auto playCalibrationButton = [NSButton
        buttonWithTitle:@"play"
        target:actions
        action:@selector(playCalibration)
    ];
    [playCalibrationButton setFrame:NSMakeRect(
        filePathTextFieldWidth + textFieldLeadingEdge + buttonWidth + 10,
        120,
        buttonWidth,
        buttonHeight
    )];
    addSubview(browseForMaskerButton);
    addSubview(browseForStimulusListButton);
    addSubview(browseForCalibrationButton);
    addSubview(browseForTrackSettingsButton);
    addSubview(confirmButton);
    addSubview(playCalibrationButton);
    addSubview(subjectIdLabel);
    addSubview(subjectId_);
    addSubview(testerIdLabel);
    addSubview(testerId_);
    addSubview(sessionLabel);
    addSubview(session_);
    addSubview(maskerLevel_dB_SPL_label);
    addSubview(maskerLevel_dB_SPL_);
    addSubview(calibrationLevel_dB_SPL_label);
    addSubview(calibrationLevel_dB_SPL_);
    addSubview(startingSnr_dB_label);
    addSubview(startingSnr_dB_);
    addSubview(targetListDirectoryLabel);
    addSubview(targetListDirectory_);
    addSubview(maskerFilePath_label);
    addSubview(maskerFilePath_);
    addSubview(trackSettingsFile_label);
    addSubview(trackSettingsFile_);
    addSubview(calibrationFilePath_label);
    addSubview(calibrationFilePath_);
    addSubview(condition_label);
    addSubview(conditionMenu);
    addSubview(method_label);
    addSubview(methodMenu);
    [view_ setHidden:NO];
}

void CocoaTestSetupView::addSubview(NSView *subview) {
    [view_ addSubview:subview];
}

NSView *CocoaTestSetupView::view() {
    return view_;
}

void CocoaTestSetupView::show() {
    [view_ setHidden:NO];
}

void CocoaTestSetupView::hide() {
    [view_ setHidden:YES];
}

std::string CocoaTestSetupView::startingSnr_dB() {
    return stringValue(startingSnr_dB_);
}

const char *CocoaTestSetupView::stringValue(NSTextField *field) {
    return field.stringValue.UTF8String;
}

std::string CocoaTestSetupView::maskerLevel_dB_SPL() {
    return stringValue(maskerLevel_dB_SPL_);
}

std::string CocoaTestSetupView::calibrationLevel_dB_SPL() {
    return stringValue(calibrationLevel_dB_SPL_);
}

std::string CocoaTestSetupView::maskerFilePath() {
    return stringValue(maskerFilePath_);
}

std::string CocoaTestSetupView::calibrationFilePath() {
    return stringValue(calibrationFilePath_);
}

std::string CocoaTestSetupView::targetListDirectory() {
    return stringValue(targetListDirectory_);
}

std::string CocoaTestSetupView::testerId() {
    return stringValue(testerId_);
}

std::string CocoaTestSetupView::subjectId() {
    return stringValue(subjectId_);
}

std::string CocoaTestSetupView::session() {
    return stringValue(session_);
}

std::string CocoaTestSetupView::method() {
    return methodMenu.titleOfSelectedItem.UTF8String;
}

std::string CocoaTestSetupView::condition() {
    return conditionMenu.titleOfSelectedItem.UTF8String;
}

std::string CocoaTestSetupView::trackSettingsFile() {
    return stringValue(trackSettingsFile_);
}

void CocoaTestSetupView::setTrackSettingsFile(std::string s) {
    [trackSettingsFile_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setTargetListDirectory(std::string s) {
    [targetListDirectory_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setCalibrationFilePath(std::string s) {
    [calibrationFilePath_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setMasker(std::string s) {
    [maskerFilePath_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setCalibration(std::string s) {
    [calibrationFilePath_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setMaskerLevel_dB_SPL(std::string s) {
    [maskerLevel_dB_SPL_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setCalibrationLevel_dB_SPL(std::string s) {
    [calibrationLevel_dB_SPL_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::setStartingSnr_dB(std::string s) {
    [startingSnr_dB_ setStringValue:asNsString(std::move(s))];
}

void CocoaTestSetupView::populateConditionMenu(std::vector<std::string> items) {
    for (auto item : items)
        [conditionMenu addItemWithTitle:asNsString(item)];
}

void CocoaTestSetupView::populateMethodMenu(std::vector<std::string> items) {
    for (auto item : items)
        [methodMenu addItemWithTitle:asNsString(item)];
}

void CocoaTestSetupView::confirm() {
    listener_->confirmTestSetup();
}

void CocoaTestSetupView::subscribe(EventListener *listener) {
    listener_ = listener;
}

void CocoaTestSetupView::browseForMasker() {
    listener_->browseForMasker();
}

void CocoaTestSetupView::browseForTargetList() {
    listener_->browseForTargetList();
}

void CocoaTestSetupView::browseForCalibration() {
    listener_->browseForCalibration();
}

void CocoaTestSetupView::browseForTrackSettings() {
    listener_->browseForTrackSettingsFile();
}

void CocoaTestSetupView::playCalibration() {
    listener_->playCalibration();
}

static auto greenColor = NSColor.greenColor;
static auto redColor = NSColor.redColor;
static auto blueColor = NSColor.blueColor;
static auto whiteColor = NSColor.whiteColor;
static constexpr int numbers[] = {1, 2, 3, 4, 5, 6, 8, 9};
static constexpr auto responseNumbers = std::size(numbers);
static constexpr auto responseColors = 4;

CocoaSubjectView::CocoaSubjectView(NSRect r) :
    // Defer may be critical here...
    window{[[NSWindow alloc]
        initWithContentRect:r
        styleMask:NSWindowStyleMaskBorderless
        backing:NSBackingStoreBuffered
        defer:YES
    ]},
    responseButtons{[[NSView alloc]
        initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height)
    ]},
    nextTrialButton{[[NSView alloc]
        initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height)
    ]},
    actions{[SubjectViewActions alloc]}
{
    actions.controller = this;
    addButtonRow(blueColor, 0);
    addButtonRow(greenColor, 1);
    addButtonRow(whiteColor, 2);
    addButtonRow(redColor, 3);
    addNextTrialButton();
    [window.contentView addSubview:nextTrialButton];
    [window.contentView addSubview:responseButtons];
    hideResponseButtons();
    hideNextTrialButton();
}

void CocoaSubjectView::addButtonRow(NSColor *color, int row) {
    for (std::size_t col = 0; col < responseNumbers; ++col)
        addNumberButton(color, numbers[col], row, col);
}

void CocoaSubjectView::addNumberButton(NSColor *color, int number, int row, std::size_t col) {
    auto title = asNsString(std::to_string(number));
    const auto button = [NSButton
        buttonWithTitle:title
        target:actions
        action:@selector(respond:)
    ];
    auto responseWidth = responseButtons.frame.size.width/responseNumbers;
    auto responseHeight = responseButtons.frame.size.height/responseColors;
    [button setFrame:NSMakeRect(
        responseWidth*col,
        responseHeight*row,
        responseWidth,
        responseHeight
    )];
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    auto style = [[NSMutableParagraphStyle alloc] init];
    [style setAlignment:NSTextAlignmentCenter];
    auto attrsDictionary = [NSDictionary
        dictionaryWithObjectsAndKeys:
        color, NSForegroundColorAttributeName,
        [NSNumber numberWithFloat: -4.0], NSStrokeWidthAttributeName,
        NSColor.blackColor, NSStrokeColorAttributeName,
        style, NSParagraphStyleAttributeName,
        [NSFont fontWithName:@"Arial-Black" size:48], NSFontAttributeName,
        nil
    ];
    auto attrString = [[NSAttributedString alloc]
        initWithString:title
        attributes:attrsDictionary
    ];
    [button setAttributedTitle:attrString];
    [responseButtons addSubview:button];
}

void CocoaSubjectView::addNextTrialButton() {
    const auto button = [NSButton
        buttonWithTitle:@""
        target:actions
        action:@selector(playTrial)
    ];
    [button setBezelStyle:NSBezelStyleTexturedSquare];
    auto style = [[NSMutableParagraphStyle alloc] init];
    [style setAlignment:NSTextAlignmentCenter];
    auto font = [NSFont fontWithName:@"Courier" size:36];
    auto attrsDictionary = [NSDictionary
        dictionaryWithObjectsAndKeys:
        font, NSFontAttributeName,
        nil
    ];
    auto attrString = [[NSAttributedString alloc]
        initWithString:@"Press when ready"
        attributes:attrsDictionary
    ];
    [button setAttributedTitle:attrString];
    [button setFrame:NSMakeRect(
        0,
        0,
        nextTrialButton.frame.size.width,
        nextTrialButton.frame.size.height
    )];
    [nextTrialButton addSubview:button];
}

std::string CocoaSubjectView::numberResponse() {
    return lastButtonPressed.title.UTF8String;
}

bool CocoaSubjectView::greenResponse() {
    return lastPressedColor() == greenColor;
}

NSColor *CocoaSubjectView::lastPressedColor() {
    return [lastButtonPressed.attributedTitle
        attribute:NSForegroundColorAttributeName
        atIndex:0
        effectiveRange:nil
    ];
}

bool CocoaSubjectView::blueResponse() {
    return lastPressedColor() == blueColor;
}

bool CocoaSubjectView::whiteResponse() {
    return lastPressedColor() == whiteColor;
}

void CocoaSubjectView::respond(id sender) {
    lastButtonPressed = sender;
    listener_->submitResponse();
}

void CocoaSubjectView::showResponseButtons() {
    [responseButtons setHidden:NO];
}

void CocoaSubjectView::showNextTrialButton() {
    [nextTrialButton setHidden:NO];
}

void CocoaSubjectView::hideNextTrialButton() {
    [nextTrialButton setHidden:YES];
}

void CocoaSubjectView::playTrial() {
    listener_->playTrial();
}

void CocoaSubjectView::hideResponseButtons() {
    [responseButtons setHidden:YES];
}

void CocoaSubjectView::subscribe(EventListener *e) {
    listener_ = e;
}

void CocoaSubjectView::show() {
    [window makeKeyAndOrderFront:nil];
}

void CocoaSubjectView::hide() {
    [window orderOut:nil];
}


CocoaExperimenterView::CocoaExperimenterView(NSRect r) :
    view_{[[NSView alloc] initWithFrame:r]},
    displayedText_{[[NSTextView alloc] initWithFrame:NSMakeRect(
        r.origin.x + 15,
        r.origin.y + 15,
        r.size.width - 30,
        30
    )]},
    actions{[ExperimenterViewActions alloc]}
{
    exitTestButton_ = [NSButton
        buttonWithTitle:@"exit test"
        target:actions
        action:@selector(exitTest)
    ];
    [exitTestButton_ setFrame:NSMakeRect(
        0,
        r.size.height -  buttonHeight,
        buttonWidth,
        buttonHeight
    )];
    [displayedText_ setFont:[NSFont userFontOfSize:22.0]];
    [view_ addSubview:exitTestButton_];
    [view_ addSubview:displayedText_];
    [view_ setHidden:YES];
    actions.controller = this;
}

void CocoaExperimenterView::subscribe(EventListener *e) {
    listener_ = e;
}

void CocoaExperimenterView::showExitTestButton() {
    [exitTestButton_ setHidden:NO];
}

void CocoaExperimenterView::hideExitTestButton() {
    [exitTestButton_ setHidden:YES];
}

void CocoaExperimenterView::show() {
    [view_ setHidden:NO];
}

void CocoaExperimenterView::hide() {
    [view_ setHidden:YES];
}

NSView *CocoaExperimenterView::view() { 
    return view_;
}

void CocoaExperimenterView::exitTest() {
    listener_->exitTest();
}

void CocoaExperimenterView::display(std::string s) {
    [displayedText_ setString:asNsString(std::move(s))];
}


CocoaTestingView::CocoaTestingView(NSRect r) :
    view_{[[NSView alloc] initWithFrame:r]},
    nextTrialButton{[[NSView alloc]
        initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height - buttonHeight)
    ]},
    evaluationButtons{[[NSView alloc]
        initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height - buttonHeight)
    ]},
    responseSubmission{[[NSView alloc]
        initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height - buttonHeight)
    ]},
    response_{[[NSTextField alloc]
        initWithFrame:NSMakeRect(r.size.width/10, r.size.height/2, 150, labelHeight)
    ]},
    flagged_{[[NSButton alloc] initWithFrame:NSMakeRect(r.size.width/10, r.size.height/2 - labelHeight, 150, labelHeight)]},
    actions{[TestingViewActions alloc]}
{
    [flagged_ setButtonType:NSButtonTypeSwitch];
    [flagged_ setTitle:@"flagged"];
    const auto nextTrialButton_ = [NSButton
        buttonWithTitle:@"play trial"
        target:actions
        action:@selector(playTrial)
    ];
    [nextTrialButton_ setFrame:NSMakeRect(
        r.size.width - buttonWidth,
        0,
        buttonWidth,
        buttonHeight
    )];
    const auto submitResponse_ = [NSButton
        buttonWithTitle:@"submit"
        target:actions
        action:@selector(submitResponse)
    ];
    [submitResponse_ setFrame:NSMakeRect(
        r.size.width - buttonWidth,
        0,
        buttonWidth,
        buttonHeight
    )];
    const auto passButton_ = [NSButton
        buttonWithTitle:@"pass"
        target:actions
        action:@selector(submitPassedTrial)
    ];
    [passButton_ setFrame:NSMakeRect(
        r.size.width - 3 * buttonWidth,
        0,
        buttonWidth,
        buttonHeight
    )];
    const auto failButton_ = [NSButton
        buttonWithTitle:@"fail"
        target:actions
        action:@selector(submitFailedTrial)
    ];
    [failButton_ setFrame:NSMakeRect(
        r.size.width - buttonWidth,
        0,
        buttonWidth,
        buttonHeight
    )];
    [nextTrialButton addSubview:nextTrialButton_];
    [responseSubmission addSubview:submitResponse_];
    [responseSubmission addSubview:response_];
    [responseSubmission addSubview:flagged_];
    [evaluationButtons addSubview:passButton_];
    [evaluationButtons addSubview:failButton_];
    [view_ addSubview:nextTrialButton];
    [view_ addSubview:responseSubmission];
    [view_ addSubview:evaluationButtons];
    [evaluationButtons setHidden:YES];
    [nextTrialButton setHidden:YES];
    [responseSubmission setHidden:YES];
    [view_ setHidden:YES];
    actions.controller = this;
}

void CocoaTestingView::subscribe(EventListener *e) {
    listener_ = e;
}

void CocoaTestingView::showNextTrialButton() {
    [nextTrialButton setHidden:NO];
}

void CocoaTestingView::hideNextTrialButton() {
    [nextTrialButton setHidden:YES];
}

void CocoaTestingView::show() {
    [view_ setHidden:NO];
}

void CocoaTestingView::hide() {
    [view_ setHidden:YES];
}

void CocoaTestingView::showEvaluationButtons() {
    [evaluationButtons setHidden:NO];
}

void CocoaTestingView::showResponseSubmission() {
    [responseSubmission setHidden:NO];
}

void CocoaTestingView::hideResponseSubmission() {
    [responseSubmission setHidden:YES];
}

void CocoaTestingView::hideEvaluationButtons() {
    [evaluationButtons setHidden:YES];
}

std::string CocoaTestingView::response() {
    return response_.stringValue.UTF8String;
}

bool CocoaTestingView::flagged() {
    return flagged_.state == NSControlStateValueOn;
}

NSView *CocoaTestingView::view() { 
    return view_;
}

void CocoaTestingView::playTrial() {
    listener_->playTrial();
}

void CocoaTestingView::submitResponse() {
    listener_->submitResponse();
}

void CocoaTestingView::submitPassedTrial() {
    listener_->submitPassedTrial();
}

void CocoaTestingView::submitFailedTrial() {
    listener_->submitFailedTrial();
}

CocoaView::CocoaView(NSRect r) :
    app{[NSApplication sharedApplication]},
    window{[[NSWindow alloc]
        initWithContentRect:r
        styleMask:
            NSWindowStyleMaskClosable |
            NSWindowStyleMaskResizable |
            NSWindowStyleMaskTitled
        backing:NSBackingStoreBuffered
        defer:NO
    ]},
    audioDevice_label{allocLabel(
        @"audio output:",
        NSMakeRect(15, 15, labelWidth, labelHeight)
    )},
    deviceMenu{[[NSPopUpButton alloc]
        initWithFrame:NSMakeRect(160, 15, 140, 30)
        pullsDown:NO
    ]}
{
    app.mainMenu = [[NSMenu alloc] init];
    
    auto appMenu = [[NSMenuItem alloc] init];
    auto appSubMenu = [[NSMenu alloc] init];
    [appSubMenu addItemWithTitle:
        @"Quit"
        action:@selector(stop:)
        keyEquivalent:@"q"
    ];
    [appMenu setSubmenu:appSubMenu];
    [app.mainMenu addItem:appMenu];
    
    [window.contentView addSubview:audioDevice_label];
    [window.contentView addSubview:deviceMenu];
    [window makeKeyAndOrderFront:nil];
}

void CocoaView::eventLoop() {
    [app run];
}

void CocoaView::showErrorMessage(std::string s) {
    auto alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Error."];
    [alert setInformativeText:asNsString(std::move(s))];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

std::string CocoaView::browseForDirectory() {
    auto panel = [NSOpenPanel openPanel];
    panel.canChooseDirectories = true;
    panel.canChooseFiles = false;
    return browseModal(panel);
}

bool CocoaView::browseCancelled() { 
    return browseCancelled_;
}

std::string CocoaView::browseForOpeningFile() {
    auto panel = [NSOpenPanel openPanel];
    panel.canChooseDirectories = false;
    panel.canChooseFiles = true;
    return browseModal(panel);
}

std::string CocoaView::browseModal(NSOpenPanel *panel) {
    switch([panel runModal]) {
        case NSModalResponseOK:
            browseCancelled_ = false;
            return panel.URLs.lastObject.path.UTF8String;
        default:
            browseCancelled_ = true;
            return {};
    }
}

std::string CocoaView::audioDevice() {
    return deviceMenu.titleOfSelectedItem.UTF8String;
}

void CocoaView::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (auto item : items)
        [deviceMenu addItemWithTitle:asNsString(item)];
}

void CocoaView::addSubview(NSView *view) {
    [window.contentView addSubview:view];
}

void CocoaView::setDelegate(id<NSWindowDelegate> delegate) {
    [window setDelegate:delegate];
}

void CocoaView::center() {
    [window center];
}
}
