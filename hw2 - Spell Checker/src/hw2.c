#include "hw2.h"

/* Great filename. */

void processDictionary(FILE* f){
    dict->num_words = 0;
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE+1, f))
    {
        //initialize the current word.
        struct dict_word* currWord;
        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            exit(2);
        }
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        //variables
        char word[MAX_SIZE];
        char* wdPtr = word;

        char* character = line;
        //char word_list[MAX_MISSPELLED_WORDS+1][MAX_SIZE];
        int counter = 0;
        int firstWord = 1;
        //if there isn't a space at the end of the line, put one there
        /*if((line[strlen(line)-1] != ' ' && line[strlen(line)-1] == '\n') || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n'))
            strcat(line, " ");*/

        while(*character != 0)
        {
            if(counter >= MAX_MISSPELLED_WORDS+1)
                break;
            //if the character is a space, add the word in word_list and make word NULL.
            if(*(character) == ' '|| *(character+1) == 0 || *character == '\n' )
            {
                if(*(character+1) != 0){
                    *character = '\0';
                }
                *wdPtr = '\0';
                wdPtr = word;
                if(firstWord)
                {
                    addWord(currWord, wdPtr);
                    dict->num_words++;
                    firstWord = 0;
                }
                else
                {
                    struct misspelled_word* currMisspelling;
                    if((currMisspelling = malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        exit(2);
                    }

                    addMisspelledWord(currMisspelling, currWord, wdPtr);
                }
                //printf("wordadd:%s:end", wdPtr);
                //character++;
            }
            //if the character isn't a space or a new line, add the character to word.
            else
            {
                *(wdPtr++) = *character;
            }
            character++;
        }
    }
    //fclose(f);
}

void addWord(struct dict_word* dWord, char* word){
    //setting up dWord fields
    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;
    dWord->next = dict->word_list;
    strcpy(dWord->word, word);
    dict->word_list = dWord;
}

void addMisspelledWord(struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){
    //setting up misspelledWord fields
    strcpy(misspelledWord->word, word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;
    misspelledWord->next = m_list;
    (correctWord->misspelled)[correctWord->num_misspellings++] = misspelledWord;
    m_list = misspelledWord;
}


void freeWords(struct dict_word* currWord){
    if(currWord != NULL)
    {
        freeWords(currWord->next);
        //free word
        int i = currWord->num_misspellings;
        while(i != 0){
            //printf("FREED MISS: %s\n",currWord->misspelled[i-1]->word );
            //printf("Freed Miss: %s\n", currWord->misspelled[i-1]->word);
            free(currWord->misspelled[i-1]);
            i--;
        }
        //printf("FREED %s\n", currWord->word);
        free(currWord);
    }

}

void saveDict(struct dict_word* dict, char* file){
    FILE* d;
    d = fopen(file, "w");
    if(d == NULL){
        USAGE(EXIT_FAILURE);
    }
    while(dict){
        fwrite(dict->word, strlen(dict->word), 1, d);
        if(dict->num_misspellings != 0){
            for(int i = 0; i < dict->num_misspellings; i++){
                fwrite(" ", 1, 1, d);
                fwrite(dict->misspelled[i]->word, strlen(dict->misspelled[i]->word), 1, d);
            }
        }
        fwrite("\n", 1, 1, d);
        dict =  dict->next;
    }

    fclose(d);

}

void sizeDictionary(){
    dictSize = 0;
    missSize = 0;
    numMiss = 0;
    int topMiss[3] = {0,0,0};
    printWords(dict->word_list , stderr, topMiss);
    fprintf(stderr, "Total number of words in dictionary: %d\n", dict->num_words);
    dictSize += sizeof(dict);
    fprintf(stderr, "Size of dictionary (in bytes): %lu\n", sizeof(struct dictionary) + (sizeof(struct dict_word) * dict->num_words));
    fprintf(stderr, "Size of misspelled word list (in bytes): %d\n",  numMiss * (int)sizeof(struct misspelled_word));
    fprintf(stderr, "Total number of misspelled words: %d\n", numMiss);
    fprintf(stderr, "Top 3 misspelled words:\n");
    struct dict_word* topM = NULL;



    for(int topcount = 0; topcount < 3; topcount++){
        topM = dict->word_list;
        while(topM){
            if(topM->misspelled_count == topMiss[topcount] && topMiss[topcount] != 0){
                //fprintf(stderr, "word: %s:end",topM->word);
                fprintf(stderr, "%s (%d times): ",topM->word, topM->misspelled_count);
                int comma = 0;
                for(int i = 0; i < topM->num_misspellings; i++){
                    if(topM->misspelled[i]->misspelled > 0){
                        if(comma == 1){
                            fprintf(stderr, ", ");
                        }
                        fprintf(stderr, "%s", topM->misspelled[i]->word);
                        comma = 1;
                    }
                }
                fprintf(stderr, "\n");

            }
            topM = topM->next;
        }
        if(topcount < 2 && topMiss[topcount + 1] == topMiss[topcount]){
            topcount++;
        }
    }

}


void printWords(struct dict_word* currWord, FILE* f, int topMiss[]){
    if(currWord != NULL)
    {
        printWords(currWord->next, f, topMiss);
        dictSize += sizeof(currWord);

        if(currWord->misspelled_count > 0){
            numMiss += currWord->misspelled_count;
        }
        for(int j = 0; j < 3; j++){
            if(currWord->misspelled_count >= topMiss[j]){
                if(j == 0){
                    topMiss[2] = topMiss[j + 1];
                    topMiss[1] = topMiss[j];
                    topMiss[0] = currWord->misspelled_count;
                }
                else if(j == 1){
                    topMiss[2] = topMiss[j];
                    topMiss[1] = currWord->misspelled_count;
                }
                else{
                    topMiss[2] = currWord->misspelled_count;
                }
                j = 3;
            }
        }
        missSize += currWord->num_misspellings;


    }

}

char* processWord(char* inputWord, int numWord){

    char* found = foundMisspelledMatch(inputWord);
    if(found != NULL){
        //printf("\n%s\n",found );
        return found;
    }
    else if(foundDictMatch(inputWord)){
        return inputWord;
    }
    else
    {
        //char ch;
        //char conf;
        //add new word to dictionary
        if(numWord != -1)
        {

            struct dict_word* newWord;


            if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
            {
                //printf("ERROR: OUT OF MEMORY.\n");
                exit(2);
            }
            addWord(newWord, inputWord);
            //hhi
            dict->num_words++;
            //HELLO
            char** randTypos = gentypos(numWord, inputWord);
            char** randPtr = randTypos;
            //char* randPtr = *randTypos;
            char* wdPtr;
            while(numWord > 0)
            {

                    wdPtr = *randTypos;
                    struct misspelled_word* newMWord;

                    if((newMWord = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        exit(2);
                    }

                    addMisspelledWord(newMWord, newWord, wdPtr);
                    //free(randTypos);
                    free(*randTypos);
                    //free(randTypos);
                    randTypos++;
                    numWord--;

            }
            //randTypos -= 4;
            free(randPtr);
            //free(wdPtr);

        }
        return inputWord;
    }
}

char* foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            strcpy(inputWord, listPtr->correct_word->word);
            listPtr->misspelled = 1;
            listPtr->correct_word->misspelled_count++;
            return listPtr->correct_word->word;
        }
        listPtr = listPtr -> next;
    }
    return NULL;
}

bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return true;
        listPtr = listPtr->next;
    }
    return false;
}
