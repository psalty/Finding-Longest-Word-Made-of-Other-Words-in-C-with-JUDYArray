/*
 * Finding compund word in dictionary
 * Copyright Jaehong Park
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <judy.h>

#define MAXLINE 		100  
#define MIN_WORD_LEN 	2

static void create_index(char *filename);
static int string_prime_div(char *str, int count);
static void remove_eol(char *str);
static void substring(char *newstr, char *str, int count);
static int find_word(char *str);

// global mapping table
Pvoid_t   	PJArray = (PWord_t)NULL;  // Judy array.
uint8_t 	Index[MAXLINE];

int main(int argc, void **argv)
{
	char buffer[MAXLINE];
	Word_t    Bytes;

   	FILE *fp = NULL;
   	int result =0;
   	int count = 0;	
   	int i,j;
	int start_len;

	if(argc < 2) {
		printf("provide file to search\n"); 
		return;
	}
	
	char *file = argv[1];

	// populate map and new sorted list
   	create_index(file);

    fp = fopen(file, "r");
    if (fp == NULL) {
        perror("no sorted list exist");
        goto process_done;
    }
	// loop through the list from the longest word
    while(fgets(buffer, sizeof(buffer), fp) !=NULL) {
		remove_eol(buffer);
		start_len = strlen(buffer);
		while(1) {
			if (start_len < MIN_WORD_LEN*MIN_WORD_LEN)
				break;
			if (string_prime_div(buffer, start_len - 1)) {
				printf("%d.%s\n", count, buffer);
				count++;
				break;
			} else {
				start_len--;
			}
		}
	}
    printf("total %d of words found that composed of other word in the list\n", count);
	JSLFA(Bytes, PJArray); 
	fclose(fp);

process_done:
    return 0;
}

// remove EOL character and replace with \0
void remove_eol(char *str)
{
	int len = strlen(str);
	if (str[len - 2] == '\r') {
		str[len - 2] = '\0';
	} else if (str[len - 1] == '\n') {
		str[len - 1] = '\0';
	}
}

void create_index(char *filepath)
{
    PWord_t   PValue;                   // Judy array element.
    Word_t    Bytes;                    // size of JudySL array.
	char   	  buffer[MAXLINE];             // string to insert
	FILE *fp;

    fp = fopen(filepath, "r");

    if (fp == NULL) {
        perror ("db open failure");
        return;
    }

    while(fgets(buffer, sizeof(buffer), fp) != NULL) {
        remove_eol(buffer);
		JSLI(PValue, PJArray, buffer);   // store string into array

        if (PValue == PJERR)            // if out of memory?
        {                               // so do something
            printf("Malloc failed -- get more ram\n");
            exit(1);
        }
        ++(*PValue);                    // count instances of string
    }
    fclose(fp);
}

// factorization of string like finding prime number with given number
int string_prime_div(char *str, int count)
{
    PWord_t   PValue;                   // Judy array element.
	int len = strlen(str);
	int result = 0;
	int found  = 0;
	char keystr[MAXLINE];

	if(len == 0){
		return 1;
	} else if (len  == 1 || count <+ 2) {
		return 0;
	}

    memset(keystr, 0, sizeof(keystr));
    substring(keystr, str, count);
	while(strlen(keystr) >= MIN_WORD_LEN) {
		if(find_word(keystr)) {
			result = string_prime_div(str+strlen(keystr), len - strlen(keystr));
			if(result)
				return 1;
		}
		memset(keystr, 0, sizeof(keystr));
		substring(keystr, str, --count);
	}
	return 0;
}

int find_word(char *str)
{
	PWord_t  PValue;      
	memset(Index, 0, sizeof(Index));
	strncpy(Index, str, strlen(str));

	JSLG(PValue, PJArray, Index);

	if(PValue != NULL) {
		return 1;        
	}	

	return 0;
}

void substring(char *newstr, char *str, int count)
{
	if (str == NULL || count== 0)
		return;
	strncpy(newstr, str, count);
}
