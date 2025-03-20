#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#define HASHTABLE_SIZE 10

typedef enum{FALSE,TRUE} bool ;

typedef struct var_tag{
    char character;
    int val;  
}var;

typedef struct scope_node_tag{
    //a hash table
    var hash_table[HASHTABLE_SIZE];
    //self referential pointer for a linked list 
    struct scope_node_tag* next;
    struct scope_node_tag* prev;
}scope_node;



int hash(char s){
    
    int i = 0;
    int h = 0 ;
    h = (31 * h + s) ;
    return h%HASHTABLE_SIZE ;
}

void init_hashtable(var hashtable[]){
    for(int i = 0; i<HASHTABLE_SIZE; i++){
        hashtable[i].val = 0;
    }
}
scope_node* createNode(){
    scope_node* lptr ;
    lptr = (scope_node*)malloc(sizeof(scope_node));
    lptr->next = NULL ;
    lptr->prev = NULL ;
    init_hashtable(lptr->hash_table);
    return lptr ;
}

bool hash_table_insert(char s,int value,var hashtable[]){
    int index = hash(s);
    if(hashtable[index].val!=0){return FALSE ;}
    hashtable[index].character = s ;
    hashtable[index].val = value;
    return TRUE ;
}

bool hash_table_lookup(char s , var hashtable[]){
    int index = hash(s);
    // printf("%d\n",hashtable[index].val);
    // printf("%c\n",hashtable[index].character);
    if((hashtable[index].val!=0)&&(hashtable[index].character == s)){
        return TRUE ;
    }
    else{
        return FALSE;
    }
}

int get_variable_value(char s, var hashtable[]){
    int index = hash(s);
    return hashtable[index].val ;
}

int main()
{
    scope_node* current_scope = NULL ;
    FILE *file = fopen("scoping_code.txt", "r");  
    char line[100];  // To store each line from the file

    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {  // Read line by line
        char command[10], var;
        int value;

        // Checking if it's an 'assign' command
        if (sscanf(line, "%s %c %d", command, &var, &value) == 3) {
            if (strcmp(command, "assign") == 0) {
                bool k = hash_table_insert(var,value, current_scope->hash_table);  // Insert into current scope
                if(k ==TRUE){
                    printf("Var inserted successfully\n");
                }
            }
        } 
        // Check if it's the 'begin' keyword
        else if (strncmp(line, "begin", 5) == 0) {  
            scope_node *new_scope = createNode();  // Create new scope
            new_scope->prev = current_scope;  // Link it to the previous scope
            if (current_scope != NULL) {
                current_scope->next = new_scope;
            }
            current_scope = new_scope; 
            printf("Created a new scope \n");
        }
        
        else if (strncmp(line, "end", 3) == 0) {
            if (current_scope != NULL) {
                scope_node* nptr = current_scope;
                current_scope = current_scope->prev;
                free(nptr) ;
                printf("Scope popped\n");
            }
        }

        else if (strncmp(line, "print", 5) == 0) {
            char var_name;
            sscanf(line, "print %c", &var_name);  // Extract the variable name
            scope_node* temp = current_scope;
            bool found = FALSE;
        
            while (temp != NULL) {  
                // printf("going to check condn\n");
                // bool m = hash_table_lookup(var_name, temp->hash_table);
                // if(m == TRUE){
                //     printf("Valid \n");
                // }
                // else{
                //     printf("Invalid\n");
                // }
                if (hash_table_lookup(var_name, temp->hash_table)) {  
                    int value = get_variable_value(var_name, temp->hash_table);
                    printf("%d\n", value);
                    found = TRUE;
                    break;
                }
                // printf("Outside check\n");
                temp = temp->prev;  
            }
        
            if (!found) {
                printf("Error: Variable %c is undefined\n", var_name);
            }
        }
        
    }

    fclose(file);
    return 0 ;
}