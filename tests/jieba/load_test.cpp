#include <iostream>
#include <ctime>
#include <fstream>
#include "spdlog/spdlog.h"
#include "jieba/MPSegment.hpp"
#include "jieba/HMMSegment.hpp"
#include "jieba/MixSegment.hpp"
#include "jieba/KeywordExtractor.hpp"

using namespace jieba;

void Cut(size_t times = 50)
{
    MixSegment seg("/Users/kiran/DEVEL/NLP/corpus/jieba/jieba.dict.utf8",
                   "/Users/kiran/DEVEL/NLP/corpus/jieba/hmm_model.utf8");
    vector<string> res;
    string doc;
    ifstream ifs("/Users/kiran/DEVEL/NLP/tests/jieba/testdata/weicheng.utf8");
    assert(ifs);
    doc << ifs;
    long beginTime = clock();
    for (size_t i = 0; i < times; i++) {
        printf("process [%3.0lf %%]\r", 100.0 * (i + 1) / times);
        fflush(stdout);
        res.clear();
        seg.Cut(doc, res);
    }
    printf("\n");
    long endTime = clock();
    spdlog::info("Cut: [{} seconds]time consumed.", double(endTime - beginTime) / CLOCKS_PER_SEC);
}

void Extract(size_t times = 400)
{
    KeywordExtractor Extractor(
        "/Users/kiran/DEVEL/NLP/corpus/jieba/jieba.dict.utf8", "/Users/kiran/DEVEL/NLP/corpus/jieba/hmm_model.utf8",
        "/Users/kiran/DEVEL/NLP/corpus/jieba/idf.utf8", "/Users/kiran/DEVEL/NLP/corpus/jieba/stop_words.utf8");
    vector<string> words;
    string doc;
    ifstream ifs("/Users/kiran/DEVEL/NLP/tests/jieba/testdata/review.100");
    assert(ifs);
    doc << ifs;
    long beginTime = clock();
    for (size_t i = 0; i < times; i++) {
        printf("process [%3.0lf %%]\r", 100.0 * (i + 1) / times);
        fflush(stdout);
        words.clear();
        Extractor.Extract(doc, words, 5);
    }
    printf("\n");
    long endTime = clock();
    spdlog::info("Extract: [{} seconds]time consumed.", double(endTime - beginTime) / CLOCKS_PER_SEC);
}

int main(int argc, char **argv)
{
    Cut();
    Extract();
    return EXIT_SUCCESS;
}
