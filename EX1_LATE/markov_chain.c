#include "markov_chain.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> // Include for memcpy
#include <ctype.h>

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}


/**
* Check if data_ptr is in database. If so, return the Node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the data to look for
 * @return Pointer to the Node wrapping given data, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
    Node * start = markov_chain->database->first;
    Node * current = start;
    while (current != NULL){
        if(strcmp(current->data->data, data_ptr)==0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


int count_words_before_dot(const char *sentence) {
    int count = 0;
    int in_word = 0;

    while (*sentence && *sentence != '.') {
        if (isspace(*sentence)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        sentence++;
    }

    return count;
}

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the data to look for
 * @return Node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */
Node* add_to_database(MarkovChain *markov_chain, char *data_ptr)
{
    //
    LinkedList *db = markov_chain->database;  // Access database directly

    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    if (newNode == NULL) {
        // Handle allocation failure
        printf("Memory allocation failed\n");
        exit(1); // Exit or handle the error as appropriate
    }
    newNode->data = (MarkovNode *)malloc(sizeof(MarkovNode));
    newNode->data->data = (char* *)malloc(sizeof(char*));
    size_t data_size = strlen(data_ptr) + 1;
    // Assuming data_ptr and newNode->data are properly allocated and you know the size in bytes
    memcpy(newNode->data->data, data_ptr, data_size);

    newNode->next = NULL;

    if(db->first == NULL) {

        if(count_words_before_dot(newNode->data->data)>1)
        {
            db->first = newNode;
            db->size++;
            return db->first;
        }
    }
    struct Node* current = db->first;
    Node * search = get_node_from_database(markov_chain, data_ptr);
    if(search != NULL)
    {
        return search;
    }
    if(current!=NULL)
    {
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newNode;
        db->last = newNode;
        db->size++;
        db->last = current->next;
        return newNode;
    }
    return NULL;
}

/**
 * Add the second markov_node to the frequency list of the first markov_node.
 * If already in list, update it's occurrence frequency value.
 * @param first_node
 * @param second_node
 * @return success/failure: 0 if the process was successful, 1 if in
 * case of allocation error.
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode * second_node)
{

    if (!first_node || !second_node) {
        return 1; // Failure due to invalid input
    }

    MarkovNodeFrequency *current = first_node->frequency_list;
    MarkovNodeFrequency *previous = NULL;

    // Search for the node in the frequency list
    while (current != NULL) {
        if (strcmp(current->markov_node->data, second_node->data)==0) {
            // Node already exists, increment its frequency
            current->frequency++;
            return 0; // Success
        }
        previous = current;
        current = current->next;
    }

    // Node not found, create a new frequency node
    MarkovNodeFrequency *new_node = (MarkovNodeFrequency *)malloc(sizeof(MarkovNodeFrequency));
    if (!new_node) {
        return 1; // Failure due to memory allocation error
    }

    new_node->markov_node = second_node;
    new_node->frequency = 1;
    new_node->next = NULL;

    // Add the new node to the list
    if (previous == NULL) {
        first_node->frequency_list = new_node; // First node in the list
    } else {
        previous->next = new_node; // Append to the end of the list
    }

    return 0; // Success
}


/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database(MarkovChain ** ptr_chain)
{
    struct Node* current  = (*ptr_chain)->database->first;
    struct Node* nextNode;

    while (current != NULL) {
        nextNode = current->next;
        free(current);             // Free the current node
        current = nextNode;        // Move to the next node
    }

    *ptr_chain = NULL;  // Set head to NULL to indicate list is empty
}


/**
 * Get one random MarkovNode from the given markov_chain's database.
 * @param markov_chain
 * @return the random MarkovNode
 */
 MarkovNode* get_first_random_node(MarkovChain *markov_chain)
 {
     int random_number = get_random_number(markov_chain->database->size);
     printf("");
     Node* curNode = markov_chain->database->first;
     for(int i=0; i<random_number;i++)
     {
         curNode = curNode->next;
     }
     return curNode->data;
 }

/**
 * Choose randomly the next MarkovNode, depend on it's occurrence frequency.
 * @param cur_markov_node current MarkovNode
 * @return the next random MarkovNode
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    if (!cur_markov_node || !cur_markov_node->frequency_list) {
        return NULL; // Return NULL if input is invalid
    }
    // Calculate total frequency
    int total_frequency = 0;
    MarkovNodeFrequency *current = cur_markov_node->frequency_list;
    while (current) {

        total_frequency += current->frequency;
        current = current->next;
    }
    MarkovNode **array = (MarkovNode **)malloc(total_frequency * sizeof(MarkovNode *));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    MarkovNodeFrequency *temp = cur_markov_node->frequency_list;
    for (int i = 0; i < total_frequency; i++) {
        // Allocate memory for the string and copy it
        array[i] = temp->markov_node;
        if (array[i]  == NULL) {
            printf("Memory allocation failed for string %d.\n", i + 1);
            return NULL;
        }
        //strcpy(array[i] , temp->markov_node); // Copy the string
    }
    // Generate a random number between 0 and total_frequency - 1
    int random_value = get_random_number(total_frequency);
    return array[random_value];
}



// Function to check if a string ends with a '.' character
int ends_with_dot(const char *str) {
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


/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence must have at least 2 words in it.
 * @param first_node markov_node to start with
 * @param max_length maximum length of chain to generate
 */
void generate_tweet(MarkovNode *first_node, int max_length)
{
    if (!first_node || max_length <= 0) 
    {
        printf("Invalid input.\n");
        return;
    }
    //Seed random number generator
    srand((unsigned int)time(NULL));

    MarkovNode* current_node = first_node;
    int word_count = 0;
    char **tweets = (char **)malloc(max_length * sizeof(char *));
    int index= 0;
    // Generate the tweet
    int max_tweet_length = 20;
    while ((current_node && word_count < max_tweet_length)) {
        tweets[index] = current_node->data;
        // Print the current word
        printf("%s",current_node->data);
        // Stop if we've reached a terminal node (no transitions)
        //        if (!current_node->frequency_list) {
        //            break;
        //        }
        if (ends_with_dot(current_node->data)) {
            break;
        }
        // Choose the next node randomly based on frequencies
        current_node = get_next_random_node(current_node);
        // Add a space if there is a next node
        if (current_node) {
            printf(" ");
        }
        word_count++;
    }
}