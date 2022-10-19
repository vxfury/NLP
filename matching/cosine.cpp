#include <string>
#include <vector>
#include <set>
#include <map>
#include <numeric>

static std::map<std::string, int> CountTermFrequency(const std::vector<std::string> &input,
                                                     const std::set<std::string> &tokens)
{
    std::map<std::string, int> terms;
    for (auto const &word : input) {
        terms[word]++;
    }

    for (std::set<std::string, std::greater<std::string>>::const_iterator it = tokens.begin(); it != tokens.end();
         it++) {
        if (terms.find(*it) == terms.end()) {
            terms.insert(std::pair<std::string, int>(*it, 0));
        }
    }

    return terms;
}

static int DotProduct(const std::vector<int> &v1, const std::vector<int> &v2)
{
    return std::inner_product(v1.begin(), v1.end(), v2.begin(), 0);
}

double CosineSimilarity(const std::vector<std::string> &seq1, const std::vector<std::string> &seq2)
{
    std::set<std::string> tokens;
    std::for_each(seq1.begin(), seq1.end(), [&](const std::string &word) {
        tokens.insert(word);
    });
    std::for_each(seq2.begin(), seq2.end(), [&](const std::string &word) {
        tokens.insert(word);
    });

    auto M1 = CountTermFrequency(seq1, tokens);
    auto M2 = CountTermFrequency(seq2, tokens);

    std::vector<int> v1, v2;
    for (auto const &pair : M1) {
        v1.push_back(pair.second);
    }
    for (auto const &pair : M2) {
        v2.push_back(pair.second);
    }

    return DotProduct(v1, v2) / std::sqrt(DotProduct(v1, v1) * DotProduct(v2, v2));
}
