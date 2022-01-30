#include <iostream>
#include <sys/stat.h>
#include <bits/stdc++.h>
using namespace std;

// NEXT STEP: INFO GUESSES! (potentially: do only info guesses until down to 1 word??)

vector<vector<int>> weights;

//checks if a file exists already
bool exists(const std::string& name) {
  struct stat buffer;  
  return (stat (name.c_str(), &buffer) == 0); 
}

// returns a 2D vector of char weights: weights[n][26], the value at weights[i][j] is how many words have the j-th letter at the i-th index in the word
vector<vector<int>> getWeights(int n, vector<string> words) {
  vector<vector<int>> ns;
  for(int i = 0; i < n; i++) {
    ns.push_back({});
    for(int j = 0; j < 26; j++) {
      ns[i].push_back(0);
    }
  }
  for(int i = 0; i < words.size(); i++) {
    string b = words[i];
    for(int j = 0; j < n; j++) {
      int c = int(b.at(j)) - 97;
      ns[j][c] += 1;
    }
  }
  return ns;
}

// scores a word based on the current weights; a duplicate letter adds NO score to the word
int score(string s) {
  unordered_set<char> used;
  int score = 0;
  for(int i = 0; i < s.length(); i++) {
    char c = s.at(i);
    if(used.find(c) == used.end()) {
      score += weights[i][int(c) - 97];
      used.insert(c);
    }
  }
  return score;
}

// checks if a word contains a character
bool contains(string s, char c) {
  for(int i = 0; i < s.length(); i++) {
    if(s.at(i) == c) {
      return true;
    }
  }
  return false;
}

// removes from words vector based on a grey tile result from a guess
vector<string> removeGrey(char c, vector<string> words) {
  vector<int> removalInds;
  for(int i = words.size() - 1; i >= 0; i--) {
    if(contains(words[i], c)) {
      removalInds.push_back(i);
    }
  }
  for(int i = 0; i < removalInds.size(); i++) {
    words.erase(words.begin() + removalInds[i]);
  }
  return words;
}

// removes from words vector based on a yellow tile result from a guess
vector<string> removeYellow(char c, int pos, vector<string> words) {
  vector<int> removalInds;
  for(int i = words.size() - 1; i >= 0; i--) {
    if((not contains(words[i], c)) or words[i].at(pos) == c) {
      removalInds.push_back(i);
    }
  }
  for(int i = 0; i < removalInds.size(); i++) {
    words.erase(words.begin() + removalInds[i]);
  }
  return words;
}

// removes from words vector based on a green tile result from a guess
vector<string> removeGreen(char c, int pos, vector<string> words) {
  vector<int> removalInds;
  for(int i = words.size() - 1; i >= 0; i--) {
    if(words[i].at(pos) != c) {
      removalInds.push_back(i);
    }
  }
  for(int i = 0; i < removalInds.size(); i++) {
    words.erase(words.begin() + removalInds[i]);
  }
  return words;
}

void solve(int n, string s) {
  // get all words of that length into vector
  ifstream getWords(s);
  vector<string> words;
  string temp;
  while(getline(getWords, temp)) {
    words.push_back(temp);
  }
  // each iteration of this while loop is one guess
  vector<bool> isGreen;
  for(int i = 0; i < n; i++) {
    isGreen.push_back(false);
  }
  while(true) {
    weights = getWeights(n, words);
    int maxInd = -1;
    int maxScore = -1;
    // find word with the current highest score in words
    for(int i = 0; i < words.size(); i++) {
      int z = score(words[i]);
      if(z > maxScore) {
        maxInd = i;
        maxScore = z;
      }
    }
    string guess = words[maxInd];
    cout << guess << endl;
    string status;
    cin >> status;
    // clear vector words of ones that it cannot be
    if(status == "win") {
      cout << "YAY!" << endl;
      break;
    }
    for(int i = 0; i < status.length(); i++) {
      char c = status.at(i);
      if(c == '.') {
        words = removeGrey(guess.at(i), words);
      }
      else if(c == 'y') {
        words = removeYellow(guess.at(i), i, words);
      }
      else if(c == 'g') {
        words = removeGreen(guess.at(i), i, words);
        isGreen[i] = true;
      }
    }
    cout << "There are " << words.size() << " possible words remaining." << endl;
  }
}

int main() {
  cout << "Enter number of letters: ";
  int n;
  cin >> n;
  if(n == 1) {
    cout << "This solver does not work for single-letter words, please enter a different value." << endl;
    return 0;
  }
  // if there is not already, make document for that length of word
  string s = to_string(n) + ".txt";
  if(not exists(s)) {
    ofstream nFile(s);
    ifstream theWords("words.txt");
    string lastWord;
    while(getline(theWords, lastWord)) {
      if(lastWord.length() == n) {
        nFile << lastWord << endl;
      }
    }
  }
  solve(n, s);
}
/*int main() {
  ifstream wordFile("words.txt");
  ofstream newOne("wordsNew.txt");
  string tem;
  while(getline(wordFile, tem)) {
    std::transform(tem.begin(), tem.end(), tem.begin(), [](unsigned char c){ return std::tolower(c); });
    newOne << tem << endl;
  }
}*/