#include <antares/exception/LoadingError.hpp>

#include "csv_writer.h"
#include "../logs.h"

namespace TimeElapsed
{
CSVWriter::CSVWriter(const Yuni::String& filename, ContentHandler* handler) : contentHandler_(handler)
{
    if (!mOutputFile.openRW(filename))
    {
        throw Antares::Error::CreatingTimeMeasurementFile(filename);
    }
}

void CSVWriter::flush()
{
    if (!contentHandler_)
        return;

    mOutputFile << "#item\tduration_ms\tNumber of calls\n";
    for (const auto& it : *contentHandler_)
    {
        const auto& label = it.first;
        const auto& info = it.second;
        mOutputFile << label << "\t" << info.duration_ms_ << "\t" << info.numberOfCalls_ << "\n";
    }
}
} // namespace TimeElapsed