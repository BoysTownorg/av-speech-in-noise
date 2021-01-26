#include "Syllables.hpp"

namespace av_speech_in_noise {
SyllablesPresenterImpl::SyllablesPresenterImpl(SyllablesView &view)
    : view{view} {}

void SyllablesPresenterImpl::start() {}

void SyllablesPresenterImpl::stop() { view.hide(); }

void SyllablesPresenterImpl::showResponseSubmission() {}

void SyllablesPresenterImpl::hideResponseSubmission() { view.hide(); }
}