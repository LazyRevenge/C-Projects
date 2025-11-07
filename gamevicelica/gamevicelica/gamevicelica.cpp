#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cctype>

using namespace std;

int main() {
    vector<string> words = { "program", "hangman", "computer", "algorithm", "developer", "coding" };

    srand(static_cast<unsigned int>(time(0)));
    random_shuffle(words.begin(), words.end());
    const string secret_word = words[0];

    string guessed_word(secret_word.length(), '-');
    int wrong_guesses = 0;
    const int max_wrong = 6;  
    string used_letters = "";

    cout << "Welcome to Hangman!\n";
    cout << "Guess the word by entering letters. You have " << max_wrong << " tries.\n";

    while (wrong_guesses < max_wrong && guessed_word != secret_word) {
        cout << "\nYou have " << (max_wrong - wrong_guesses) << " guesses left.\n";
        cout << "Used letters: " << used_letters << endl;
        cout << "Current word: " << guessed_word << endl;

        char guess;
        cout << "Enter a letter: ";
        cin >> guess;
        guess = tolower(guess);  

        if (used_letters.find(guess) != string::npos) {
            cout << "You've already tried '" << guess << "'!\n";
            continue;
        }

        used_letters += guess;

        if (secret_word.find(guess) != string::npos) {
            cout << "Correct! '" << guess << "' is in the word.\n";
            for (int i = 0; i < secret_word.length(); ++i) {
                if (secret_word[i] == guess) {
                    guessed_word[i] = guess;
                }
            }
        }
        else {
            cout << "Oops! '" << guess << "' is not in the word.\n";
            ++wrong_guesses;
        }
    }

    if (wrong_guesses == max_wrong) {
        cout << "\nGame over! The word was: " << secret_word << endl;
    }
    else {
        cout << "\nCongratulations! You guessed the word: " << secret_word << endl;
    }

    return 0;
}