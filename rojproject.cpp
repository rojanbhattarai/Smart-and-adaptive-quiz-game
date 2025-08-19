#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <conio.h>
#include <windows.h>

using namespace std;

// -------- TEMPLATE: generic answer checker --------
template <typename T>
bool checkAnswer(T correct, T user) { return correct == user; }

// -------- Question class --------
class Question {
protected:
    string question, optionA, optionB, optionC, optionD;
    char correctAns;

public:
    Question() : correctAns('A') {}
    Question(const string& q, const string& a, const string& b,
             const string& c, const string& d, char ans)
        : question(q), optionA(a), optionB(b), optionC(c), optionD(d),
          correctAns(toupper(ans)) {}

    void displayQuestion() {
        cout << "\n" << question << "\n";
        cout << "A. " << optionA << "\n";
        cout << "B. " << optionB << "\n";
        cout << "C. " << optionC << "\n";
        cout << "D. " << optionD << "\n";
        cout << "(15s) Enter your answer (A/B/C/D): ";
    }

    char getCorrectAns() const { return correctAns; }
    string getQuestionText() const { return question; }
};

// -------- Player class (operator++) --------
class Player {
    string name;
    int score;
public:
    Player(const string& n="Guest") : name(n), score(0) {}
    void operator++() { ++score; }
    int getScore() const { return score; }
    string getName() const { return name; }
};

// -------- helper: create file if missing --------
void createFileIfMissing(const string& filename, const vector<string>& lines) {
    ifstream in(filename.c_str());
    if (!in) {
        ofstream out(filename.c_str());
        for (auto &line : lines) out << line << "\n";
        out.close();
    }
}

// -------- Concrete Levels --------
void loadQuestions(const string& filename, vector<Question>& questions) {
    ifstream file(filename);
    if (!file) throw runtime_error(filename + " not found!");
    string q,a,b,c,d; char ans;
    while(getline(file,q)) {
        if(!getline(file,a)) break;
        if(!getline(file,b)) break;
        if(!getline(file,c)) break;
        if(!getline(file,d)) break;
        if(!(file >> ans)) break;
        file.ignore(10000,'\n');
        questions.push_back(Question(q,a,b,c,d,ans));
    }
    file.close();
}

// -------- helper: shuffle --------
void fisherYatesShuffle(vector<Question>& v) {
    if(v.empty()) return;
    srand((unsigned)time(NULL));
    for(int i=(int)v.size()-1;i>0;--i){
        int j=rand()%(i+1);
        swap(v[i],v[j]);
    }
}

// -------- helper: read answer with 15s timeout --------
char readAnswerWithTimeout(int seconds) {
    time_t start = time(NULL);
    while(difftime(time(NULL),start)<seconds) {
        if(kbhit()){
            int ch = getch();
            if(ch==0||ch==224){ getch(); continue; }
            ch=toupper(ch);
            if(ch=='A'||ch=='B'||ch=='C'||ch=='D'){
                cout << (char)ch << "\n";
                return (char)ch;
            }
        }
        Sleep(50);
    }
    return '-';
}

// -------- store results --------
struct Result {
    string question;
    char playerAns;
    char correctAns;
};

// -------- Quiz driver --------
class Quiz {
    Player player;
    vector<Result> results;

    vector<Question> easyQuestions, mediumQuestions, hardQuestions;
public:
    Quiz(const Player& p):player(p){
        // Sample questions
        vector<string> easySample={
            "What is the capital of France?","Paris","London","Berlin","Madrid","A",
            "2 + 2 = ?","3","4","5","6","B",
            "Which data type stores decimals in C++?","int","float","char","bool","B",
            "Which is smallest prime number?","0","1","2","3","C",
            "Symbol for addition in C++?","+","-","*","/","A",
            "Which of these is a loop?","for","foreach","repeat","looped","A",
            "Keyword in C++?","var","class","function","int","D"
        };
        createFileIfMissing("easy.txt", easySample);

        vector<string> mediumSample={
            "System programming language?","Python","C++","HTML","CSS","B",
            "Red Planet?","Venus","Mars","Jupiter","Saturn","B",
            "Keyword for inheritance in C++?","super","extends","inherit","public","D",
            "Operator for logical AND?","&","|","&&","||","C",
            "Constructor is called when?","Destructor","Constructor","Main","Friend","B",
            "Dynamic array in STL?","vector","array","stack","queue","A",
            "Access allowing inheritance but hiding data?","public","protected","private","friend","B"
        };
        createFileIfMissing("medium.txt", mediumSample);

        vector<string> hardSample={
            "Who developed relativity?","Newton","Einstein","Tesla","Edison","B",
            "Time complexity of binary search?","O(n)","O(n^2)","O(log n)","O(1)","C",
            "OOP feature to reuse code?","Polymorphism","Encapsulation","Inheritance","Abstraction","C",
            "Keyword preventing inheritance?","final","sealed","private","virtual","A",
            "Loop executing at least once?","for","while","do-while","none","C",
            "STL class storing key-value?","vector","map","set","list","B",
            "Runtime polymorphism?","Function overloading","Function overriding","Operator overloading","Templates","B"
        };
        createFileIfMissing("hard.txt", hardSample);

        loadQuestions("easy.txt", easyQuestions);
        loadQuestions("medium.txt", mediumQuestions);
        loadQuestions("hard.txt", hardQuestions);

        fisherYatesShuffle(easyQuestions);
        fisherYatesShuffle(mediumQuestions);
        fisherYatesShuffle(hardQuestions);
    }

    void startQuiz(){
        cout << "===== Welcome to Smart Adaptive Quiz Game =====\n";
        char currentLevel='E'; // E=easy, M=medium, H=hard
        int totalQuestions = 9; // total to ask in session

        while(totalQuestions-- > 0){
            Question q;
            if(currentLevel=='E' && !easyQuestions.empty()){
                q = easyQuestions.back(); easyQuestions.pop_back();
            } else if(currentLevel=='M' && !mediumQuestions.empty()){
                q = mediumQuestions.back(); mediumQuestions.pop_back();
            } else if(currentLevel=='H' && !hardQuestions.empty()){
                q = hardQuestions.back(); hardQuestions.pop_back();
            } else if(!easyQuestions.empty()){ q=easyQuestions.back(); easyQuestions.pop_back(); currentLevel='E'; }
            else if(!mediumQuestions.empty()){ q=mediumQuestions.back(); mediumQuestions.pop_back(); currentLevel='M'; }
            else if(!hardQuestions.empty()){ q=hardQuestions.back(); hardQuestions.pop_back(); currentLevel='H'; }
            else break;

            q.displayQuestion();
            char ans = readAnswerWithTimeout(15);
            char playerAns = ans;

            if(ans=='-'){ cout<<"\n⏱ Time's up! Skipped.\n"; }
            else if(checkAnswer(q.getCorrectAns(),ans)){
                cout<<"✅ Correct!\n"; ++player;
                // adaptive: move up level if possible
                if(currentLevel=='E') currentLevel='M';
                else if(currentLevel=='M') currentLevel='H';
            } else {
                cout<<"❌ Wrong! Correct: "<<q.getCorrectAns()<<"\n";
                // adaptive: drop level if possible
                if(currentLevel=='H') currentLevel='M';
                else if(currentLevel=='M') currentLevel='E';
            }

            results.push_back({q.getQuestionText(),playerAns,q.getCorrectAns()});
        }

        cout<<"\n🎉 Quiz Finished! 🎉\n";
        if(player.getScore()==9) cout<<"🎊 YOU ARE A CHAMPION! 🎊\n";
        showScoreboard();
    }

    void showScoreboard(){
        cout<<"\n===== SCOREBOARD =====\n";
        int qNum=1,totalCorrect=0;
        for(auto &res:results){
            cout<<qNum++<<". "<<res.question<<"\n";
            cout<<"   Your answer: "<<(res.playerAns=='-'?"Skipped":string(1,res.playerAns))
                <<" | Correct answer: "<<res.correctAns
                <<" | "<<(res.playerAns==res.correctAns?"✅":"❌")<<"\n";
            if(res.playerAns==res.correctAns) ++totalCorrect;
        }
        cout<<"\nTotal Score: "<<player.getScore()<<" | Correct: "<<totalCorrect<<"\n";
        ofstream out("result.txt",ios::app);
        out<<"Player: "<<player.getName()<<" | Final Score: "<<player.getScore()
           <<" | Correct: "<<totalCorrect<<"\n";
        out.close();
    }
};

// -------- main --------
int main(){
    cout<<"Enter your name: ";
    string name; getline(cin,name);
    if(name.empty()) name="Guest";

    Player p(name);
    Quiz q(p);
    q.startQuiz();

    cout<<"\nThanks for playing!\n";
    return 0;
}
