/* map.c
 * ----------------------------------------------------------
 *  CS350
 *  Assignment 1
 *  Question 1
 *
 *  Purpose:  Gain experience with threads and basic 
 *  synchronization.
 *
 *  YOU MAY ADD WHATEVER YOU LIKE TO THIS FILE.
 *  YOU CANNOT CHANGE THE SIGNATURE OF CountOccurrences.
 * ----------------------------------------------------------
 */
#include "map.h"

// Global : num - the number of occurence of the target word; w - the word being searched; 
//          done - num of threads done
int volatile num, done;
char* volatile w;

//Lock : on the num, Conditional variable: on done
pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

//countArt:
// (void*)struct Article* -> (void*) NULL
//counts the occurence of the word in an article, then adds it to num

void* countArt(void* art){
    struct Article* article = (struct Article*) art;
    unsigned int numW = article->numWords;
    char ** words = article->words;
    int artC = 0;
    for (size_t i = 0; i < numW; i++){
        if(strcmp(words[i], w) == 0)
            artC++;
    }
    pthread_mutex_lock(&lk);
    num += artC;
    done--;
    if(done == 0){
        pthread_cond_signal(&cv);
    }
    pthread_mutex_unlock(&lk);
    pthread_exit(NULL);
    return NULL;
}


/* --------------------------------------------------------------------
 * CountOccurrences
 * --------------------------------------------------------------------
 * Takes a Library of articles containing words and a word.
 * Returns the total number of times that the word appears in the 
 * Library.
 *
 * For example, "There the thesis sits on the theatre bench.", contains
 * 2 occurences of the word "the".
 * --------------------------------------------------------------------
 */
int CountOccurrences( struct  Library * lib, char * word )
{
    w = word;
    done = lib->numArticles;
    pthread_t sthreads[lib->numArticles];
    for (size_t i = 0; i < lib->numArticles; i++){
        pthread_create(&sthreads[i], NULL, countArt, (void*)lib->articles[i]);
    }
    pthread_mutex_lock(&lk);
    while(done != 0){
        pthread_cond_wait(&cv, &lk);
    }
    pthread_mutex_unlock(&lk);
    return num;
    
    return 0;
}

