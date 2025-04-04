#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"
#include "markov_chain.h"
#include <stdio.h>   // For printf and file I/O functions
#include <string.h> // For strdup, strcmp, etc.
#include <unistd.h> // For sleep()

// Function to check if a string ends with a '.' character
int ends_with_point(const char *str) {
    if (!str || *str == '\0') { // Check for NULL or empty string
        return 0; // Return false
    }

    // Move pointer to the end of the string
    while (*(str + 1)) {
        str++;
    }

    // Check if the last character is '.'
    return (*str == '.') ? 1 : 0;
}


int fill_database(FILE* fp, int words_to_read, MarkovChain* markovChain) {
    MarkovNode *prev = NULL;
    MarkovNode *current;
    int word_count = 0;
    char line[1024];
    char *token = NULL;
    char *last_word_to_read = NULL;
    while (fgets(line, sizeof(line), fp) != NULL) {
        token = strtok(line, DELIMITERS);
        while (((token != NULL && word_count < words_to_read) && (!ends_with_point(last_word_to_read)))) {
            // Allocate and copy data
            word_count++;
            char *data = strdup(token);
            if (!data) {
                printf("Error: Memory allocation failed for word.\n");
                fclose(fp);
                free_database(&markovChain);
                return 1;
            }

            Node * current_node = add_to_database(markovChain,data);
            if (current_node == NULL && markovChain->database->size !=0 ) {
                printf("Failed to add word to database.\n");
                free(current);
                free(data);
                return 1;
            }

            // If there's a previous word, add it to the frequency list
            if (prev != NULL) {
                Node* first = get_node_from_database(markovChain,prev->data);
                if(first->data!=NULL)
                {
                    add_node_to_frequency_list(first->data, current_node->data);
                }
            }

            // Update previous word and increment count
            if(current_node!=NULL)
            {
                prev = current_node->data;

                last_word_to_read = data;

            }
            token = strtok(NULL, DELIMITERS);

        }
        if(word_count > words_to_read)
        {
            break;
        }

    }

    return 0;
}


int main(int argc, char *argv[]) {

    if(argc !=5)
    {
        // add the 10 tweets length if the number of tweets is not specifieds
        fprintf(stderr, NUM_ARGS_ERROR);
        exit(EXIT_FAILURE);
    }

    // Parse command-line arguments
    int seed = atoi(argv[1]);
    int max_length = atoi(argv[2]);
    char *file_path = argv[3];
    int num_tweets = atoi(argv[4]);
    //LinkedList* all_words;

    if (max_length <= 0 || num_tweets <= 0) {
        printf("Error: max_length and num_tweets must be positive integers.\n");
        return 1;
    }

    // Seed the random number generator
    srand(seed);

    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (!markov_chain) {
        printf("Error: Memory allocation failed for Markov Chain.\n");
        return 1;
    }
    markov_chain->database = malloc(sizeof(LinkedList));
    if (!markov_chain->database) {
        printf("Error: Memory allocation failed for database.\n");
        free(markov_chain);
        return 1;
    }
    markov_chain->database->first = NULL;


    // Load the text corpus into the Markov Chain
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, FILE_PATH_ERROR);
        free(markov_chain->database);
        free(markov_chain);
        exit(EXIT_FAILURE);
    }

    int result =fill_database(file,num_tweets,markov_chain);

    if(result!=0)
    {
        printf("Database fill end with error!\n");
        exit(1);
    }
    MarkovNode* current = get_first_random_node(markov_chain);
    for(int i =0 ; i<max_length; i++)
    {
        printf("Tweet %d: ",i+1);
        generate_tweet(current,max_length);
        printf("\n"); // End the tweet
        sleep(1);
        current = get_first_random_node(markov_chain);
    }

    // Free resources
    free_database(&markov_chain);
    fclose(file);
    return 0;
}
