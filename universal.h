#include<string.h>

#define SIZE_OF_WORDS 20

//article types
char Article_Type[][SIZE_OF_WORDS] = {"Dataset-1", "Dataset-2"};

//files to save training data
char Training_Data_Folder[SIZE_OF_WORDS] = "trained_bin/";
char Training_Data_Type[SIZE_OF_WORDS] = ".bin";

//the file to compare training data with
char Main_File[SIZE_OF_WORDS] = "check_file.txt";

//structure to store training data
struct WORD_COUNT{
    char word[SIZE_OF_WORDS];
    double probability;
};


char common_words_to_disclude[][SIZE_OF_WORDS] = {"a", "about", "am", "an", "and", "any", "are", "as", "at", "be", "because", "by",
            "can", "do", "each", "for", "had", "has", "have", "he", "her", "his", "how", "i", "if", "in", "is", "it", "may", "not", "of", "on", "one", "or", "said", "she",
            "should", "so", "some", "that", "the", "their","them", "then", "they", "this", "to", "was", "we", "what", "when","where", "which", "who", "will", "with",
            "would", "why", "you", "your"};

//to print the structure above using <<
std::ostream& operator<< (std::ostream & out, WORD_COUNT const& data) {
    out << data.word << " ";
    out << data.probability;
    return out ;
}
