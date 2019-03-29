#include "OutputFilePathImpl.hpp"
#include <sstream>

namespace recognition_test {
    OutputFilePath::OutputFilePath(TimeStamp *timeStamp) : timeStamp{timeStamp} {}
    
    std::string OutputFilePath::generateFileName(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        std::stringstream stream;
        stream << "Subject_";
        stream << test.subject;
        stream << "_Session_";
        stream << test.session;
        stream << "_Experimenter_";
        stream << test.experimenter;
        stream << "_";
        stream << timeStamp->year();
        stream << "-";
        stream << timeStamp->month();
        stream << "-";
        stream << timeStamp->dayOfMonth();
        stream << "-";
        stream << timeStamp->hour();
        stream << "-";
        stream << timeStamp->minute();
        stream << "-";
        stream << timeStamp->second();
        return stream.str();
    }
}
