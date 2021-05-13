import SwiftUI
import Foundation
import AppKit

// https://www.swiftbysundell.com/tips/inline-wrapping-of-uikit-or-appkit-views-within-swiftui/
struct Wrap<Wrapped: NSView>: NSViewRepresentable {
    typealias Updater = (Wrapped, Context) -> Void

    var makeView: () -> Wrapped
    var update: (Wrapped, Context) -> Void

    init(_ makeView: @escaping @autoclosure () -> Wrapped,
         updater update: @escaping Updater) {
        self.makeView = makeView
        self.update = update
    }

    func makeNSView(context: Context) -> Wrapped {
        makeView()
    }

    func updateNSView(_ view: Wrapped, context: Context) {
        update(view, context)
    }
}

extension Wrap {
    init(_ makeView: @escaping @autoclosure () -> Wrapped,
         updater update: @escaping (Wrapped) -> Void) {
        self.makeView = makeView
        self.update = { view, _ in update(view) }
    }

    init(_ makeView: @escaping @autoclosure () -> Wrapped) {
        self.makeView = makeView
        self.update = { _, _ in }
    }
}

struct IdentifiableString : Identifiable {
    let string: String
    var id: String { string }
}

struct IdentifiableStringCollection : Identifiable {
    var id: String
    let items: [IdentifiableString]
}

class ObservableStringCollection: ObservableObject {
    @Published var items = [IdentifiableString]()
}

class TestSetupUIObserverObservable : ObservableObject {
    @Published var observer : TestSetupUIObserver? = nil
}

class TestUIObserverObservable : ObservableObject {
    @Published var observer : TestUIObserver? = nil
}

class FreeResponseUIObserverObservable : ObservableObject {
    @Published var observer : FreeResponseUIObserver? = nil
}

class SyllablesUIObserverObservable : ObservableObject {
    @Published var observer : SyllablesUIObserver? = nil
}

class ChooseKeywordsUIObserverObservable : ObservableObject {
    @Published var observer : ChooseKeywordsUIObserver? = nil
}

class CorrectKeywordsUIObserverObservable : ObservableObject {
    @Published var observer : CorrectKeywordsUIObserver? = nil
}

class PassFailUIObserverObservable : ObservableObject {
    @Published var observer : PassFailUIObserver? = nil
}

class ObservableBool : ObservableObject {
    @Published var value = false
}

class ObservableString : ObservableObject {
    @Published var string = ""
}

struct SwiftSessionView : View {
    @ObservedObject var audioDevice: ObservableString
    @ObservedObject var showErrorMessage: ObservableBool
    @ObservedObject var errorMessage: ObservableString
    @ObservedObject var audioDevices: ObservableStringCollection
    
    init(ui: SwiftSessionUI) {
        audioDevices = ui.audioDevices
        audioDevice = ui.audioDevice_
        errorMessage = ui.errorMessage
        showErrorMessage = ui.showErrorMessage
    }
    
    var body: some View {
        Picker("Audio Device", selection: $audioDevice.string) {
            ForEach(audioDevices.items) {
                Text($0.string)
            }
        }
        .alert(isPresented: $showErrorMessage.value) {
            Alert(
                title: Text("Error"),
                message: Text(errorMessage.string)
            )
        }
    }
}

class SwiftSessionUI : NSObject, SessionUI {
    var showErrorMessage = ObservableBool()
    var errorMessage = ObservableString()
    var audioDevices = ObservableStringCollection()
    var audioDevice_ = ObservableString()
    
    func eventLoop() {}
    
    func showErrorMessage(_ something: String!) {
        showErrorMessage.value = true
        errorMessage.string = something
    }
    
    func audioDevice() -> String! {
        return audioDevice_.string
    }
    
    func populateAudioDeviceMenu(_ devices: [String]!) {
        for each in devices {
            audioDevices.items.append(IdentifiableString(string: each))
        }
    }
}

struct SwiftTestSetupView: View {
    @ObservedObject var testerId: ObservableString
    @ObservedObject var subjectId: ObservableString
    @ObservedObject var session: ObservableString
    @ObservedObject var startingSnr: ObservableString
    @ObservedObject var transducer: ObservableString
    @ObservedObject var rmeSetting: ObservableString
    @ObservedObject var transducers: ObservableStringCollection
    @ObservedObject var observableObserver: TestSetupUIObserverObservable
    let testSettingsPathControl: NSPathControl
    
    init(ui: SwiftTestSetupUI, testSettingsPathControl: NSPathControl){
        transducers = ui.transducers
        rmeSetting = ui.rmeSetting_
        testerId = ui.testerId_
        subjectId = ui.subjectId_
        session = ui.session_
        startingSnr = ui.startingSnr_
        transducer = ui.transducer_
        observableObserver = ui.observableObserver
        self.testSettingsPathControl = testSettingsPathControl
        self.testSettingsPathControl.pathStyle = NSPathControl.Style.popUp
        self.testSettingsPathControl.allowedTypes = ["txt"]
    }
    
    var body: some View {
        Form() {
            TextField(
                "subject ID",
                text: $subjectId.string)
                .disableAutocorrection(true)
            TextField(
                "tester ID",
                text: $testerId.string)
                .disableAutocorrection(true)
            TextField(
                "session",
                text: $session.string)
                .disableAutocorrection(true)
            TextField(
                "RME setting",
                text: $rmeSetting.string)
                .disableAutocorrection(true)
            Picker("Transducer", selection: $transducer.string) {
                ForEach(transducers.items) {
                    Text($0.string)
                }
            }
            HStack() {
                Wrap(testSettingsPathControl)
                Button(action: {
                    observableObserver.observer?.notifyThatPlayCalibrationButtonHasBeenClicked()
                }) {
                    Text("Play Calibration")
                }
            }
            TextField(
                "starting SNR (dB)",
                text: $startingSnr.string)
                .disableAutocorrection(true)
        }.padding()
        Button(action: {
            observableObserver.observer?.notifyThatConfirmButtonHasBeenClicked()
        }) {
            Text("Confirm")
        }.keyboardShortcut(.defaultAction)
    }
}

class SwiftTestSetupUI : NSObject, TestSetupUI {
    let transducers = ObservableStringCollection()
    let observableObserver = TestSetupUIObserverObservable()
    let showing = ObservableBool()
    let rmeSetting_ = ObservableString()
    let testerId_ = ObservableString()
    let subjectId_ = ObservableString()
    let session_ = ObservableString()
    let startingSnr_ = ObservableString()
    let transducer_ = ObservableString()
    let testSettingsPathControl: NSPathControl
    
    init(testSettingsPathControl: NSPathControl) {
        self.testSettingsPathControl = testSettingsPathControl
        showing.value = true
    }
    
    func show() {
        showing.value = true
    }
    
    func hide() {
        showing.value = false
    }
    
    func testerId() -> String { return testerId_.string }
    
    func subjectId() -> String { return subjectId_.string }
    
    func session() -> String { return session_.string }
    
    func testSettingsFile() -> String {
        return testSettingsPathControl.url?.path ?? ""
    }
    
    func startingSnr() -> String {
        return startingSnr_.string
    }
    
    func transducer() -> String { return transducer_.string }
    
    func rmeSetting() -> String { return rmeSetting_.string }
    
    func populateTransducerMenu(_ transducers: Array<String>) {
        for transducer in transducers {
            self.transducers.items.append(IdentifiableString(string: transducer))
        }
    }
    
    func attach(_ observer: TestSetupUIObserver!) {
        observableObserver.observer = observer
    }
}

struct SwiftTestView : View {
    @ObservedObject var observableObserver: TestUIObserverObservable
    @ObservedObject var exitTestButtonEnabled: ObservableBool
    @ObservedObject var nextTrialButtonEnabled: ObservableBool
    
    init(ui: SwiftTestUI) {
        observableObserver = ui.observableObserver
        exitTestButtonEnabled = ui.exitTestButtonEnabled
        nextTrialButtonEnabled = ui.nextTrialButtonEnabled
    }
    
    var body: some View {
        Button(action: {
            observableObserver.observer?.exitTest()
        }) {
            Text("Exit Test")
        }.disabled(!exitTestButtonEnabled.value)
        Button(action: {
            observableObserver.observer?.playTrial()
        }) {
            Text("Play Trial")
        }.keyboardShortcut(.defaultAction)
        .disabled(!nextTrialButtonEnabled.value)
    }
}

class SwiftTestUI : NSObject, TestUI {
    let observableObserver = TestUIObserverObservable()
    let exitTestButtonEnabled = ObservableBool()
    let nextTrialButtonEnabled = ObservableBool()
    
    func attach(_ observer: TestUIObserver!) {
        observableObserver.observer = observer
    }
    
    func showExitTestButton() {
        exitTestButtonEnabled.value = true
    }
    
    func hideExitTestButton() {
        exitTestButtonEnabled.value = false
    }
    
    func show() {
    }
    
    func hide() {
    }
    
    func display(_ something: String!) {
        
    }
    
    func secondaryDisplay(_ something: String!) {
        
    }
    
    func showNextTrialButton() {
        nextTrialButtonEnabled.value = true
    }
    
    func hideNextTrialButton() {
        nextTrialButtonEnabled.value = false
    }
    
    func showContinueTestingDialog() {
        
    }
    
    func hideContinueTestingDialog() {
        
    }
    
    func setContinueTestingDialogMessage(_ something: String!) {
        
    }
    
    func showSheet(_ something: String!) {
        
    }
}

struct SwiftSyllablesView : View {
    @ObservedObject var syllable: ObservableString
    @ObservedObject var flagged: ObservableBool
    @ObservedObject var observableObserver: SyllablesUIObserverObservable
    @ObservedObject var showing: ObservableBool
    
    init(ui: SwiftSyllablesUI) {
        syllable = ui.syllable_
        flagged = ui.flagged_
        showing = ui.showing
        observableObserver = ui.observableObserver
    }
    
    private let syllables: [IdentifiableStringCollection] = [
        IdentifiableStringCollection(id: "a", items:[
            IdentifiableString(string: "B"),
            IdentifiableString(string: "D"),
            IdentifiableString(string: "G"),
            IdentifiableString(string: "F"),
            IdentifiableString(string: "Ghee"),
            IdentifiableString(string: "H"),
            IdentifiableString(string: "Yee"),
        ]),
        IdentifiableStringCollection(id: "b", items:[
            IdentifiableString(string: "K"),
            IdentifiableString(string: "L"),
            IdentifiableString(string: "M"),
            IdentifiableString(string: "N"),
            IdentifiableString(string: "P"),
            IdentifiableString(string: "R"),
            IdentifiableString(string: "Sh"),
        ]),
        IdentifiableStringCollection(id: "c", items:[
            IdentifiableString(string: "S"),
            IdentifiableString(string: "Th"),
            IdentifiableString(string: "T"),
            IdentifiableString(string: "Ch"),
            IdentifiableString(string: "V"),
            IdentifiableString(string: "W"),
            IdentifiableString(string: "Z")
        ])
    ]
    
    var body: some View {
        if showing.value {
            Toggle("flagged", isOn: $flagged.value)
            ForEach(syllables, content: { row in
                HStack() {
                    ForEach(row.items, content: { column in
                        Button(column.string, action: {
                            syllable.string = column.string
                            observableObserver.observer?.notifyThatResponseButtonHasBeenClicked()
                        })
                    })
                }
            })
        }
    }
}

class SwiftSyllablesUI : NSObject, SyllablesUI {
    let syllable_ = ObservableString()
    let flagged_ = ObservableBool()
    let observableObserver = SyllablesUIObserverObservable()
    let showing = ObservableBool()
    
    func hide() {
        showing.value = false
    }
    
    func show() {
        showing.value = true
    }
    
    func syllable() -> String! {
        return syllable_.string;
    }
    
    func flagged() -> Bool {
        return flagged_.value;
    }
    
    func clearFlag() {
        flagged_.value = false
    }
    
    func attach(_ observer: SyllablesUIObserver!) {
        observableObserver.observer = observer
    }
}

struct SwiftFreeResponseView : View {
    @ObservedObject var freeResponse: ObservableString
    @ObservedObject var flagged: ObservableBool
    @ObservedObject var observableObserver: FreeResponseUIObserverObservable
    @ObservedObject var showing: ObservableBool
    
    init(ui: SwiftFreeResponseUI) {
        freeResponse = ui.freeResponse_
        flagged = ui.flagged_
        showing = ui.showing
        observableObserver = ui.observableObserver
    }
    
    var body: some View {
        if showing.value {
            Form {
                Toggle("flagged", isOn: $flagged.value)
                TextField("response", text: $freeResponse.string)
            }.padding()
            Button(action: {
                observableObserver.observer?.notifyThatSubmitButtonHasBeenClicked()
            }) {
                Text("Submit")
            }.keyboardShortcut(.defaultAction)
        }
    }
}

class SwiftFreeResponseUI : NSObject, FreeResponseUI {
    let freeResponse_ = ObservableString()
    let flagged_ = ObservableBool()
    let observableObserver = FreeResponseUIObserverObservable()
    let showing = ObservableBool()
    
    func attach(_ observer: FreeResponseUIObserver!) {
        observableObserver.observer = observer
    }
    
    func showFreeResponseSubmission() {
        showing.value = true
    }
    
    func hideFreeResponseSubmission() {
        showing.value = false
    }
    
    func freeResponse() -> String! {
        return freeResponse_.string
    }
    
    func flagged() -> Bool {
        return flagged_.value
    }
    
    func clearFreeResponse() {
        freeResponse_.string = ""
    }
    
    func clearFlag() {
        flagged_.value = false
    }
}

struct SwiftChooseKeywordsView : View {
    @ObservedObject var firstKeywordCorrect_: ObservableBool
    @ObservedObject var secondKeywordCorrect_: ObservableBool
    @ObservedObject var thirdKeywordCorrect_: ObservableBool
    @ObservedObject var flagged_: ObservableBool
    @ObservedObject var firstKeywordButtonText: ObservableString
    @ObservedObject var secondKeywordButtonText: ObservableString
    @ObservedObject var thirdKeywordButtonText: ObservableString
    @ObservedObject var textPrecedingFirstKeywordButton: ObservableString
    @ObservedObject var textFollowingFirstKeywordButton: ObservableString
    @ObservedObject var textFollowingSecondKeywordButton: ObservableString
    @ObservedObject var textFollowingThirdKeywordButton: ObservableString
    @ObservedObject var showing: ObservableBool
    @ObservedObject var observableObserver: ChooseKeywordsUIObserverObservable
    
    init(ui: SwiftChooseKeywordsUI) {
        firstKeywordButtonText = ui.firstKeywordButtonText
        secondKeywordButtonText = ui.secondKeywordButtonText
        thirdKeywordButtonText = ui.thirdKeywordButtonText
        flagged_ = ui.flagged_
        firstKeywordCorrect_ = ui.firstKeywordCorrect_
        secondKeywordCorrect_ = ui.secondKeywordCorrect_
        thirdKeywordCorrect_ = ui.thirdKeywordCorrect_
        textPrecedingFirstKeywordButton = ui.textPrecedingFirstKeywordButton
        textFollowingFirstKeywordButton = ui.textFollowingFirstKeywordButton
        textFollowingSecondKeywordButton = ui.textFollowingSecondKeywordButton
        textFollowingThirdKeywordButton = ui.textFollowingThirdKeywordButton
        showing = ui.showing
        observableObserver = ui.observableObserver
    }
    
    var body: some View {
        if showing.value {
            Toggle("flagged", isOn: $flagged_.value)
            HStack() {
                Text(textPrecedingFirstKeywordButton.string)
                Button(firstKeywordButtonText.string, action: {
                    observableObserver.observer?.notifyThatFirstKeywordButtonIsClicked()
                })
                Text(textFollowingFirstKeywordButton.string)
                Button(secondKeywordButtonText.string, action: {
                    observableObserver.observer?.notifyThatSecondKeywordButtonIsClicked()
                })
                Text(textFollowingSecondKeywordButton.string)
                Button(thirdKeywordButtonText.string, action: {
                    observableObserver.observer?.notifyThatThirdKeywordButtonIsClicked()
                })
                Text(textFollowingThirdKeywordButton.string)
            }
            HStack() {
                Button("Reset", action: {
                    observableObserver.observer?.notifyThatResetButtonIsClicked()
                })
                Button("All wrong", action: {
                    observableObserver.observer?.notifyThatAllWrongButtonHasBeenClicked()
                })
                Button("Submit", action: {
                    observableObserver.observer?.notifyThatSubmitButtonHasBeenClicked()
                }).keyboardShortcut(.defaultAction)
            }
        }
    }
}

class SwiftChooseKeywordsUI : NSObject, ChooseKeywordsUI {
    let firstKeywordCorrect_ = ObservableBool()
    let secondKeywordCorrect_ = ObservableBool()
    let thirdKeywordCorrect_ = ObservableBool()
    let flagged_ = ObservableBool()
    let firstKeywordButtonText = ObservableString()
    let secondKeywordButtonText = ObservableString()
    let thirdKeywordButtonText = ObservableString()
    let textPrecedingFirstKeywordButton = ObservableString()
    let textFollowingFirstKeywordButton = ObservableString()
    let textFollowingSecondKeywordButton = ObservableString()
    let textFollowingThirdKeywordButton = ObservableString()
    let showing = ObservableBool()
    let observableObserver = ChooseKeywordsUIObserverObservable()
    
    func attach(_ observer: ChooseKeywordsUIObserver!) {
        observableObserver.observer = observer
    }
    
    func firstKeywordCorrect() -> Bool {
        return firstKeywordCorrect_.value
    }
    
    func secondKeywordCorrect() -> Bool {
        return secondKeywordCorrect_.value
    }
    
    func thirdKeywordCorrect() -> Bool {
        return thirdKeywordCorrect_.value
    }
    
    func flagged() -> Bool {
        return flagged_.value
    }
    
    func clearFlag() {
        flagged_.value = false
    }
    
    func markFirstKeywordIncorrect() {
        firstKeywordCorrect_.value = false
    }
    
    func markSecondKeywordIncorrect() {
        secondKeywordCorrect_.value = false
    }
    
    func markThirdKeywordIncorrect() {
        thirdKeywordCorrect_.value = false
    }
    
    func markFirstKeywordCorrect() {
        firstKeywordCorrect_.value = true
    }
    
    func markSecondKeywordCorrect() {
        secondKeywordCorrect_.value = true
    }
    
    func markThirdKeywordCorrect() {
        thirdKeywordCorrect_.value = true
    }
    
    func hideResponseSubmission() {
        showing.value = false
    }
    
    func showResponseSubmission() {
        showing.value = true
    }
    
    func setFirstKeywordButtonText(_ text: String!) {
        firstKeywordButtonText.string = text
    }
    
    func setSecondKeywordButtonText(_ text: String!) {
        secondKeywordButtonText.string = text
    }
    
    func setThirdKeywordButtonText(_ text: String!) {
        thirdKeywordButtonText.string = text
    }
    
    func setTextPrecedingFirstKeywordButton(_ text: String!) {
        textPrecedingFirstKeywordButton.string = text
    }
    
    func setTextFollowingFirstKeywordButton(_ text: String!) {
        textFollowingFirstKeywordButton.string = text
    }
    
    func setTextFollowingSecondKeywordButton(_ text: String!) {
        textFollowingSecondKeywordButton.string = text
    }
    
    func setTextFollowingThirdKeywordButton(_ text: String!) {
        textFollowingThirdKeywordButton.string = text
    }
}

struct SwiftCorrectKeywordsView : View {
    @ObservedObject var showing: ObservableBool
    @ObservedObject var correctKeywords_: ObservableString
    @ObservedObject var observableObserver: CorrectKeywordsUIObserverObservable
    
    init(ui: SwiftCorrectKeywordsUI) {
        showing = ui.showing
        correctKeywords_ = ui.correctKeywords_
        observableObserver = ui.observableObserver
    }
    
    var body: some View {
        if showing.value {
            Form {
                TextField("# keywords correct", text: $correctKeywords_.string)
                Button("Submit", action: {
                    observableObserver.observer?.notifyThatSubmitButtonHasBeenClicked()
                }).keyboardShortcut(.defaultAction)
            }
        }
    }
}

class SwiftCorrectKeywordsUI : NSObject, CorrectKeywordsUI {
    let showing = ObservableBool()
    let correctKeywords_ = ObservableString()
    let observableObserver = CorrectKeywordsUIObserverObservable()
    
    func attach(_ observer: CorrectKeywordsUIObserver!) {
        observableObserver.observer = observer
    }
    
    func hideCorrectKeywordsSubmission() {
        showing.value = false
    }
    
    func showCorrectKeywordsSubmission() {
        showing.value = true
    }
    
    func correctKeywords() -> String! {
        return correctKeywords_.string
    }
}

struct SwiftPassFailView : View {
    @ObservedObject var showing: ObservableBool
    @ObservedObject var observableObserver: PassFailUIObserverObservable
    
    init(ui: SwiftPassFailUI) {
        showing = ui.showing
        observableObserver = ui.observableObserver
    }
    
    var body: some View {
        if showing.value {
            HStack() {
                Button("Incorrect", action: {
                    observableObserver.observer?.notifyThatIncorrectButtonHasBeenClicked()
                })
                Button("Correct", action: {
                    observableObserver.observer?.notifyThatCorrectButtonHasBeenClicked()
                })
            }
        }
    }
}

class SwiftPassFailUI : NSObject, PassFailUI {
    let showing = ObservableBool()
    let observableObserver = PassFailUIObserverObservable()
    
    func attach(_ observer: PassFailUIObserver!) {
        observableObserver.observer = observer
    }
    
    func hideEvaluationButtons() {
        showing.value = false
    }
    
    func showEvaluationButtons() {
        showing.value = true
    }
}

class SwiftTestSetupUIFactory : NSObject, TestSetupUIFactory {
    let testSetupUI: TestSetupUI
    
    init(testSetupUI: TestSetupUI){
        self.testSetupUI = testSetupUI
        super.init()
    }
    
    func make(_ viewController: NSViewController!) -> TestSetupUI! {
        return testSetupUI;
    }
}

struct SwiftFacemaskStudyTestSetupView : View {
    @ObservedObject var subjectID_: ObservableString
    @ObservedObject var testSettingsShortName: ObservableString
    @ObservedObject var observableObserver: TestSetupUIObserverObservable
    
    private let testSettingsShortNames = [IdentifiableString]()
    
    var body: some View {
        HStack() {
            Image("btnrh.png")
            Text("Facemask Study")
        }
        TextField("Subject ID", text: $subjectID_.string)
        Picker("Test Settings", selection: $testSettingsShortName.string) {
            ForEach(testSettingsShortNames) {
                Text($0.string)
            }
        }.pickerStyle(RadioGroupPickerStyle())
        Button("START", action: {
            observableObserver.observer?.notifyThatConfirmButtonHasBeenClicked()
        })
        Button("play left speaker", action: {
            observableObserver.observer?.notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked()
        })
        Button("play right speaker", action: {
            observableObserver.observer?.notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked()
        })
    }
}

@main
struct SwiftCPPApp: App {
    let testSettingsPathControl = NSPathControl()
    let sessionUI = SwiftSessionUI()
    let sessionView: SwiftSessionView
    let testSetupUI: SwiftTestSetupUI
    let testSetupView: SwiftTestSetupView
    let testUI = SwiftTestUI()
    let testView: SwiftTestView
    let freeResponseUI = SwiftFreeResponseUI()
    let freeResponseView: SwiftFreeResponseView
    let syllablesUI = SwiftSyllablesUI()
    let syllablesView: SwiftSyllablesView
    let chooseKeywordsUI = SwiftChooseKeywordsUI()
    let chooseKeywordsView: SwiftChooseKeywordsView
    let correctKeywordsUI = SwiftCorrectKeywordsUI()
    let correctKeywordsView: SwiftCorrectKeywordsView
    let passFailUI = SwiftPassFailUI()
    let passFailView: SwiftPassFailView
    @ObservedObject var showingTestSetup: ObservableBool
    
    init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionView = SwiftSessionView(ui: sessionUI)
        testSetupView = SwiftTestSetupView(ui: testSetupUI, testSettingsPathControl:testSettingsPathControl)
        testView = SwiftTestView(ui: testUI)
        freeResponseView = SwiftFreeResponseView(ui: freeResponseUI)
        syllablesView = SwiftSyllablesView(ui: syllablesUI)
        chooseKeywordsView = SwiftChooseKeywordsView(ui: chooseKeywordsUI)
        correctKeywordsView = SwiftCorrectKeywordsView(ui: correctKeywordsUI)
        passFailView = SwiftPassFailView(ui: passFailUI)
        showingTestSetup = testSetupUI.showing
        HelloWorldObjc.doEverything(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: testUI, with: freeResponseUI, with: syllablesUI, with: chooseKeywordsUI, with: correctKeywordsUI, with: passFailUI)
    }
    
    var body: some Scene {
        WindowGroup {
            sessionView
            if showingTestSetup.value {
                testSetupView
            }
            else {
                testView
                freeResponseView
                syllablesView
                chooseKeywordsView
                correctKeywordsView
                passFailView
            }
        }
    }
}
