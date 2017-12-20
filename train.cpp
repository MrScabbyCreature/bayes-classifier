#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <cctype>
#include <fstream>
#include <vector>
#include <string.h>
#include <dirent.h>
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

//convert a file to vector in sorted ascending order of words
WORD_VECTOR file_to_vector(char file_name[]){
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
                //(*word_it).probability += 1;
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

//merge vectors to create word counting vector
WORD_VECTOR merge_vectors(WORD_VECTOR main_vec, WORD_VECTOR temp_vec){
    WORD_ITERATOR word_it;
    WORD_COUNT word_object;

    for(int i = 0; i < temp_vec.size(); i++){
        word_it = (main_vec.begin() + binary_search_on_vector(main_vec, temp_vec[i].word, 0, main_vec.size()));
        if(word_it == main_vec.end()){
            //cout << "ASd" << " ";
            strcpy(word_object.word, temp_vec[i].word);
            word_object.probability = 1;
            main_vec.push_back(WORD_COUNT(word_object));
        }
        else if(strcmpi((*word_it).word, temp_vec[i].word) == 0){
            //cout << "INSIDE 2" << endl;
            //cout << (*word_it).word << "\n";
            (*word_it).probability += 1;
        }
        else{
            //cout << "INSIDE 3" << endl;
            strcpy(word_object.word, temp_vec[i].word);
            word_object.probability = 1;
            main_vec.insert(word_it, WORD_COUNT(word_object));
        }
    }
    return main_vec;
}

//write vector into a binary file
void vector_to_file(char file_name[], WORD_VECTOR vec){
    ofstream output_file;
    output_file.open(file_name, ios::out | ios::binary);

    if(!output_file.is_open()){
        cout << "Couldn't open output file " << file_name << ". Please check if required folder is present.\n";
    }

    //writing into training data
    int vec_len = vec.size();
    int object_size = sizeof(WORD_COUNT);

    for(int j = 0; j < vec_len; j++){
        output_file.write((char*)&vec[j], object_size);
    }
    output_file.close();
}

int main(){

    //some important variables
    const int number_of_files = sizeof(Article_Type)/SIZE_OF_WORDS;
    WORD_VECTOR vec, main_vec;
    DIR *pDIR;
    long document_samples = 0; // to divide and thus, find probability
    struct dirent *entry;
    char path_to_input_file[3*SIZE_OF_WORDS];
    char path_to_output_file[3*SIZE_OF_WORDS];

    //main loop
    for(int i = 0; i < number_of_files; i++){
        strcpy(path_to_input_file, "./");
        strcat(path_to_input_file, Article_Type[i]);
        if( pDIR=opendir(path_to_input_file) ){
            document_samples = 0;
            main_vec.clear();
            //WORD_VECTOR(main_vec).swap(main_vec);
            while(entry = readdir(pDIR)){
                if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
                    vec.clear();
                    //WORD_VECTOR(vec).swap(vec);
                    strcpy(path_to_input_file, Article_Type[i]);
                    strcat(path_to_input_file, "/");
                    strcat(path_to_input_file, entry->d_name);
                    vec = file_to_vector(path_to_input_file);
                    //cout << vec.capacity() << "\n";
                    main_vec = merge_vectors(main_vec, vec);
                    //cout << main_vec.capacity() << "\n";
                    document_samples++;
                }
            }
            //cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n";
            closedir(pDIR);

            //dividing by number of documents to make occurrence  probability
            int x;
            for(x = 0; x < main_vec.size(); x++){
                main_vec[x].probability /= document_samples;
            }


            strcpy(path_to_output_file, Training_Data_Folder);
            strcat(path_to_output_file, Article_Type[i]);
            strcat(path_to_output_file, Training_Data_Type);
            vector_to_file(path_to_output_file, main_vec);
        }
    }
    cout << "Training Complete.";
    getch();
    //main loop
//    for(int i = 0; i < number_of_files; i++){
//
//        //taking input
//        Give_Path_To(path_to_input_file, Input_To_Train_Folder, Article_Type[i], Input_To_Train_Type);
//        vec = file_to_vector(path_to_input_file);
//        //opening output file
//        Give_Path_To(path_to_output_file, Training_Data_Folder, Article_Type[i], Training_Data_Type);
//        output_file.open(path_to_output_file, ios::out | ios::binary);
//
//        //checking for any errors
//        if(vec.empty()){
//            cout << "Either " << path_to_input_file << " file is empty or not found." << endl;
//            continue;
//        }
//        if(!output_file.is_open()){
//            cout << "Couldn't open output file " << path_to_output_file << ". Please check if required folder is present.\n";
//            continue;
//        }
//
//        //writing into training data
//        int vec_len = vec.size();
//        int object_size = sizeof(WORD_COUNT);
//
//        for(int j = 0; j < vec_len; j++){
//            output_file.write((char*)&vec[j], object_size);
//        }
//        output_file.close();
//        vec.clejxcadfsfaar();ads+
//    }
    return 0;
}
