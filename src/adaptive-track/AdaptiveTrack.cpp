#include "AdaptiveTrack.hpp"

namespace adaptive_track {
    AdaptiveTrack::AdaptiveTrack(const Settings &p) :
        x_{p.startingX},
        ceiling_{p.ceiling},
        floor_{p.floor},
        bumpLimit_{p.bumpLimit},
        bumpCount_{0}
    {
        for (const auto &sequence : *p.rule)
            if (sequence.runCount) {
                stepSizes.push_back(sequence.stepSize);
                runCounts.push_back(sequence.runCount);
                up.push_back(sequence.up);
                down.push_back(sequence.down);
            }
        stepSizes.push_back(0);
    }
    
    void AdaptiveTrack::pushUp() {
        updateBumpCount(ceiling_);
        update(Direction::up, up, &AdaptiveTrack::stepUp);
    }
    
    void AdaptiveTrack::updateBumpCount(int bumpBoundary) {
        bumpCount_ = x_ == bumpBoundary
            ? bumpCount_ + 1
            : 0;
        
        if (bumpCount_ == bumpLimit_)
            bumpedOut = true;
    }
    
    void AdaptiveTrack::update(
        Direction direction,
        const std::vector<int> &thresholds,
        void(AdaptiveTrack::*whenThresholdMet)()
    ) {
        if (complete_())
            return;
        
        updateConsecutiveCount(direction);
        callIfConsecutiveCountMet(whenThresholdMet, thresholds.at(sequenceIndex));
        previousDirection = direction;
    }
    
    void AdaptiveTrack::callIfConsecutiveCountMet(
        void(AdaptiveTrack::*f)(),
        int threshold
    ) {
        if (consecutiveCountMet(threshold))
            (this->*f)();
    }
    
    bool AdaptiveTrack::consecutiveCountMet(int threshold) {
        return sameDirectionConsecutiveCount == threshold;
    }

    void AdaptiveTrack::updateConsecutiveCount(Direction direction) {
        sameDirectionConsecutiveCount = previousDirection == direction
            ? sameDirectionConsecutiveCount + 1
            : 1;
    }
    
    void AdaptiveTrack::stepUp() {
        updateReversals(Step::fall);
        x_ = std::min(x_ + stepSize(), ceiling_);
        sameDirectionConsecutiveCount = 0;
        previousStep = Step::rise;
    }

    void AdaptiveTrack::updateReversals(Step step) {
        if (previousStep == step)
            reversal();
    }

    int AdaptiveTrack::stepSize() {
        return stepSizes.at(sequenceIndex);
    }

    void AdaptiveTrack::reversal() {
        ++reversals_;
        if (++runCounter == runCounts.at(sequenceIndex)) {
            runCounter = 0;
            ++sequenceIndex;
        }
    }
    
    void AdaptiveTrack::pushDown() {
        updateBumpCount(floor_);
        update(Direction::down, down, &AdaptiveTrack::stepDown);
    }

    void AdaptiveTrack::stepDown() {
        updateReversals(Step::rise);
        x_ = std::max(x_ - stepSize(), floor_);
        sameDirectionConsecutiveCount = 0;
        previousStep = Step::fall;
    }

    int AdaptiveTrack::x() {
        return x_;
    }

    bool AdaptiveTrack::complete() {
        return complete_();
    }

    bool AdaptiveTrack::complete_() const {
        return sequenceIndex == runCounts.size() || bumpedOut;
    }

    int AdaptiveTrack::reversals() {
        return reversals_;
    }
}
