/*
=================================================@@@@@@@@@@@@=
CS1002 Final Lab Project: Console-Based Quiz Game
=@@@@@@@@@@@@=================================================

DESCRIPTION:

A fully interactive console quiz game with multiple features:

    - 5 categories (Science, Computer, Sports, History, IQ)
    - 3 difficulty levels affecting scoring (Easy/Medium/Hard)
    - 4 lifelines: 50/50, Skip, Replace, Extra Time
    - Streak bonus system (+5 for 3 correct,+15 for 5 correct)
    - Negative marking based on difficulty
    - Timer-based gameplay
    - High score tracking with file persistence
    - Session logging and progress saving
    - Question addition feature for users

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

TEAM MEMBERS:
    Mahida Nayyar - 25F-0795
    Sawera Khalid - 25F-0792
    Tayyba Fatima - 25F-0780

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

SECTION: A
DATE: 9-12-2025

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cstring>
using namespace std;

// Global Var Declaration
const int Max_questions = 500;
const int Max_wrong = 100;

string questions[Max_questions];
string optA[Max_questions], optB[Max_questions], optC[Max_questions], optD[Max_questions];
char answers[Max_questions];
string categories[Max_questions];
int difficulties[Max_questions];
bool answeredCorrectly[Max_questions];
int questionCount = 0;

int quizQuestions[10];  
int quizCount = 0;
int currentQuestionPos = 0;

string wrongQuestions[Max_wrong];
string wrongOptA[Max_wrong], wrongOptB[Max_wrong], wrongOptC[Max_wrong], wrongOptD[Max_wrong];
char wrongAnswers[Max_wrong];
int wrongCount = 0;

string playerName;
int playerScore = 0;
string playerCategory;
time_t quizStartTime;

int streakCount = 0;
bool timerExpired = false;

bool lifeline50 = true;
bool lifelineSkip = true;
bool lifelineReplace = true;
bool lifelineTime = true;

// Declaration of Func
void clearScreen();
void showMainMenu();
void startNewQuiz();
void loadQuestions(string category, int difficulty);
void shuffleQuestions();
void shuffleOptions(int qIndex);
void askQuestion(int qPos);
void checkAnswer(char userAns, int qIndex);
void useLifeline(int& qPos);
void showCategoryMenu();
void showDifficultyMenu();
void viewHighScores();
void saveHighScore();
void saveQuizLog();
void reviewWrongAnswers();
void addQuestionSuggestion();
void showScoreSummary();
void waitForEnter();
string getTime();

// For clearing screen
void clearScreen() {
    system("cls");
}

string getTime() {
    time_t now = time(0);
    char buffer[26];          
    ctime_s(buffer, sizeof(buffer), &now);
    string dt(buffer);
    if (!dt.empty() && dt.back() == '\n') {
        dt.pop_back();         
    }
    return dt;
}

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void swapInt(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

void swapString(string& a, string& b) {
    string temp = a;
    a = b;
    b = temp;
}

void swapChar(char& a, char& b) {
    char temp = a;
    a = b;
    b = temp;
}

void swapBool(bool& a, bool& b) {
    bool temp = a;
    a = b;
    b = temp;
}

// Main Function
int main() {
    srand(time(0));

    clearScreen();
    cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
    cout << "     WELCOME TO CONSOLE QUIZ GAME\n";
    cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n";
    
    cout << "Enter your name: ";
    getline(cin, playerName);

    char choice;
    do {
        showMainMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case '1': {
            startNewQuiz();
            break;
        }
        case '2': {
            viewHighScores();
            break;
        }
        case '3': {
            reviewWrongAnswers();
            break;
        }
        case '4': {
            addQuestionSuggestion();
            break;
        }
        case '5': {
            cout << "\nThank you for playing!" << endl;
            break;
        }
        default: {
            cout << "\nInvalid choice!\n";
        }
        }

        if (choice != '5') waitForEnter();

    } while (choice != '5');

    return 0;
}

// Menu Function
void showMainMenu() {
    clearScreen();
    cout << "============================@@@@@@@@@@@@=" << endl;
    cout << "                MAIN MENU                " << endl;
    cout << "=@@@@@@@@@@@@============================" << endl;
    cout << endl;
    cout << "Player Name: " << playerName << endl;
    if (playerScore > 0) {
        cout << "Last Score: " << playerScore << endl;
    }
    cout << ".........................................." << endl;
    cout << "1. Start New Quiz" << endl;
    cout << "2. View High Scores" << endl;
    cout << "3. Review Wrong Answers" << endl;
    cout << "4. Suggest New Question" << endl;
    cout << "5. Exit Game" << endl;
    cout << ".........................................." << endl;
    cout << "Enter choice: ";
}

void showCategoryMenu() {
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
    cout << "          SELECT CATEGORY" << endl;
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
    cout << "1. Science\n2. Computer\n3. Sports\n";
    cout << "4. History\n5. IQ/Logic\n6. Mixed\n";
    cout << "..........................................\n";
    cout << "Enter category (1-6): ";
}

void showDifficultyMenu() {
    cout << "\n++++++++++++++++++++++++++++++++++++++++++\n";
    cout << "          SELECT DIFFICULTY" << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++++\n";
    cout << "1. Easy (-2 for wrong)" << endl;
    cout << "2. Medium (-3 for wrong)" << endl;
    cout << "3. Hard (-5 for wrong)" << endl;
    cout << "..........................................\n";
    cout << "Enter difficulty (1-3): ";
}

// Loading Questions from text files
void loadQuestions(string category, int difficulty) {
    questionCount = 0;
    string files[5] = { "science.txt", "computer.txt", "sports.txt", "history.txt", "iq.txt" };
    int fileCount = 1;

    if (category == "Mixed") {
        fileCount = 5;
    }
    else {
        files[0] = category + ".txt";
    }

    for (int f = 0; f < fileCount && questionCount < Max_questions; f++) {
        ifstream file(files[f]);
        if (!file) {
            cout << "Note: Could not open " << files[f] << endl;
            continue;
        }

        string line;
        while (getline(file, line) && questionCount < Max_questions) {
            if (line.empty()) continue;

            questions[questionCount] = line;

            getline(file, line);
            optA[questionCount] = line;

            getline(file, line);
            optB[questionCount] = line;

            getline(file, line);
            optC[questionCount] = line;

            getline(file, line);
            optD[questionCount] = line;

            getline(file, line);
            answers[questionCount] = toupper(line[0]);

            categories[questionCount] = category;
            difficulties[questionCount] = difficulty;
            answeredCorrectly[questionCount] = false;

            questionCount++;
        }
        file.close();
    }

    // Filter by difficulty
    if (category != "Mixed") {
        string tempQ[Max_questions];
        string tempA[Max_questions], tempB[Max_questions], tempC[Max_questions], tempD[Max_questions];
        char tempAns[Max_questions];
        string tempCat[Max_questions];
        int tempDiff[Max_questions];
        bool tempCorr[Max_questions];
        int tempCount = 0;

        for (int i = 0; i < questionCount; i++) {
            if (difficulties[i] == difficulty) {
                tempQ[tempCount] = questions[i];
                tempA[tempCount] = optA[i];
                tempB[tempCount] = optB[i];
                tempC[tempCount] = optC[i];
                tempD[tempCount] = optD[i];
                tempAns[tempCount] = answers[i];
                tempCat[tempCount] = categories[i];
                tempDiff[tempCount] = difficulties[i];
                tempCorr[tempCount] = answeredCorrectly[i];
                tempCount++;
            }
        }

        // Copy back
        questionCount = tempCount;
        for (int i = 0; i < questionCount; i++) {
            questions[i] = tempQ[i];
            optA[i] = tempA[i];
            optB[i] = tempB[i];
            optC[i] = tempC[i];
            optD[i] = tempD[i];
            answers[i] = tempAns[i];
            categories[i] = tempCat[i];
            difficulties[i] = tempDiff[i];
            answeredCorrectly[i] = tempCorr[i];
        }
    }
}

// Randomizing
void shuffleQuestions() {
    // Creates index array
    int indices[Max_questions];
    for (int i = 0; i < questionCount; i++) {
        indices[i] = i;
    }

    // Shuffle indices
    for (int i = questionCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swapInt(indices[i], indices[j]);
    }

    // Select 10 questions
    quizCount = (questionCount < 10) ? questionCount : 10;
    for (int i = 0; i < quizCount; i++) {
        quizQuestions[i] = indices[i];
        shuffleOptions(quizQuestions[i]);
    }
}

void shuffleOptions(int qIndex) {
    //Save correct option
    char correct = answers[qIndex];
    string correctText;

    if (correct == 'A') {
        correctText = optA[qIndex];
    }
    else if (correct == 'B') {
        correctText = optB[qIndex];
    }
    else if (correct == 'C') {
        correctText = optC[qIndex];
    }
    else {
        correctText = optD[qIndex];
    }

    //Shuffle Options
    string options[4] = { optA[qIndex], optB[qIndex], optC[qIndex], optD[qIndex] };
    int order[4] = { 0,1,2,3 };

    //Random Shuffle
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        swapInt(order[i], order[j]);
    }

    // Update arrays with new order
    optA[qIndex] = options[order[0]];
    optB[qIndex] = options[order[1]];
    optC[qIndex] = options[order[2]];
    optD[qIndex] = options[order[3]];

    // Update correct answer
    for (int i = 0; i < 4; i++) {
        if (options[order[i]] == correctText) {
            answers[qIndex] = 'A' + i;
            break;
        }
    }
}

// Option 1 - Start New Quiz
void startNewQuiz() {
    clearScreen();

    // Reset previous values
    playerScore = 0;
    streakCount = 0;
    currentQuestionPos = 0;
    wrongCount = 0;
    lifeline50 = lifelineSkip = lifelineReplace = lifelineTime = true;
    quizStartTime = time(0);

    // Select Category
    showCategoryMenu();
    int catChoice;
    cin >> catChoice;
    cin.ignore();

    string category;
    switch (catChoice) {
    case 1: category = "Science"; break;
    case 2: category = "Computer"; break;
    case 3: category = "Sports"; break;
    case 4: category = "History"; break;
    case 5: category = "IQ"; break;
    case 6: category = "Mixed"; break;
    default: category = "Science"; break;
    }

    //Select Difficulty
    showDifficultyMenu();
    int diffChoice;
    cin >> diffChoice;
    cin.ignore();

    //Load and Setup Questions
    loadQuestions(category, diffChoice);
    if (questionCount == 0) {
        cout << "\nNo question found!" << endl;
        return;
    }

    shuffleQuestions();
    playerCategory = category;

    //Ask Questions
    for (currentQuestionPos = 0; currentQuestionPos < quizCount; ) {
        clearScreen();
        cout << "(((((((******************)))))))" << endl;
        cout << "        QUESTION " << (currentQuestionPos + 1) << " of " << quizCount << endl;
        cout << "(((((((******************)))))))" << endl;
        cout << endl;
        cout << "Category: " << category << endl;
        cout << "Score: " << playerScore << " | Streak: " << streakCount << endl;
        cout << "---------------------------------------" << endl;
        cout << endl;

        // Add a flag to track if we should skip to next question
        bool askNextQuestion = true;

        askQuestion(currentQuestionPos);

        // Only increment if we didn't skip or replace
        currentQuestionPos++;

        if (currentQuestionPos < quizCount) {
            waitForEnter();
        }
    }
    // Quiz complete
    showScoreSummary();
    saveHighScore();
    saveQuizLog();

    cout << "\nReview wrong answers? (Y/N): ";
    char review;
    cin >> review;
    cin.ignore();

    if (toupper(review) == 'Y') {
        reviewWrongAnswers();
    }
}

void askQuestion(int qPos) {
    int qIndex = quizQuestions[qPos];

    cout << questions[qIndex] << endl;
    cout << endl;
    cout << "A) " << optA[qIndex] << endl;
    cout << "B) " << optB[qIndex] << endl;
    cout << "C) " << optC[qIndex] << endl;
    cout << "D) " << optD[qIndex] << endl;

    //Show Lifelines
    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "Lifelines: ";
    if (lifeline50) cout << "1=50/50 ";
    if (lifelineSkip) cout << "2=Skip ";
    if (lifelineReplace) cout << "3=Replace ";
    if (lifelineTime) cout << "4=+10sec ";
    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    //Time Simulator
    cout << "\nYou have 15 seconds to answer!\n";

    // Start timer
    time_t startTime = time(NULL);
    time_t currentTime;

    char answer;
    bool answered = false;
    bool skipped = false;

    // Ask until user enters OR time expires
    while (!answered && !skipped) {
        cout << "\nEnter answer (A/B/C/D) or L for lifeline: ";
        cin >> answer;
        answer = toupper(answer);

        // Time after answer
        currentTime = time(NULL);
        int elapsed = currentTime - startTime;

        if (elapsed > 15) {
            cout << "\nTime's up! You took " << elapsed << " seconds.\n";
            cout << "Correct answer was: " << answers[qIndex] << endl;

            // Record wrong answer
            if (wrongCount < Max_wrong) {
                wrongQuestions[wrongCount] = questions[qIndex];
                wrongOptA[wrongCount] = optA[qIndex];
                wrongOptB[wrongCount] = optB[qIndex];
                wrongOptC[wrongCount] = optC[qIndex];
                wrongOptD[wrongCount] = optD[qIndex];
                wrongAnswers[wrongCount] = answers[qIndex];
                wrongCount++;
            }

            // Deduct penalty
            int penalty = (difficulties[qIndex] == 1) ? 2 :
                (difficulties[qIndex] == 2) ? 3 : 5;

            playerScore -= penalty;
            cout << "Penalty: -" << penalty << " points\n";
            streakCount = 0;

            answered = true;
            return;  // Move to next question
        }

        // If L → lifeline
        if (answer == 'L') {
            useLifeline(qPos);
            return;
        }

        // Valid answer
        checkAnswer(answer, qIndex);
        answered = true;
    }
}

void checkAnswer(char userAns, int qIndex) {
    if (userAns == answers[qIndex]) {
        cout << "\nCorrect! +10 Points" << endl;
        playerScore += 10;
        streakCount++;
        answeredCorrectly[qIndex] = true;

        if (streakCount == 3) {
            cout << "3-in-a-row! +5 Bonus" << endl;
            playerScore += 5;
        }
        else if (streakCount >= 5) {
            cout << streakCount << "-in-a-row! +15 Bonus" << endl;
            playerScore += 15;
        }
    }
    else {
        cout << "Wrong! Answer was " << answers[qIndex] << endl;

        //Save Wrong Answers
        if (wrongCount < Max_wrong) {
            wrongQuestions[wrongCount] = questions[qIndex];
            wrongOptA[wrongCount] = optA[qIndex];
            wrongOptB[wrongCount] = optB[qIndex];
            wrongOptC[wrongCount] = optC[qIndex];
            wrongOptD[wrongCount] = optD[qIndex];
            wrongAnswers[wrongCount] = answers[qIndex];
            wrongCount++;
        }

        //Apply Penalty
        int penalty = 0;
        if (difficulties[qIndex] == 1) penalty = 2;
        else if (difficulties[qIndex] == 2) penalty = 3;
        else penalty = 5;

        playerScore -= penalty;
        cout << "Penalty: -" << penalty << " points\n";
        streakCount = 0;
    }

}

// Lifelines
void useLifeline(int& qPos) {
    clearScreen();
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~<<<<<<<<<<\n";
    cout << "            LIFELINES\n";
    cout << ">>>>>>>>>>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    int choice;
    cout << "1. 50/50 (remove 2 wrong)\n";
    cout << "2. Skip question\n";
    cout << "3. Replace question\n";
    cout << "4. Extra time (+10 sec)\n";
    cout << "5. Cancel\n";
    cout << "Choice: ";
    cin >> choice;
    cin.ignore();

    if (choice == 1 && lifeline50) {
        lifeline50 = false;
        int qIndex = quizQuestions[qPos];
        char correct = answers[qIndex];

        cout << "\n50/50 Activated!\n";
        cout << "Options removed: ";

        char hide1, hide2;
        if (correct == 'A') { hide1 = 'B'; hide2 = 'D'; }
        else if (correct == 'B') { hide1 = 'A'; hide2 = 'C'; }
        else if (correct == 'C') { hide1 = 'B'; hide2 = 'D'; }
        else { hide1 = 'A'; hide2 = 'C'; }

        cout << hide1 << " and " << hide2 << "\n\n";

        cout << questions[qIndex] << "\n\n";

        // Create display versions with [HIDDEN] for wrong options
        string dispA = optA[qIndex];
        string dispB = optB[qIndex];
        string dispC = optC[qIndex];
        string dispD = optD[qIndex];

        if (correct != 'A' && (hide1 == 'A' || hide2 == 'A')) dispA = "[HIDDEN]";
        if (correct != 'B' && (hide1 == 'B' || hide2 == 'B')) dispB = "[HIDDEN]";
        if (correct != 'C' && (hide1 == 'C' || hide2 == 'C')) dispC = "[HIDDEN]";
        if (correct != 'D' && (hide1 == 'D' || hide2 == 'D')) dispD = "[HIDDEN]";

        cout << "A) " << dispA << endl;
        cout << "B) " << dispB << endl;
        cout << "C) " << dispC << endl;
        cout << "D) " << dispD << endl;

        // IMPORTANT: Get user answer AFTER showing 50/50 options
        char ans;
        cout << "\nEnter answer (A/B/C/D): ";
        cin >> ans;
        ans = toupper(ans);

        // Check the answer
        checkAnswer(ans, qIndex);

        return;
    }
    else if (choice == 2 && lifelineSkip) {
        lifelineSkip = false;
        cout << "\nQuestion skipped! No penalty.\n";
        // Don't increment qPos here - let the calling function handle it
        // Just mark this question as handled
        return;
    }
    else if (choice == 3 && lifelineReplace) {
        lifelineReplace = false;
        cout << "\nFinding replacement question...\n";

        // Find a new question not in current quiz
        bool found = false;
        for (int i = 0; i < questionCount && !found; i++) {
            bool inQuiz = false;
            for (int j = 0; j < quizCount; j++) {
                if (quizQuestions[j] == i) {
                    inQuiz = true;
                    break;
                }
            }
            if (!inQuiz) {
                quizQuestions[qPos] = i;
                shuffleOptions(i);
                found = true;
            }
        }

        if (found) {
            cout << "Question replaced!\n";
            // Ask the SAME question position again with new question
            currentQuestionPos--; // Decrement so we ask this position again
        }
        else {
            cout << "No replacement available.\n";
        }
        return;
    }
    else if (choice == 4 && lifelineTime) {
        lifelineTime = false;
        cout << "\nExtra time added! (Simulated)\n";
        waitForEnter();
        return;
    }
    else if (choice == 5) {
        return;
    }
}
//  Scores
void showScoreSummary() {
    clearScreen();
    cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
    cout << "|  QUIZ COMPLETED! HERE'S THE SUMMARY  |\n";
    cout << "----------------------------------------\n";
    cout << "Player name: " << playerName << endl;
    cout << "Quiz Category: " << playerCategory << endl;
    cout << "Score: " << playerScore << endl;
    cout << "Questions: " << quizCount << endl;
    cout << "Wrong Answers: " << wrongCount << endl;
    cout << "Date and time: " << getTime() << endl;
    cout << "----------------------------------------\n";
}

void saveHighScore() {
    ofstream file("high_scores.txt", ios::app);
    if (file) {
        file << playerName << " | " << playerScore << " | "
            << playerCategory << " | " << getTime() << endl;
        file.close();
    }
}

void saveQuizLog() {
    ofstream file("quiz_logs.txt", ios::app);
    if (file) {
        file << "=== QUIZ SESSION ===\n";
        file << "Player: " << playerName << endl;
        file << "Date: " << getTime() << endl;
        file << "Category: " << playerCategory << endl;
        file << "Score: " << playerScore << endl;
        file << "Questions: " << quizCount << endl;
        file << "Wrong: " << wrongCount << endl;
        file << "-------------------\n";
        file.close();
    }
}

void viewHighScores() {
    clearScreen();
    cout << "*************************************\n";
    cout << "*          HIGH SCORES              *\n";
    cout << "*************************************\n";
    cout << endl;

    ifstream file("high_scores.txt");
    if (!file) {
        cout << "No scores yet!\n";
        return;
    }

    string line;
    string names[100];
    int scores[100];
    string cats[100];
    string dates[100];
    int count = 0;

    // Read cores
    while (getline(file, line) && count < 100) {
        int first = line.find(" | ");
        int second = line.find(" | ", first + 3);
        int third = line.find(" | ", second + 3);

        if (first != string::npos && second != string::npos && third != string::npos) {
            names[count] = line.substr(0, first);
            scores[count] = stoi(line.substr(first + 3, second - first - 3));
            cats[count] = line.substr(second + 3, third - second - 3);
            dates[count] = line.substr(third + 3);
            count++;
        }
    }

    file.close();


    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j] < scores[j + 1]) {
                swapInt(scores[j], scores[j + 1]);
                swapString(names[j], names[j + 1]);
                swapString(cats[j], cats[j + 1]);
                swapString(dates[j], dates[j + 1]);
            }
        }
    }

    // to Show top 5
    int show = (count < 5) ? count : 5;
    for (int i = 0; i < show; i++) {
        cout << i + 1 << ".  " << names[i] << " - " << scores[i]
            << " (" << cats[i] << ") " << dates[i].substr(0, 10) << endl;
    }

    if (count == 0) cout << "No scores yet!\n";
}


void reviewWrongAnswers() {
    clearScreen();
    if (wrongCount == 0) {
        cout << "No wrong answers to review!\n";
        return;
    }

    cout << "~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~\n";
    cout << "{       REVIEW WRONG ANSWERS            }\n";
    cout << "~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~`~\n";
    cout << "Total wrong: " << wrongCount << "\n\n";

    for (int i = 0; i < wrongCount; i++) {
        cout << "Question " << (i + 1) << ":\n";
        cout << wrongQuestions[i] << "\n\n";
        cout << "A) " << wrongOptA[i] << (wrongAnswers[i] == 'A' ? " ✓" : "") << endl;
        cout << "B) " << wrongOptB[i] << (wrongAnswers[i] == 'B' ? " ✓" : "") << endl;
        cout << "C) " << wrongOptC[i] << (wrongAnswers[i] == 'C' ? " ✓" : "") << endl;
        cout << "D) " << wrongOptD[i] << (wrongAnswers[i] == 'D' ? " ✓" : "") << endl;

        cout << "\n-----------------------------------------\n";
        if (i < wrongCount - 1) waitForEnter();
        clearScreen();
    }
}

void addQuestionSuggestion() {
    clearScreen();
    cout << "                 *===================\n";
    cout << "    SUGGEST NEW QUESTION\n";
    cout << "===================*                 \n";

    ofstream file("suggestions.txt", ios::app);
    if (!file) {
        cout << "Error opening file!\n";
        return;
    }

    string q, a, b, c, d, ans, cat;
    int diff;

    cout << "Category (Science/Computer/Sports/History/IQ): ";
    getline(cin, cat);

    cout << "Difficulty (1-Easy, 2-Medium, 3-Hard): ";
    cin >> diff;
    cin.ignore();

    cout << "Question: ";
    getline(cin, q);

    cout << "Option A: ";
    getline(cin, a);
    cout << "Option B: ";
    getline(cin, b);
    cout << "Option C: ";
    getline(cin, c);
    cout << "Option D: ";
    getline(cin, d);

    cout << "Correct answer (A/B/C/D): ";
    getline(cin, ans);

    // Save to file
    file << "Category: " << cat << endl;
    file << "Difficulty: " << diff << endl;
    file << "Question: " << q << endl;
    file << "A: " << a << endl;
    file << "B: " << b << endl;
    file << "C: " << c << endl;
    file << "D: " << d << endl;
    file << "Answer: " << ans << endl;
    file << "Suggested by: " << playerName << endl;
    file << "Date: " << getTime() << endl;
    file << "---\n";

    file.close();
    cout << "\nThank you! Suggestion saved." << endl;
}