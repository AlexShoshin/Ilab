#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define LENGHT_OF_FILE_STRING 10000


// This program will try to guess that you thought
// It uses binary tree algorithms
// This tree is saved in file, and it is reviving with every program start
// In the end of the program tree saves into the file


struct Node             // nodes of our data-tree
{
    char *s;            //text that is in node
    struct Node *left;
    struct Node *right;
};

void help_section();

int is_question(struct Node *tree);  //checking if node's text is question

struct Node * revive_tree(struct Node * tree, char *s, int j); //revives tree from the file

struct Node * change_answers(struct Node * tree);  //if akinator don't guesses the right answer, we should add to tree right answer
                                                 //and question, explaining with that the answers are different

void save_data(struct Node * tree, FILE *output);  //after the program saves tree into the file

void tree_destroy (struct Node * tree);          //frees memory

char * check_file_structure();        //checking structure of file then reading

char * read_file();                     //read data from file

char * read_symbol_for_question(struct Node * cur);

char *read_symbol_for_quess(struct Node *cur);

char *read_symbol_for_ending();

void akinate(struct Node * tree);






int main()
{
    char *file_string = calloc(LENGHT_OF_FILE_STRING, sizeof(char));   //read data
    file_string = read_file();


    struct Node *tree = NULL;                   //reviving tree from file's string
    struct Node * root = calloc(1, sizeof(struct Node));
    tree = revive_tree(tree, file_string, 0);
    root = tree;
    printf("\nDatafile found, tree constructed, I'm ready!\n\n");

    akinate(root);

    tree = root;                                    //saving tree to the file
    FILE *output = fopen("vhod.txt", "w");
    save_data(tree, output);

    fclose(output);                     //free memory
    tree_destroy(tree);
    free(file_string);
    free(root);
    printf("See you later!\n");

    return 0;
}







void help_section()
{
    printf("  Hello! Akinator is a game, that will try to guess your thought.\n");
    printf("  How to play: make guess - any person or thing you want and answer for questions correctly:\n");
    printf("If you want answer yes, write y , if you want answer no, write n.\n");
    printf("  During the game process you can change wording of questions or wording of answers in the end.\n");
    printf("For this, write symbol c.\n");
    printf("  At the end of the game there are two ways:\n");
    printf("If akinator guessed that you thought, you can exit or play again.\n");
    printf("If akinator didn't guess - you can add your answer for gamedata.\n");
    printf("  Have a good game!\n");

}

void akinate(struct Node * tree)
{
    char *symbol = calloc(100, sizeof(char));
    int exit_flag = 0;                                     //this flag == 0 if user wants to play again
    struct Node * cur = malloc(sizeof(struct Node));       //current tree's node
    struct Node * root = malloc(sizeof(struct Node));
    root = tree;
    cur = tree;

    while(exit_flag == 0)
    {
        if(is_question(cur) == 1)               //checking if node's text is question
        {
            printf("%s? (y/n/c)\n", cur->s);      //if yes - go further

            symbol = read_symbol_for_question(cur);

            if(strcmp(symbol, "y") == 0)
            {
                cur = cur->left;
            }
            else if(strcmp(symbol, "n") == 0)
            {
                cur = cur->right;
            }

        }
        else                                                    //if not - show the answer
        {

            printf("It is %s, am i right? (y/n)\n", cur->s);

            symbol = read_symbol_for_quess(cur);
            if(strcmp(symbol, "y") == 0)
            {
                printf("Cool, i known it!\n");
            }
            else
            {
                tree = cur;
                tree = change_answers(tree);                //changing answers

            }


            printf("Do you want to play again? (y/n)\n");       //finding out if user wants to play again

            symbol = read_symbol_for_ending();
            if(strcmp(symbol, "n") == 0)
            {
                exit_flag = 1;
            }
            else
            {
                cur = root;
            }
        }
    }
}



char *read_symbol_for_ending()
{
            char *symbol = calloc(100, sizeof(char));
            int check_y = 0;                        //they are needed for defining if users writing is correct
            int check_n = 0;
            scanf("%s", symbol);
            check_y = strcmp(symbol, "y");                      //checking for right symbol
            check_n = strcmp(symbol, "n");
            while(check_n != 0 && check_y != 0)
            {
                printf("WTF? Please, write |y| for yes or |n| for no!\n");
                printf("Do you want to play again? (y/n)\n");
                scanf("%s", symbol);
                check_y = strcmp(symbol, "y");
                check_n = strcmp(symbol, "n");
            }
            return symbol;
}

char * read_symbol_for_question(struct Node * cur)
{

            char *symbol = calloc(100, sizeof(char));
            int check_y = 0;                        //they are needed for defining if users writing is correct
            int check_n = 0;
            int check_c = 0;
            char *change_buf = calloc(100, sizeof(char));  //is needed to change wording of questions or answers

            scanf("%s", symbol);
            check_y = strcmp(symbol, "y");                      //checking for right symbol
            check_n = strcmp(symbol, "n");
            check_c = strcmp(symbol, "c");
            while(check_n != 0 && check_y != 0)
            {
                if(check_c == 0)                                                        //checking if user wants to change wording
                {
                    printf("You are changing wording of the qustion.\n");
                    printf("Please, write new wording of question, add ? aeparetely in the end:\n");
                    change_buf[0] = '\0';
                    scanf("%s", symbol);
                    while(symbol[0] != '?')
                    {
                        change_buf = strcat(change_buf, symbol);
                        change_buf = strcat(change_buf, " ");
                        scanf("%s", symbol);
                    }

                    cur->s = change_buf;
                    printf("Changing successed!\n");
                    printf("%s?(y/n/c)\n", cur->s);

                }
                else
                {
                    printf("WTF? Please, write |y| for yes, |n| for no or |c| for changing wording of question!\n");
                    printf("%s? (y/n/c)\n", cur->s);
                }

                scanf("%s", symbol);
                check_y = strcmp(symbol, "y");
                check_n = strcmp(symbol, "n");
                check_c = strcmp(symbol, "c");
            }

            return symbol;
}

char *read_symbol_for_quess(struct Node *cur)
{

            char *symbol = calloc(100, sizeof(char));
            int check_y = 0;                        //they are needed for defining if users writing is correct
            int check_n = 0;
            scanf("%s", symbol);
            check_y = strcmp(symbol, "y");                              //checking for right symbol
            check_n = strcmp(symbol, "n");

            while(check_n != 0 && check_y != 0)
            {
                printf("WTF? Please, write |y| for yes or |n| for no!\n");
                printf("It is %s, am i right? (y/n)\n", cur->s);
                scanf("%s", symbol);
                check_y = strcmp(symbol, "y");
                check_n = strcmp(symbol, "n");
            }
            return symbol;

}


char * read_file()                     //read data from file
{
    printf("Akinator 1.0\n");
    printf("I'll try to guess you thought!\n");
    printf("\n");

    assert(check_file_structure());
    FILE* input = fopen("vhod.txt", "r");
    if(input == NULL)                                           //checking for right file direction
    {
        printf("File not found!\n");
        printf("Please, check your data-file location!\n");
        printf("It should be in one direction with the program file!\n");
        return 0;
    }
    char *file_string = calloc(LENGHT_OF_FILE_STRING, sizeof(char));
    char *symbol = calloc(100, sizeof(char));

    file_string[0] = '\0';                      //reading string from file, that consists tree's structure - symbol by symbol
    while(fscanf(input, "%s", symbol) == 1)
    {
        file_string = strcat(file_string, symbol);
        file_string = strcat(file_string, " ");
    }

    help_section();
    fclose(input);
    return file_string;
}



char * check_file_structure()
{
    FILE* input = fopen("vhod.txt", "r");
    char * check_string = calloc(5, sizeof(char));
    int opening_brackets_counteer = 0;
    int closing_brackets_counteer = 0;
    int sum = 0;
    char symbol;
    while(fscanf(input, "%c", &symbol) == 1)
    {
        if(symbol == '{')
        {
            opening_brackets_counteer++;
        }
        else if (symbol == '}')
        {
            closing_brackets_counteer++;
        }

        if(closing_brackets_counteer > opening_brackets_counteer + 1)
        {
            return NULL;
        }
    }
    sum = opening_brackets_counteer - closing_brackets_counteer;
    if(sum != 0)
    {
        return NULL;
    }

    check_string = "okay";

    return check_string;
}




int is_question(struct Node *tree)  //checking if node's text is question
{
    if(tree->left != NULL && tree->right != NULL)  //because node has a question if after this there are answers
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


struct Node * revive_tree(struct Node * tree, char *s, int j) //revives tree from the file
{


    tree = malloc(sizeof(struct Node));         //constructing tree's node
    tree->left = NULL;
    tree->right = NULL;
    tree->s = calloc(100, sizeof(char));
    tree->s[0] = '\0';


    int i = j + 1;                              //reading text that should be in this node by reading every symbol
    char *symbol = calloc(100, sizeof(char));
    while(s[i] != '}' && s[i] != '{')
    {
        symbol[0] = s[i];
        symbol[1] = '\0';
        tree->s = strcat(tree->s, symbol);
        i++;
    }

    int counter = 1;                            //recursive algorithm of constructing all tree by reading every symbol
    while(counter != 0)                         //there are counted brackets those are in file to construct tree right
    {

        if (s[i] == '{')
        {
            counter++;
        }
        if(s[i] == '}')
        {
            counter--;
        }


        if(s[i] == '{' && (counter == 2) && (tree->left == NULL))  //counter == 2 because then in file at this place starts node's left part
        {

            tree->left = revive_tree(tree->left, s, i);
        }
        if(s[i] == '{' && (counter == 2) && (tree->left != NULL)) //counter == 2 because then in file at this place starts node's right part
        {
            tree->right = revive_tree(tree->right, s, i);
        }
        i++;

    }
    free(symbol);

    return tree;
}


struct Node * change_answers(struct Node * tree)  //if akinator don't guesses the right answer, we should add to tree right answer
{                                                 //and question, explaining with that the answers are different
    printf("Ooo, that (or who) it was?( please, in the end separately write !)\n");

    char *thought_answer = calloc(100, sizeof(char));      //answer that user thought
    char *symbol = calloc(100, sizeof(char));
    char *diff_question = calloc(100, sizeof(char));       //question that shows with what answers are different

    thought_answer[0] = '\0';                                //reading answer that user thought
    scanf("%s", symbol);
    while(symbol[0] != '!')
    {
        thought_answer = strcat(thought_answer, symbol);
        thought_answer = strcat(thought_answer, " ");
        scanf("%s", symbol);
    }


    tree->left = calloc(1, sizeof(struct Node));
    tree->right = calloc(1, sizeof(struct Node));
    struct Node *cur = calloc(1, sizeof(struct Node));                 //current node before changing
    cur = tree;


    printf("Now, please write with that  %s  differs from  %s.\n", tree->s, thought_answer);
    printf("It should be some kind of question, in the end separately write ?.\n");             //reading question

    diff_question[0] = '\0';
    scanf("%s", symbol);
    while(symbol[0] != '?')
    {
        diff_question = strcat(diff_question, symbol);
        diff_question = strcat(diff_question, " ");
        scanf("%s", symbol);
    }


    printf("Okey, that(or who) should be after your answer |yes|?\n");      //correctly filling answer nodes
    printf("1) %s\n", tree->s);
    printf("2) %s\n", thought_answer);
    printf("Write |1| or |2|\n");

    scanf("%s", symbol);
    int check_1 = strcmp(symbol, "1");                      //checking for right symbol
    int check_2 = strcmp(symbol, "2");
    while(check_1 != 0 && check_2 != 0)
    {
        printf("WTF? Please, write |1| for %s or |2| for %s!\n", tree->s, thought_answer);
        scanf("%s", symbol);
        check_1 = strcmp(symbol, "1");
        check_2 = strcmp(symbol, "2");
    }
    if(strcmp(symbol, "1") == 0)                        //filling left answer
    {
        tree = tree->left;
        tree->left = NULL;
        tree->right = NULL;
        tree->s = calloc(100, sizeof(char));
        tree->s = cur->s;

        tree = cur->right;                              //filling right answer
        tree->left = NULL;
        tree->right = NULL;
        tree->s = calloc(100, sizeof(char));
        tree->s = thought_answer;

        tree = cur;
    }
    else
    {
        tree = tree->right;                             //filling left answer
        tree->left = NULL;
        tree->right = NULL;
        tree->s = calloc(100, sizeof(char));
        tree->s = cur->s;

        tree = cur->left;                           //filling right answer
        tree->left = NULL;
        tree->right = NULL;
        tree->s = calloc(100, sizeof(char));
        tree->s = thought_answer;

        tree = cur;
    }
    tree->s = diff_question;                    //in the current node writing question instead of previous answer
    printf("Okey, now i know it!\n");


    free(symbol);

    return tree;
}


void save_data(struct Node * tree, FILE *output)  //after the program saves tree into the file
{

    fprintf(output, "%c", '{');
    fprintf(output, "%s", tree->s);

    if(tree->left != NULL)
    {
        save_data(tree->left, output);
    }

    if(tree->right != NULL)
    {
        save_data(tree->right, output);
    }


    fprintf(output, "%c", '}');


}


void tree_destroy (struct Node * tree)          //frees memory
{
    if(tree != NULL)
    {
        tree_destroy(tree->left);
        tree_destroy(tree->right);
        struct Node * p;
        p = realloc(tree, 0);
    }
}



