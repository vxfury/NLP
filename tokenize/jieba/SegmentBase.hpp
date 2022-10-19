#pragma once

#include "spdlog/spdlog.h"

#include "PreFilter.hpp"
#include <cassert>

namespace jieba
{

const char *const SPECIAL_SEPARATORS = " \t\n\xEF\xBC\x8C\xE3\x80\x82";

using namespace limonp;

class SegmentBase {
  public:
    SegmentBase()
    {
        ResetSeparators(SPECIAL_SEPARATORS);
        // XCHECK(ResetSeparators(SPECIAL_SEPARATORS));
    }
    virtual ~SegmentBase() {}

    virtual void Cut(const string &sentence, vector<string> &words) const = 0;

    bool ResetSeparators(const string &s)
    {
        symbols_.clear();
        RuneStrArray runes;
        if (!DecodeRunesInString(s, runes)) {
            spdlog::error("Decode {} failed.", s);
            return false;
        }
        for (size_t i = 0; i < runes.size(); i++) {
            if (!symbols_.insert(runes[i].rune).second) {
                spdlog::error("{} already exists", s.substr(runes[i].offset, runes[i].len));
                return false;
            }
        }
        return true;
    }

  protected:
    unordered_set<Rune> symbols_;
}; // class SegmentBase

} // namespace jieba