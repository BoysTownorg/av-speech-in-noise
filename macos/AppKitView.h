#ifndef MACOS_MAIN_COCOAVIEW_H_
#define MACOS_MAIN_COCOAVIEW_H_

#include <av-speech-in-noise/ui/Consonant.hpp>
#include <av-speech-in-noise/ui/CoordinateResponseMeasure.hpp>

#import <AppKit/AppKit.h>

#include <unordered_map>

@class CoordinateResponseMeasureUIActions;
@class ConsonantUIActions;

namespace av_speech_in_noise {
class AppKitConsonantUI : public ConsonantTaskView,
                          public ConsonantTaskControl {
  public:
    explicit AppKitConsonantUI(NSView *);
    void attach(Observer *) override;
    void show() override;
    void hide() override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showReadyButton() override;
    void hideReadyButton() override;
    auto consonant() -> std::string override;
    void hideCursor() override;
    void notifyThatResponseButtonHasBeenClicked(id sender);
    void notifyThatReadyButtonHasBeenClicked();
    void showCursor() override;

  private:
    std::unordered_map<void *, std::string> consonants;
    NSView *view;
    NSStackView *responseButtons;
    NSButton *readyButton{};
    NSButton *lastButtonPressed{};
    ConsonantUIActions *actions;
    Observer *listener_{};
};

class AppKitCoordinateResponseMeasureUI
    : public CoordinateResponseMeasureControl,
      public CoordinateResponseMeasureView {
  public:
    explicit AppKitCoordinateResponseMeasureUI(NSView *);
    auto numberResponse() -> std::string override;
    auto greenResponse() -> bool override;
    auto blueResponse() -> bool override;
    auto whiteResponse() -> bool override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void attach(Observer *) override;
    void show() override;
    void hide() override;
    void notifyThatResponseButtonHasBeenClicked(id sender);
    void notifyThatReadyButtonHasBeenClicked();

  private:
    void addNextTrialButton();
    auto lastPressedColor() -> NSColor *;
    void addNumberButton(NSColor *color, int number, int row, std::size_t col);
    void addButtonRow(NSColor *color, int row);

    NSView *view;
    NSStackView *responseButtons{};
    NSButton *nextTrialButton;
    NSButton *lastButtonPressed{};
    CoordinateResponseMeasureUIActions *actions;
    Observer *listener_{};
};
}

#endif
