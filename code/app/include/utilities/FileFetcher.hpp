#pragma once
#ifndef __NR_FILE_FETCHER_HPP__
#define __NR_FILE_FETCHER_HPP__

#include <optional>
#include <string>

namespace NRenderer
{
    using namespace std;
    class FileFetcher
    {
    public:
        FileFetcher() = default;
        ~FileFetcher() = default;
        optional<string> fetch(const char* filter) const;
    };
} // namespace NRenderer


#endif