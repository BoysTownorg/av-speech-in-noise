#ifndef TESTS_TARGETLISTSTUB_HPP_
#define TESTS_TARGETLISTSTUB_HPP_

#include "LogString.hpp"
#include <recognition-test/Model.hpp>
#include <utility>

namespace av_speech_in_noise {
class TargetListStub : public virtual TargetList {
  public:
    auto current() -> std::string override { return current_; }

    void setCurrent(std::string s) { current_ = std::move(s); }

    void setCurrentTargetWhenNext(std::string s) {
        currentWhenNext_ = std::move(s);
    }

    void loadFromDirectory(std::string directory) override {
        log_.insert("loadFromDirectory ");
        directory_ = std::move(directory);
    }

    auto next() -> std::string override {
        log_.insert("next ");
        nextCalled_ = true;
        current_ = currentWhenNext_;
        return next_;
    }

    void setNext(std::string s) { next_ = std::move(s); }

    auto directory() { return directory_; }

    auto log() const -> auto & { return log_; }

  private:
    LogString log_{};
    std::string currentWhenNext_{};
    std::string directory_{};
    std::string next_{};
    std::string current_{};
    bool nextCalled_{};
};

class FiniteTargetListStub : public FiniteTargetList, public TargetListStub {
  public:
    void setEmpty() { empty_ = true; }

    auto empty() -> bool override { return empty_; }

    void reinsertCurrent() override { reinsertCurrentCalled_ = true; }

    auto reinsertCurrentCalled() const { return reinsertCurrentCalled_; }

  private:
    bool empty_{};
    bool reinsertCurrentCalled_{};
};
}

#endif
