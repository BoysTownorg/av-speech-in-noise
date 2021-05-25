#include "EyeTrackerCalibration.hpp"

namespace av_speech_in_noise::eye_tracking::calibration {
static auto windowPoint(Point p) -> WindowPoint { return {p.x, 1 - p.y}; }

static void moveDotTo(
    SubjectView &view, Point p, SubjectPresenterImpl::DotState &dotState) {
    view.moveDotTo(windowPoint(p));
    dotState = SubjectPresenterImpl::DotState::moving;
}

void SubjectPresenterImpl::start() { view.show(); }

void SubjectPresenterImpl::stop() { view.hide(); }

void TesterPresenterImpl::start() { view.show(); }

void TesterPresenterImpl::stop() { view.hide(); }

void SubjectPresenterImpl::present(Point x) {
    pointPresenting = x;
    if (dotState == DotState::shrunk) {
        view.growDot();
        dotState = DotState::growing;
    } else
        moveDotTo(view, pointPresenting, dotState);
}

void SubjectPresenterImpl::notifyThatAnimationHasFinished() {
    if (dotState == DotState::growing)
        moveDotTo(view, pointPresenting, dotState);
    else if (dotState == DotState::shrinking) {
        dotState = DotState::shrunk;
        if (observer != nullptr)
            observer->notifyThatPointIsReady();
    } else {
        view.shrinkDot();
        dotState = DotState::shrinking;
    }
}

void TesterPresenterImpl::present(const std::vector<Result> &results) {
    view.clear();
    for (const auto &result : results) {
        view.drawWhiteCircleWithCenter(windowPoint(result.point));
        for (const auto point : result.leftEyeMappedPoints)
            view.drawRed(Line{windowPoint(result.point), windowPoint(point)});
        for (const auto point : result.rightEyeMappedPoints)
            view.drawGreen(Line{windowPoint(result.point), windowPoint(point)});
    }
}

static auto format(float x) -> std::string {
    std::stringstream stream;
    stream << x;
    return stream.str();
}

void validation::TesterPresenterImpl::present(const Result &result) {
    view.setLeftEyeAccuracyDegrees(format(result.left.errorOfMeanGaze.degrees));
    view.setLeftEyePrecisionDegrees(
        format(result.left.standardDeviationFromTheMeanGaze.degrees));
    view.setRightEyeAccuracyDegrees(
        format(result.right.errorOfMeanGaze.degrees));
    view.setRightEyePrecisionDegrees(
        format(result.right.standardDeviationFromTheMeanGaze.degrees));
}

void validation::TesterPresenterImpl::start() { view.show(); }

void validation::TesterPresenterImpl::stop() { view.hide(); }

validation::TesterPresenterImpl::TesterPresenterImpl(TesterView &view)
    : view{view} {}

Controller::Controller(Control &control, IInteractor &interactor)
    : interactor{interactor}, control{control} {
    control.attach(this);
}

void Controller::notifyThatWindowHasBeenTouched(WindowPoint point) {
    const auto whiteCircleCenters{control.whiteCircleCenters()};
    const auto whiteCircleDiameter{control.whiteCircleDiameter()};
    auto found{find_if(whiteCircleCenters.begin(), whiteCircleCenters.end(),
        [whiteCircleDiameter, point](WindowPoint candidate) {
            return std::hypot(point.x - candidate.x, point.y - candidate.y) <=
                whiteCircleDiameter / 2;
        })};
    if (found != whiteCircleCenters.end())
        interactor.redo(Point{
            static_cast<float>(found->x), 1 - static_cast<float>(found->y)});
}

void Controller::notifyThatSubmitButtonHasBeenClicked() { interactor.finish(); }

void Controller::notifyThatMenuHasBeenSelected() { interactor.start(); }
}
