#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;


class findRhythm {
private:
    vector<pair<string, int>> topx;
    unordered_map<int, string> rhymeMap;

    string input, input_lastSyllable, cils = "";
    int countWords = 0;

    //TODO: Reduce the time needed for checking, storing and sorting the word. //hashing with map maybe? ask gpt
    //TODO(DONE): Implement Levenshtein distance on your own
    //TODO(DONE): Implement quick sort and partitioning on your own
    //TODO: Parallel Processing. Running the app on multiple cores/threads will help with time
    //TODO: Search info about Trie Data Structure or Hash Tables, they might help with time

    //Finds the last syllable of the word
    string find_last_syllable(const string& word) {
        string syllable = "";
        int position = word.size() - 1;
        int vowelCount = 0;

        while (position >= 0) {
            char ch = word[position];
            if (ch != 'a' && ch != 'e' && ch != 'i' && ch != 'o' && ch != 'u' && ch != 'y' &&
                ch != 'A' && ch != 'E' && ch != 'I' && ch != 'O' && ch != 'U' && ch != 'Y') {
                if (vowelCount >= 1) { // Stop after the first consonant following a vowel/vowels
                    syllable += ch;
                    break;
                }
                syllable += ch;
            }
            else {
                syllable += ch;
                vowelCount++;
            }
            position--;
        }

        reverse(syllable.begin(), syllable.end());
        return syllable;
    }


    //Returns if last syllables are same
    bool check_syllables_end(const string& word, int mode) { //You make a function to check if NO words have been matched as well and repeat the process.
        //mode 1 checks the syllables
        //mode 2 checks similar syllables(Input syllable - OX and word syllable ROX)  
        if (word >= input_lastSyllable) {
            string syl = find_last_syllable(word);
            if (mode == 1) {
                if (syl == input_lastSyllable) return true;
            }
            else {
                if (syl.size() > input_lastSyllable.size()) {
                    syl.erase(0, syl.size() - input_lastSyllable.size());
                }
                if(syl == input_lastSyllable) return true;
            }
        }

        return false;
    }

    int LevenshteinDistance(string word) {
        int m = word.length();
        int n = input.length();

        std::vector<std::vector<int>> d(m + 1, std::vector<int>(n + 1, 0));

        for (int i = 1; i <= m; ++i) {
            d[i][0] = i;
        }
        for (int j = 1; j <= n; ++j) {
            d[0][j] = j;
        }

        //The Levenshtein distance
        for (int i = 1; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                int substitutionCost;
                if (word[i - 1] == input[j - 1])substitutionCost = 0;
                else substitutionCost = 1;
                d[i][j] = std::min({ d[i - 1][j] + 1,        // deletion
                                    d[i][j - 1] + 1,        // insertion
                                    d[i - 1][j - 1] + substitutionCost }); // substitution goddam dis shits hard
            }
        }

        return d[m][n];
    }

    int Partition(int left, int right, int pivot) {
        int i = left, j = right;
        while (i <= j) {
            while (i <= j && topx[i].second < pivot) {
                i++;
            }
            while (i <= j && topx[j].second > pivot) {
                j--;
            }
            if (i <= j) {
                swap(topx[i], topx[j]);
                i++;
                j--;
            }
        }

        return i;
    }

    //Sorts the vector in ascending order using quick sort
    void qsortVector(int left, int right) {
        if (left < right) {
            // Choose a pivot and partition the array
            int pivotIndex = left + (right - left) / 2;
            int pivot = topx[pivotIndex].second;

            // Partition the array
            int newPivotIndex = Partition(left, right, pivot);

            // Recursively sort the left and right sub-arrays
            qsortVector(left, newPivotIndex - 1);
            qsortVector(newPivotIndex + 1, right);
        }
    }

 public:
    //Write data in an unordered_map from .txt file
    void mappifyTXT() {
        string line;
        ifstream words("assets/moreword.txt"); //slower, heavier, but more accurate and is ranked by frequency of use
        //ifstream words("assets/ENGLISH_WORDS.txt"); //faster, lighter, but words are mumbo-jumbo new zealandian and australian quite often
        if (words.is_open()) {
            while (getline(words, line)) {
                rhymeMap[countWords] = line;
                countWords++;
            }
            words.close();
        }
        system("pause");
    }

    void fillVector(int mode) {
        if (mode == 1) {
            //Adding words to a vector from the map, if last syllables are same
            for (int b = 0; b < 3080821; b++) {
                if (rhymeMap[b] != input) {
                    if (check_syllables_end(rhymeMap[b], 1)) {
                        pair<string, int> pusher;
                        pusher.first = rhymeMap[b];
                        pusher.second = LevenshteinDistance(rhymeMap[b]);
                        topx.push_back(pusher);
                    }
                }
            }
        }
        if (mode == 2) {
            //Adding words to a vector from the map, if last syllables are similar
            for (int b = 0; b < 3080821; b++) {
                if (rhymeMap[b] != input) {
                    if (check_syllables_end(rhymeMap[b], 2)) {
                        pair<string, int> pusher;
                        pusher.first = rhymeMap[b];
                        pusher.second = LevenshteinDistance(rhymeMap[b]);
                        topx.push_back(pusher);
                    }
                }
            }
        }
    }

    void run() {
        cout << "Base word: ";
        getline(cin, input);
        //Used for checking if last syllables of input and word from database match with each other
        input_lastSyllable = find_last_syllable(input);
        cils = input_lastSyllable;
        cout << "Last syllable of input: " << input_lastSyllable << endl;
        cout << "Number of words in the database: " << countWords << endl;
        fillVector(1);
        if (topx.size() == 0) {
            fillVector(2);
        }
        if (topx.size() > 0)qsortVector(0, topx.size() - 1);
        else cout << "No rhythmical words have been found in our database..." << endl;
        int limit = 100;
        cout << topx.size() << " -amount of words have been sorted" << endl;
        if (topx.size() < 100) {
            limit = topx.size();
        }
        for (int b = 0; b < limit; b++) {
            cout << b + 1 << ": " << topx[b].first << " L.D:" << topx[b].second << endl;
        }
        topx.clear();
        system("pause");
    }
};

int main()
{
    findRhythm findR;
    cout << "Wait before the application loads..." << endl;
    findR.mappifyTXT();

    while (true) {
        findR.run();
        system("cls");
    }
}
