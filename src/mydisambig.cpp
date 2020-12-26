#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include"Ngram.h"
#include"Vocab.h"
#include"File.h"

using namespace std;

double getBigramProb(const char *word1, const char *word2, Ngram &lm, Vocab &voc)
{
    VocabIndex word_idx_one = voc.getIndex(word1);
    VocabIndex word_idx_two = voc.getIndex(word2);

    // OOV
    if (word_idx_one == Vocab_None) {
        word_idx_one = voc.getIndex(Vocab_Unknown);
        }
    // OOV
    if(word_idx_two == Vocab_None) {
        word_idx_two = voc.getIndex(Vocab_Unknown);
        }

    VocabIndex context[] = { word_idx_one, Vocab_None };
    return lm.wordProb( word_idx_two, context);
}

double getTrigramProb(const char *word1, const char *word2, const char *word3, Ngram &lm, Vocab &voc) 
{
    VocabIndex wid1 = voc.getIndex(word1);
    VocabIndex wid2 = voc.getIndex(word2);
    VocabIndex wid3 = voc.getIndex(word3);

    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);
    if(wid3 == Vocab_None)  //OOV
        wid3 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid2, wid1, Vocab_None };
    return lm.wordProb( wid3, context );
}

string remove_all_space(string &sequence)
{
    int any_space = 0;
    while (any_space != -1) 
    {   
        any_space = sequence.find(" ");
        if (any_space != -1)
            sequence.erase(any_space, 1);
    }
    return sequence;
}

string viterbi_algorithm(string &sequence, map<string, vector<string>> &zhuin_to_word, Ngram &lm, Vocab &voc)
{
    // init
    vector<string> candidates_seq, candidates_seq_temp;
    vector<double> candidates_prob, candidates_prob_temp;
    double now_prob;
    double max_prob = -100000;
    int temp_loc;
    string output;

    // cal <s> + start word
    string start_word = sequence.substr(0,2);
    for(int i = 0; i < zhuin_to_word[start_word].size(); i++)
    {
        // save the potential string end prob
        candidates_seq.push_back(zhuin_to_word[start_word][i]);
        candidates_prob.push_back(getBigramProb("<s>", zhuin_to_word[start_word][i].c_str(), lm, voc));
    }

    // second word to the end
    for (int i = 2; i < sequence.size(); i += 2) 
    {
        string now_word = sequence.substr(i,2); // get the word now
        for(int j = 0;j < zhuin_to_word[now_word].size(); j++) // for all potential substitution
        {
            max_prob = -100000;
            for(int k = 0; k < candidates_seq.size(); k++) // try to add to all candidate seq
            {
                string last_word = candidates_seq[k].substr(candidates_seq[k].length()-2, 2); // get last word in candidate seq
                now_prob = candidates_prob[k] + getBigramProb(last_word.c_str(), zhuin_to_word[now_word][j].c_str(), lm, voc);
                if(now_prob > max_prob)
                {
                    max_prob = now_prob;
                    temp_loc = k;
                }
            }
            candidates_seq_temp.push_back(candidates_seq[temp_loc] + zhuin_to_word[now_word][j]);
            candidates_prob_temp.push_back(max_prob);
        }
        // prepare for copy from temp
        candidates_seq.clear();
        candidates_prob.clear();
        // copy from temp
        candidates_seq.assign(candidates_seq_temp.begin(),candidates_seq_temp.end());
        candidates_prob.assign(candidates_prob_temp.begin(),candidates_prob_temp.end());
        // clear temp
        candidates_seq_temp.clear();
        candidates_prob_temp.clear();
    }

    // last word + </s>
    for(int i = 0; i < candidates_seq.size(); i++)
    {
        candidates_prob[i] += getBigramProb(candidates_seq[i].substr(candidates_seq[i].length()-2, 2).c_str(), "</s>", lm, voc);
    }
    
    max_prob = -100000;
    for(int i = 0; i < candidates_seq.size(); i++)
    {
        if(candidates_prob[i] > max_prob)
        {
            output = candidates_seq[i];
            max_prob = candidates_prob[i];
        }
    }
    return output;
}

string viterbi_algorithm_trigram(string &sequence, map<string, vector<string>> &zhuin_to_word, Ngram &lm, Vocab &voc)
{
    // init
    vector<string> candidates_seq, candidates_seq_temp;
    vector<double> candidates_prob, candidates_prob_temp;
    double now_prob;
    double max_prob = -100000;
    int temp_loc;
    string output;

    // cal <s> + start word
    string start_word = sequence.substr(0,2);
    for(int i = 0; i < zhuin_to_word[start_word].size(); i++)
    {
        // save the potential string end prob
        candidates_seq.push_back(zhuin_to_word[start_word][i]);
        candidates_prob.push_back(getBigramProb("<s>", zhuin_to_word[start_word][i].c_str(), lm, voc));
    }

    // second word to the end
    for (int i = 2; i < sequence.size(); i += 2) 
    {
        string now_word = sequence.substr(i,2); // get the word now
        for(int j = 0;j < zhuin_to_word[now_word].size(); j++) // for all potential substitution
        {
            max_prob = -100000;
            for(int k = 0; k < candidates_seq.size(); k++) // try to add to all candidate seq
            {   
                string last_word = candidates_seq[k].substr(candidates_seq[k].length()-2, 2); // get last word in candidate seq
                if (i == 2) {
                    now_prob = candidates_prob[k] + getTrigramProb("<s>", last_word.c_str(), zhuin_to_word[now_word][j].c_str(), lm, voc);
                } else {
                    string last_last_word = candidates_seq[k].substr(candidates_seq[k].length()-4, 2); // get last last word in candidate seq
                    now_prob = candidates_prob[k] + getTrigramProb(last_last_word.c_str(), last_word.c_str(), zhuin_to_word[now_word][j].c_str(), lm, voc);
                }
                
                
                if(now_prob > max_prob)
                {
                    max_prob = now_prob;
                    temp_loc = k;
                }
            }
            candidates_seq_temp.push_back(candidates_seq[temp_loc] + zhuin_to_word[now_word][j]);
            candidates_prob_temp.push_back(max_prob);
        }
        // prepare for copy from temp
        candidates_seq.clear();
        candidates_prob.clear();
        // copy from temp
        candidates_seq.assign(candidates_seq_temp.begin(),candidates_seq_temp.end());
        candidates_prob.assign(candidates_prob_temp.begin(),candidates_prob_temp.end());
        // clear temp
        candidates_seq_temp.clear();
        candidates_prob_temp.clear();
    }

    // last word + </s>
    for(int i = 0; i < candidates_seq.size(); i++)
    {   
        if (candidates_seq[i].length() == 2){
            candidates_prob[i] += getTrigramProb("<s>", candidates_seq[i].substr(candidates_seq[i].length()-2, 2).c_str(), "</s>", lm, voc);
        } else {
            string last_last_word = candidates_seq[i].substr(candidates_seq[i].length()-4, 2); // get last last word in candidate seq
            candidates_prob[i] += getTrigramProb(last_last_word.c_str(), candidates_seq[i].substr(candidates_seq[i].length()-2, 2).c_str(), "</s>", lm, voc);
        }
    }
    
    max_prob = -100000;
    for(int i = 0; i < candidates_seq.size(); i++)
    {
        if(candidates_prob[i] > max_prob)
        {
            output = candidates_seq[i];
            max_prob = candidates_prob[i];
        }
    }
    return output;
}

int main(int argc, char *argv[])
{
    clock_t tStart = clock();
    const char* seq_input_path = argv[1];
    const char* zhuyin_to_word_path = argv[2];
	const char* language_model_path = argv[3];
	const char* output_file_path = argv[4];
    int mode = 2;
    if (argc == 5){
        cout << "Bigram mode" << endl;
    }
    if (argc == 6){
        cout << "Trigram mode" << endl;
        mode = 3;
    }

    // read zhuyin_to_word mapping
    string line;
    map<string, vector<string>> chuyin_to_word;
    ifstream infile_mapping(zhuyin_to_word_path);
    while (getline(infile_mapping, line)) 
    {   
        string line_wo_space, chuyin, single_word;

        line_wo_space = remove_all_space(line);

        chuyin  = line_wo_space.substr(0, 2);
        for(int i = 2; i < line_wo_space.size(); i += 2)
        {
            single_word = line_wo_space.substr(i, 2);
            chuyin_to_word[chuyin].push_back(single_word);
        }
    }

    // read lm 
    Vocab voc;
    Ngram lm(voc, mode);
    File lmfile(language_model_path, "r");
    lm.read(lmfile);
    lmfile.close();

    // read input seq and do viterbi
    ifstream input_file(seq_input_path);
    ofstream output_file(output_file_path);

    while (getline(input_file, line)) 
    {
        string line_wo_space, result;

        line_wo_space = remove_all_space(line);
        if (mode == 2){
            result = viterbi_algorithm(line, chuyin_to_word, lm, voc);
        }
        if (mode == 3){
            result = viterbi_algorithm_trigram(line, chuyin_to_word, lm, voc);
        }

        output_file << "<s> ";
        for(int i = 0; i < result.length() ; i+= 2)
        {
            output_file << result.substr(i,2) << " ";
        }
        output_file << "</s>" <<endl;
    }

    output_file.close();
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    return 0;
}