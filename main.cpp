#include <iostream>
#include <sys/stat.h>
#include <bits/stdc++.h>
using namespace std;

// NEXT STEP: INFO GUESSES! (potentially: do only info guesses until down to 1 word??)
// SOMETHINGS IS BROKEN?? w/ info weghts + guesses
// https://www.devangthakkar.com/wordle_archive/?204

vector<vector<int>> weights;
vector<vector<int>> infoWeights;
vector<string> words;
vector<string> allWords;

//checks if a file exists already
bool exists(const std::string& name) {
  struct stat buffer;  
  return (stat (name.c_str(), &buffer) == 0); 
}

// returns a 2D vector of char weights: weights[n][26], the value at weights[i][j] is how many words have the j-th letter at the i-th index in the word
vector<vector<int>> getWeights(int n, bool isInfo, vector<bool> greens) {
  vector<vector<int>> ns;
  for(int i = 0; i < n; i++) {
    ns.push_back({});
    for(int j = 0; j < 26; j++) {
      ns[i].push_back(0);
    }
  }
  for(int i = 0; i < words.size(); i++) {
    string b;
    b = words[i];
    for(int j = 0; j < n; j++) {
      int c = int(b.at(j)) - 97;
      if(not(isInfo and (greens[i] or infoWeights[j][c] == -1))) {
        ns[j][c] += 1;
      }
    }
  }
  return ns;
}

// (for info guesses) finds the weights of that letter in all OTHER positions than i
int getOthers(char c, int i, int n) {
  int count = 0;
  int charNum = int(c) - 97;
  for(int j = 0; j < n; j++) {
    if(j != i and infoWeights[j][charNum] != -1) {
      count += infoWeights[j][charNum];
    }
  }
  return count;
}

// scores a word based on the current weights; a duplicate letter adds NO score to the word
int score(string s, bool isInfo) {
  unordered_set<char> used;
  int score = 0;
  for(int i = 0; i < s.length(); i++) {
    char c = s.at(i);
    if(used.find(c) == used.end()) {
      if(isInfo) {
        int a = infoWeights[i][int(c) - 97];
        if(a != -1) {
          score += a;
          score += (0.3 * getOthers(c, i, s.length()));
        }
      }
      else {
        score += weights[i][int(c) - 97];
      }
      used.insert(c);
    }
    else {
      return 0;
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
vector<string> removeGrey(char c) {
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
vector<string> removeYellow(char c, int pos) {
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
vector<string> removeGreen(char c, int pos) {
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

int countGreens(vector<bool> bs) {
  int count = 0;
  for(bool b : bs) {
    if(b) {
      count += 1;
    }
  }
  return count;
}

// sets the (info) weight of all letters at a position to 0
vector<vector<int>> allZero(int i) {
  for(int j = 0; j < 26; j++) {
    infoWeights[i][j] = -1;
  }
  return infoWeights;
}

// sets  the (info) weight of all positions of a certain letter to 0
vector<vector<int>> zeroLetter(int n, char c) {
  int val = int(c) - 97;
  for(int i = 0; i < n; i++) {
    infoWeights[i][val] = -1;
  }
  return infoWeights;
}

bool shouldInfo(int usedGuesses) {
  if(words.size() < (6 - usedGuesses)) {
    return false;
  }
  return(words.size() != 1);
}

void solve(int n, string s) {
  for(int i = 0; i < n; i++) {
    infoWeights.push_back({});
    for(int j = 0; j < 26; j++) {
      infoWeights[i].push_back(0);
    }
  }
  // get all words of that length into vector
  ifstream getWords(s);
  string temp;
  while(getline(getWords, temp)) {
    words.push_back(temp);
    allWords.push_back(temp);
  }
  // each iteration of this while loop is one guess
  vector<bool> isGreen;
  for(int i = 0; i < n; i++) {
    isGreen.push_back(false);
  }
  bool isInfoGuess = false;
  //cout << "starting loop" << endl;
  int guesses = 0;
  while(true) {
    isInfoGuess = false;
    string guess;
    if(shouldInfo(guesses)) {
      //cout << "chose info" << endl;
      isInfoGuess = true;
      infoWeights = getWeights(n, true, isGreen);
      int maxInd = -1;
      int maxScore = -1;
      //cout << "starts for loop" << endl;
      // find word with the current highest score in words
      for(int i = 0; i < allWords.size(); i++) {
        int z = score(allWords[i], true);
        if(z > maxScore) {
          maxInd = i;
          maxScore = z;
        }
      }
      guess = allWords[maxInd];
      //cout << "got info guess" << endl;
    }
    else {
      weights = getWeights(n, false, isGreen);
      int maxInd = -1;
      int maxScore = -1;
      // find word with the current highest score in words
      for(int i = 0; i < words.size(); i++) {
        int z = score(words[i], false);
        if(z > maxScore) {
          maxInd = i;
          maxScore = z;
        }
      }
      guess = words[maxInd];
    }
    cout << guess << endl;
    guesses += 1;
    string status;
    cin >> status;
    // clear vector words of ones that it cannot be
    if(status == "win") {
      cout << "YAY! We won the wordle in " << guesses << " guesses!" << endl;
      break;
    }
    for(int i = 0; i < status.length(); i++) {
      char c = status.at(i);
      if(c == '.') {
        words = removeGrey(guess.at(i));
        infoWeights = zeroLetter(n, guess.at(i));
      }
      else if(c == 'y') {
        words = removeYellow(guess.at(i), i);
      }
      else if(c == 'g') {
        words = removeGreen(guess.at(i), i);
        isGreen[i] = true;
        infoWeights = allZero(i);
      }
    }
    cout << "There are " << words.size() << " possible words remaining." << endl;
    /*if(words.size() <= 10) {
      for(int i = 0; i < words.size(); i++) {
        cout << words[i] << ", ";
      }
    }
    cout << endl;*/
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