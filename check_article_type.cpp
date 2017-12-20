#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <cctype>
#include <fstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "universal.h"

using namespace std;

//shortforms for vector and iterators
typedef typename std::vector <WORD_COUNT> WORD_VECTOR;
typedef typename std::vector <WORD_COUNT> :: iterator WORD_ITERATOR;

//to remove some common words to reduce data size and unnatural influence of results
bool is_word_common(char word[]){
    int times = sizeof(common_words_to_disclude)/SIZE_OF_WORDS;
    for(int i = 0; i < times; i++){
        if(strcmpi(word, common_words_to_disclude[i])==0)
            return true;
    }
    return false;
}

//print all the elements in a vector
void print_vector(WORD_VECTOR & listy, int filter = 0){
    WORD_ITERATOR it = listy.begin(), stop = listy.end();
    for(; it < stop; it++){
        if((*it).probability > filter)
            cout << *it << endl;
    }
}


//binary search in a vector
int binary_search_on_vector(WORD_VECTOR & listy, char word_to_search[], int start, int finish){
    int mid = (start + finish)/ 2;
    //cout << mid << " " << listy[mid] << '\n';
    int help = strcmpi(listy[mid].word, word_to_search);
    if(help == 0 || start == finish)
        return mid;
    if(help < 0)
        return binary_search_on_vector(listy, word_to_search, mid + 1, finish);
    if(help > 0)
        return binary_search_on_vector(listy, word_to_search, start, mid);
}

//helping function to find if a word contains any special characters. If true, will not add to training as it may
//unneccarily increase size of data (unless the word had a .(dot) or ,(comma) at the end
bool character_in_string(char word[]){
    int len = strlen(word);
    if(word[len-1] == '.' || word[len-1] == ',')
        word[--len] = '\0';
    for(int i = 0; i < len; i++){
        if(!isalpha(word[i]))
            return 1;
    }
    return 0;
}

//convert a text file to vector in sorted ascending order of words
WORD_VECTOR txt_file_to_vector(char file_name[]){
    ifstream myfile(file_name, ios::in);
    WORD_VECTOR word_list;
    if(!myfile.is_open()){
        //cout << "Couldn't find file " << file_name << "\n";
        return word_list;
    }
    WORD_COUNT word_object;
    WORD_ITERATOR word_it;
    char temp_word[SIZE_OF_WORDS];
    while(!myfile.eof()){
        myfile >> temp_word;
//        getline(myfile, temp_word, ' ');
        if(!character_in_string(temp_word) && !is_word_common(temp_word)){
            word_it = (word_list.begin() + binary_search_on_vector(word_list, temp_word, 0, word_list.size()));
            //cout << word_list.size() << "lol";
            if(word_it == word_list.end()){
                strcpy(word_object.word, temp_word);
                word_object.probability = 1;
                word_list.push_back(WORD_COUNT(word_object));
            }
            else if(strcmpi((*word_it).word, temp_word) == 0){
                //cout << "INSIDE 2" << endl;
                (*word_it).probability += 1;
            }
            else{
                //cout << "INSIDE 3" << endl;
                strcpy(word_object.word, temp_word);
                word_object.probability = 1;
                word_list.insert(word_it, WORD_COUNT(word_object));
            }
        }
    }
    myfile.close();
    return word_list;
}

//convert a binary file to
WORD_VECTOR bin_file_to_vector(char file_name[]){
    ifstream myfile(file_name, ios::in | ios::binary);
    WORD_VECTOR word_list;
    WORD_COUNT word;
    if(!myfile.is_open()){
        //cout << "Couldn't find file " << file_name << "\n";
        return word_list;
    }
    while(!myfile.eof()){
        myfile.read((char*)&word, sizeof(word));
        word_list.push_back(word);
    }
    myfile.close();
    return word_list;
}

//return probability using naive bayes approach
double text_classification_probability(WORD_VECTOR vector1, WORD_VECTOR vector2){
    double probability = 0.0;
    long terms = 0, total_words = 0;
    for(int i = 0, j = 0; i < vector1.size() && j < vector2.size();){
        if(strcmpi(vector1[i].word, vector2[j].word) > 0){
            j++;
        }
        else if(strcmpi(vector1[i].word, vector2[j].word) < 0){
            i++;
        }
        else{
            //cout << vector1[i] << "\n";
            probability += (vector2[j].probability * log(vector1[i].probability));
            i++;
            j++;
        }
    }
    //cout  << "\n\n" << probability;
    return probability;
}

int main(){

    //important variables
    WORD_VECTOR main_vec, temp_vec;
    main_vec = txt_file_to_vector(Main_File);
    const int number_of_files = sizeof(Article_Type)/SIZE_OF_WORDS;
    double probability_list[number_of_files];
    char path[3*SIZE_OF_WORDS];
    double max_probability = -pow(2, sizeof(double)*8); //to set a value similar to negative infinity
    int index_of_max_probability = 0;

    //checking initial errors
    if(main_vec.empty()){
        cout << "Either " << path << " file is empty or not found." << endl;
        getch();
        exit(1);
    }

    //main loop
    for(int i = 0; i < number_of_files; i++){
        strcpy(path, Training_Data_Folder);
        strcat(path, Article_Type[i]);
        strcat(path, Training_Data_Type);
        temp_vec = bin_file_to_vector(path);

        //NOTE: this function takes main_vec as the second argument
        probability_list[i] = text_classification_probability(temp_vec, main_vec);
        //to find the best solution
        if(probability_list[i] > max_probability){
            max_probability = probability_list[i];
            index_of_max_probability = i;
        }

        //cout << path << " " << probability_list[i] << "\n\n\n\n";
        temp_vec.clear();
    }

    cout << "GIVEN INPUT IS CLOSEST TO A " << Article_Type[index_of_max_probability] << " TYPE ARTICLE!!!!" << endl;

    //to stop the screen if run as .exe
    getch();
    return 0;
}
