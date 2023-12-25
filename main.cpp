#include <iostream>
#include <algorithm>
#include <map>
#include <utility>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <stack>

using Dictionary = std::map<std::string, int>;
using Words = std::vector<std::string>;
using AnagramsForOneWord = std::vector<std::pair<std::string, int>>;
using AnagramTable = std::vector<AnagramsForOneWord>;

// Function prototypes
bool comparePairs(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b);
void sortPairsDescending(std::vector<std::pair<std::string, int>>& vec);
bool compareStrings(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b);
void sortPairsDescendingByString(std::vector<std::pair<std::string, int>>& vec);
Dictionary parseDictionary(const std::string& fileName);
Words parseWords(const std::string& fileName);
AnagramsForOneWord getAllAnagrams(const std::string& word, const Dictionary& dict);
AnagramTable getAnagramTable(const std::string& dictFile, const std::string& msgFile);
void generateAllAnagramCombinations(const AnagramTable& input, AnagramTable& allCombinations);
void print(const AnagramTable& table);

// Function definitions

bool comparePairs(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    if (a.second != b.second) {
        return a.second > b.second;
    }
    return a.first < b.first;
}

void sortPairsDescending(std::vector<std::pair<std::string, int>>& vec) {
    std::sort(vec.begin(), vec.end(), comparePairs);
}

bool compareStrings(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    return a.first > b.first;
}

void sortPairsDescendingByString(std::vector<std::pair<std::string, int>>& vec) {
    std::sort(vec.begin(), vec.end(), compareStrings);
}

Dictionary parseDictionary(const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in.is_open()) {
        throw std::runtime_error("Dictionary didn't open");
    }

    Dictionary dict;
    std::string line;

    while (std::getline(in, line)) {
        std::string key;
        int value;

        size_t delimiter = line.find(',');
        if (delimiter != std::string::npos) {
            key = line.substr(0, delimiter);
            try {
                value = std::stoi(line.substr(delimiter + 1));
                dict[key] = value;
            } catch (const std::invalid_argument& e) {
                // Обработка ошибки преобразования в число
                std::cerr << "Error converting to integer: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                // Обработка ошибки переполнения
                std::cerr << "Out of range error: " << e.what() << std::endl;
            }
        }
    }

    return dict;
}

Words parseWords(const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in.is_open()) {
        throw std::runtime_error("File with words didn't open");
    }

    Words words;
    std::string line;

    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            if (!word.empty()) {
                words.push_back(word);
            }
        }
    }

    return words;
}

AnagramsForOneWord getAllAnagrams(const std::string& word, const Dictionary& dict) {
    AnagramsForOneWord result;

    std::string sortedWord = word;
    std::sort(sortedWord.begin(), sortedWord.end(), std::greater<char>());

    do {
        auto it = dict.find(sortedWord);
        if (it != dict.cend()) {
            result.push_back({ it->first, it->second });
        }
    } while (std::prev_permutation(sortedWord.begin(), sortedWord.end()));

    return result;
}

AnagramTable getAnagramTable(const std::string& parseFile, const std::string& dictFile) {
    Dictionary dict = parseDictionary(dictFile);
    Words words = parseWords(parseFile);

    AnagramTable table;
    for (const auto& word : words) {
        AnagramsForOneWord anagramsW = getAllAnagrams(word, dict);
        if (anagramsW.empty()) {
            throw std::runtime_error("No anagram found for a word: " + word);
        }
        table.push_back(anagramsW);
    }

    return table;
}

void generateAllAnagramCombinations(const AnagramTable& input, AnagramTable& allCombinations) {
    std::stack<AnagramsForOneWord> currentStack;
    currentStack.push({});

    while (!currentStack.empty()) {
        AnagramsForOneWord currentCombination = currentStack.top();
        currentStack.pop();

        size_t currentIndex = currentCombination.size();

        if (currentIndex == input.size()) {
            allCombinations.push_back(currentCombination);
        } else {
            const AnagramsForOneWord& currentVector = input[currentIndex];

            for (const std::pair<std::string, int>& word : currentVector) {
                AnagramsForOneWord newCombination = currentCombination;
                newCombination.push_back(word);
                currentStack.push(newCombination);
            }
        }
    }
}

void print(const AnagramTable& table) {
    AnagramTable allCombinations;
    generateAllAnagramCombinations(table, allCombinations);

    std::vector<std::pair<std::string, int>> tempVector1;

    for (const AnagramsForOneWord& combination : allCombinations) {
        std::vector<std::pair<std::string, int>> tempVector;

        for (const std::pair<std::string, int>& word : combination) {
            if (word.first != "\n" && word.first != " " && word.second != 0) {
                tempVector.push_back(std::make_pair(word.first, word.second));
            }
        }

        sortPairsDescendingByString(tempVector);

        do {
            std::string strTemp = "";
            int count = 0;
            for (auto it : tempVector) {
                count += it.second;
                strTemp += it.first + ' ';
            }
            tempVector1.push_back(std::make_pair(strTemp, count));
        } while (std::prev_permutation(tempVector.begin(), tempVector.end()));
    }

    sortPairsDescending(tempVector1);

    for (auto& it : tempVector1) {
        std::cout << it.first + ' ' << "| weight = " << it.second << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Error: Incorrect number of arguments\n";
        std::cerr << "Usage: " << argv[0] << " <words_file> <dictionary_file>\n";
        return 1;
    }

    try {
        AnagramTable res = getAnagramTable(argv[1], argv[2]);
        print(res);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
