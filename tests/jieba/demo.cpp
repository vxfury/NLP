#include "jieba/Jieba.hpp"
#include <chrono>

const char *const DICT_PATH = "/Users/kiran/DEVEL/NLP/corpus/jieba/jieba.dict.utf8";
const char *const HMM_PATH = "/Users/kiran/DEVEL/NLP/corpus/jieba/hmm_model.utf8";
const char *const USER_DICT_PATH = "/Users/kiran/DEVEL/NLP/corpus/jieba/user.dict.utf8";
const char *const IDF_PATH = "/Users/kiran/DEVEL/NLP/corpus/jieba/idf.utf8";
const char *const STOP_WORD_PATH = "/Users/kiran/DEVEL/NLP/corpus/jieba/stop_words.utf8";

jieba::Jieba &GetTokenizer()
{
    static jieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    return jieba;
}

int main(int argc, char **argv)
{
    std::vector<std::string> words;
    std::vector<jieba::Word> jiebawords;
    std::string s;
    std::string result;

    GetTokenizer();
    auto elasped = [starttime = std::chrono::steady_clock::now()]() {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - starttime)
            .count();
    };

    s = "他来到了网易杭研大厦";
    std::cout << s << std::endl;
    std::cout << "[demo] Cut With HMM" << std::endl;
    GetTokenizer().Cut(s, words, true);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    std::cout << "[demo] Cut Without HMM " << std::endl;
    GetTokenizer().Cut(s, words, false);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    s = "我来到北京清华大学";
    std::cout << s << std::endl;
    std::cout << "[demo] CutAll" << std::endl;
    GetTokenizer().CutAll(s, words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    s = "小明硕士毕业于中国科学院计算所，后在日本京都大学深造";
    std::cout << s << std::endl;
    std::cout << "[demo] CutForSearch" << std::endl;
    GetTokenizer().CutForSearch(s, words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    std::cout << "[demo] Insert User Word" << std::endl;
    GetTokenizer().Cut("男默女泪", words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;
    GetTokenizer().InsertUserWord("男默女泪");
    GetTokenizer().Cut("男默女泪", words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    std::cout << "[demo] CutForSearch Word With Offset" << std::endl;
    GetTokenizer().CutForSearch(s, jiebawords, true);
    std::cout << jiebawords << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    std::cout << "[demo] Lookup Tag for Single Token" << std::endl;
    const int DemoTokenMaxLen = 32;
    char DemoTokens[][DemoTokenMaxLen] = {"拖拉机", "CEO", "123", "。"};
    std::vector<std::pair<std::string, std::string> > LookupTagres(sizeof(DemoTokens) / DemoTokenMaxLen);
    std::vector<std::pair<std::string, std::string> >::iterator it;
    for (it = LookupTagres.begin(); it != LookupTagres.end(); it++) {
        it->first = DemoTokens[it - LookupTagres.begin()];
        it->second = GetTokenizer().LookupTag(it->first);
    }
    std::cout << LookupTagres << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    std::cout << "[demo] Tagging" << std::endl;
    std::vector<std::pair<std::string, std::string> > tagres;
    s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。";
    GetTokenizer().Tag(s, tagres);
    std::cout << s << std::endl;
    std::cout << tagres << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    std::cout << "[demo] Keyword Extraction" << std::endl;
    const size_t topk = 5;
    std::vector<jieba::KeywordExtractor::Word> keywordres;
    GetTokenizer().extractor.Extract(s, keywordres, topk);
    std::cout << s << std::endl;
    std::cout << keywordres << std::endl;
    std::cout << "elasped: " << elasped() << " us" << std::endl << std::endl;

    return EXIT_SUCCESS;
}
