// main.cpp
// Project UID db1f506d06d84ab787baf250c265e24e
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <set>
#include "csvstream.h"
#include <math.h> 


using namespace std;
const double epsilon = 0.00001;


set<string> unique_words(const string& str) {
    istringstream source(str);
    set<string> words;
    string word;
    // Read word by word from the stringstream and insert into the set
    while (source >> word) {
        words.insert(word);
    }
    return words;
}

double LogCalc(int val1, int val2) {

    return log((double)val1 / (double)val2);

}

class Classifier {
public:
    Classifier(bool d);

    int GetNumberOfPosts();
    int GetNumberOfPostsPerLabel(string l);
    int GetVocSize();
    set<string> GetLabels();
    void TrainOnPosts(string csv_file);
    void ComputeLogProbCore();
    void AddNewLabel(string l);
    void AddWordsFromPost(string content, string l);
    void PopulateTrainingSet(string all);
    void PredictProb(string test_file_name);

private:
    //unique labels in the training set 
    set<string> labels;
    //unique words in the entire training set
    set<string> un_words;
    // to record the posts for each label C
    map<string, int> n_of_examples;
    //to record the posts for each word w 
    map<string, int> w_n_of_posts;
    //all the posts for a label 
    map<string, string> w_tag;
    //to record the posts for each label C and word w
    map<pair<string, string>, int> l_w_map1;
    map<pair<string, string>, double> log_lklhd;
    map<string, double> log_prior;

    bool debug_mode;
};

Classifier::Classifier(bool d) {
    debug_mode = d;
}

set<string> Classifier::GetLabels() {
    return labels;

}

int Classifier::GetNumberOfPostsPerLabel(string l) {

    return n_of_examples[l];

}

int Classifier::GetNumberOfPosts() {

    int n_posts = 0;
    for (auto l : GetLabels()) {
        n_posts += GetNumberOfPostsPerLabel(l);
    }

    return n_posts;
}

int Classifier::GetVocSize() {

    return un_words.size();
}
void Classifier::AddNewLabel(string l) {

    labels.insert(l);
    n_of_examples[l]++;

}

void Classifier::AddWordsFromPost(string content, string l) {

    set<string> all_s = unique_words(content);

    for (auto w_p : all_s) {
        w_n_of_posts[w_p]++;
        l_w_map1[{l, w_p}]++;
    }

    w_tag[l] += content;
    w_tag[l] += " ";

    if (debug_mode) {
        string rep_str = "  label = " + l + ", ";
        cout << rep_str;

        rep_str = "content = " + content;
        cout << rep_str << endl;
    }

}

void Classifier::PopulateTrainingSet(string all) {
    un_words = unique_words(all);
}

void Classifier::PredictProb(string test_file_name) {

    csvstream csvin(test_file_name);

    map<string, string> row;
    string t_tag, t_content;
    string winner, rep_str;

    cout << "test data:" << endl;

    int n_test_rows = 0,
        n_corr_pred = 0;
    double log_prob_score = 0.0,
        max_prob_score = 0.0;
    int n_posts = GetNumberOfPosts();

    while (csvin >> row) {
        n_test_rows++;
        t_tag = row["tag"];
        t_content = row["content"];

        max_prob_score = 0.0;
        int l_idx = 0;

        for (auto l : GetLabels()) {
            log_prob_score = log_prior[l];
            for (auto w_p : unique_words(t_content)) {
                if (l_w_map1.count({ l, w_p }) != 0) {
                    log_prob_score += log_lklhd[{l, w_p}];
                }
                else if (un_words.count(w_p) != 0) {
                    log_prob_score += LogCalc(w_n_of_posts[w_p], n_posts);
                }
                else {
                    log_prob_score += LogCalc(1, n_posts);
                }
            }
            if (l_idx == 0 || max_prob_score < log_prob_score) {
                max_prob_score = log_prob_score;

                winner = l;
            }
            else if (abs(max_prob_score - log_prob_score) < epsilon) {
                // its a tie but do nothing, since we are working with 
                // a set , the labels are already alphabetically sorted
                ;
            }

            l_idx++;
        }
        if (t_tag == winner) {
            n_corr_pred++;
        }

        rep_str = "  correct = " + t_tag + ", predicted = " + winner +
            ", log-probability score = ";
        cout << rep_str;
        cout << max_prob_score << endl;
        rep_str = "  content = " + t_content + "\n";
        cout << rep_str << endl;
    }

    rep_str = "performance: " + to_string(n_corr_pred) + " / " +
        to_string(n_test_rows) + " posts predicted correctly";
    cout << rep_str << endl;
}
void Classifier::TrainOnPosts(string csv_file) {

    csvstream csvin(csv_file);

    map<string, string> row;
    string tag, content;
    string all, rep_str;

    if (debug_mode) {
        cout << "training data:" << endl;
    }
    // code should ignore all but the “tag” and “content” columns
    while (csvin >> row) {
        tag = row["tag"];
        content = row["content"];
        AddNewLabel(tag);

        all += content;
        all += " ";
        AddWordsFromPost(content, tag);
    }
    rep_str = "trained on " + to_string(GetNumberOfPosts()) + " examples";
    cout << rep_str << endl;

    PopulateTrainingSet(all);

    if (debug_mode) {
        rep_str = "vocabulary size = " + to_string(GetVocSize()) + "\n";
        cout << rep_str << endl;
    }

    ComputeLogProbCore();
}

void Classifier::ComputeLogProbCore() {

    if (debug_mode) {
        cout << "classes:" << endl;
    }

    int n_posts = GetNumberOfPosts();
    string rep_str;

    for (auto t : GetLabels()) {
        log_prior[t] = LogCalc(n_of_examples[t], n_posts);

        if (debug_mode) {
            rep_str = "  " + t + ", " + to_string(n_of_examples[t]) +
                " examples, " + "log-prior = ";
            cout << rep_str;
            cout << log_prior[t] << endl;
        }
    }

    if (debug_mode) {
        cout << "classifier parameters:" << endl;
    }

    for (auto t : GetLabels()) {
        for (auto w : unique_words(w_tag[t])) {
            log_lklhd[{t, w}] = LogCalc(l_w_map1[{t, w}], n_of_examples[t]);

            if (debug_mode) {

                rep_str = "  " + t + ":" + w + ", count = " +
                    to_string(l_w_map1[{t, w}]) + ", log-likelihood = ";
                cout << rep_str;

                cout << log_lklhd[{t, w}] << endl;
            }
        }
    }
    cout << endl;

}

void Usage() {
    cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
}

bool CheckArgs(int num_args, char** cmd_args) {

    //for (int i = 0; i < num_args; i++) {
    //    cout << cmd_args[i] << " ";
    //}
    //cout << endl;

    if (num_args < 3 || num_args > 4) {
        Usage();
        return false;
    }

    if (num_args == 4 && strcmp(cmd_args[3], "--debug") != 0) {
        Usage();
        return false;
    }

    ifstream fin1, fin2;
    fin1.open(cmd_args[1]);

    if (!fin1.is_open()) {
        cout << "Error opening file: " << cmd_args[1] << endl;
        return false;
    }
    fin2.open(cmd_args[2]);
    if (!fin2.is_open()) {
        cout << "Error opening file: " << cmd_args[2] << endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {

    if (!CheckArgs(argc, argv)) {
        return 1;
    }

    cout << setprecision(3);

    char* train_file_name = argv[1];
    char* test_file_name = argv[2];
    bool debug = false;
    if (argc == 4) {
        debug = true;
    }
    Classifier	clasfr = Classifier(debug);
    clasfr.TrainOnPosts(train_file_name);
    clasfr.PredictProb(test_file_name);


    return 0;
}