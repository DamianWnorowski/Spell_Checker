#include "hw2.h"


int main(int argc, char *argv[]){
    DEFAULT_INPUT = stdin;
    DEFAULT_OUTPUT = stdout;
    char* DEFAULT_DICT_FILE = "rsrc/dictionary.txt";

    //int rand_val;
/*    struct option longopt[] = {
        {"A0",  no_argument,  &rand_val,   0},
        {"A1",  no_argument,  &rand_val,   1},
        {"A2",  no_argument,  &rand_val,   2},
        {"A3",  no_argument,  &rand_val,   3},
        {"A4",  no_argument,  &rand_val,   4},
        {"A5",  no_argument,  &rand_val,   5}
    };*/
    //create dictionary
    dict = 0;
    dict = (struct dictionary*) malloc(sizeof(struct dictionary));


    if((dict) == NULL)
    {
        exit(2);
    }
    dict->word_list = NULL;
    dict->num_words = 0;
    //m_list = (struct misspelled_words*) malloc(sizeof mi);
    m_list = NULL;

    struct Args args;
    // Set struct default values
    args.d = false;
    args.i = false;
    args.o = false;
    strcpy(args.dictFile, DEFAULT_DICT_FILE);
    // Make a loop index
    //int i;
    char line[MAX_SIZE];
    //Declare Files
    FILE* dFile;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;
    int opt;
    int random_value = -1;
    for(int i = 0; i < argc; i++){
        if((*(*(argv + i) + 0)) == '-' && (*(*(argv + i) + 1 ) == 'h' && (*(*(argv + i) + 2)) == 0)){
            USAGE(EXIT_SUCCESS);
        }
        else if((*(*(argv + i) + 0)) == '-' && (*(*(argv + i) + 1 ) == 'A' && (*(*(argv + i) + 2)) == 0)){
            USAGE(EXIT_FAILURE);
        }
    }
    while((opt = getopt(argc, argv, "ho:i:d:A::")) != -1){
        switch(opt){

            case 'h':
            //print help menu
                USAGE(EXIT_SUCCESS);
            break;

            case 'o':
            //output file
                if(*(optarg -1 ) != 0 || *(optarg) < 33){

                    USAGE(EXIT_FAILURE)
                }
                strcpy(args.output, optarg);
                args.o = true;
                oFile = fopen(optarg, "w");
            break;

            case 'i':
            //input file

                if(*(optarg -1 ) != 0 || *(optarg) < 33){
                        USAGE(EXIT_FAILURE)
                    }
                strcpy(args.input, optarg);
                args.i = true;
                iFile = fopen(optarg, "r");
                if(iFile == NULL){
                    USAGE(EXIT_FAILURE);
                }
            break;

            case 'A':
            //input file
                if(*(optarg + 1) == 0 && *(optarg) > 47 && *(optarg) < 54){
                    random_value = atoi(optarg);
                }else{
                    USAGE(EXIT_FAILURE);
                }
            break;

            case 'd':
            //dictionary file
                if(*(optarg) != ' ' && *(optarg + 1) == ' '){
                    USAGE(EXIT_FAILURE)
                }
                strcpy(args.dictFile, optarg);
                args.d = true;
            break;

            case '?':
            //invlaid
                USAGE(EXIT_FAILURE);
            break;

            case ':':
            //invalid
                USAGE(EXIT_FAILURE);
            break;
        }
    }



    dFile = fopen(args.dictFile, "r");

    if(oFile == NULL && args.o == true)
    {
        USAGE(EXIT_FAILURE);
    }
    if(dFile == NULL)
    {
        USAGE(EXIT_FAILURE);
    }
    else
    {
        processDictionary(dFile);
        fclose(dFile);
    }



    while(fgets(line, MAX_SIZE+1, iFile))
    {
        char word[MAX_SIZE];
        *word = 0;
        char* wdPtr = word;
        char tempchar[MAX_SIZE];
        *tempchar = 0;
        char* character = line;

        while(*character != 0)
        {
            *character = tolower(*character);

            if(*character == ' ' || *character == '\n' || *(character+1) == 0)
            {

                *wdPtr = '\0';
                wdPtr = word;

                while(!isalpha(*wdPtr) && *wdPtr != 0){
                    fwrite((wdPtr), 1, 1, oFile);
                    while(*wdPtr != '\0'){
                        *wdPtr = *(wdPtr + 1);
                        wdPtr++;
                    }
                    wdPtr=word;
                }

                wdPtr = word;
                int c = 0;
                c = strlen(wdPtr) -1;
                int counter = 0;
                while(c > 0){
                    if(!isalpha(*(wdPtr + c)) && *(wdPtr + c) != 0){
                        counter++;
                        c--;
                    }
                    else{
                        c = 0;
                    }
                }
                if(counter != 0){
                    strcpy(tempchar, (wdPtr + (strlen(wdPtr) - counter)));
                    *(wdPtr + strlen(wdPtr) - counter) = '\0';
                }
                if(strlen(wdPtr)>0){

                    char* newWord= processWord(wdPtr, random_value);

                    fwrite(newWord, strlen(newWord), 1, oFile);

                }
                if(counter != 0){
                    fwrite(tempchar, strlen(tempchar), 1, oFile);
                }
                fwrite(character, 1, 1, oFile);
            }
            else
            {
                *(wdPtr++) = *character;
            }
            character++;
        }
        if(iFile == stdin)
            break;

    }

    if(random_value != -1){
        char tempDict[MAX_SIZE] = "\0";
        for(int i = strlen(args.dictFile) -1; i > -1; i--){
            if(*(args.dictFile + i) == '/'){
                strncat(tempDict, args.dictFile, i + 1);
                strcat(tempDict, "new_");
                strcat(tempDict, args.dictFile+i + 1);
                i = -1;
            }
            else if(i == 0){
                strncpy(tempDict, "new_", 4);
                strncat(tempDict ,args.dictFile, strlen(args.dictFile));
            }
        }
        saveDict(dict->word_list, tempDict);
    }

    sizeDictionary();
    freeWords(dict->word_list);
    //free dictionary

    free(dict);



    if(args.i){
        fclose(iFile);
    }

    if(args.o){
        fclose(oFile);
    }

    return EXIT_SUCCESS;
}
