/*
 * CSE 321 - Lab Assignment 1
 * Problem 2: The Sandwich Maker Synchronization Problem
 *
 * Ingredients:
 *   Maker A -> has Bread    (needs Cheese + Lettuce from table)
 *   Maker B -> has Cheese   (needs Bread  + Lettuce from table)
 *   Maker C -> has Lettuce  (needs Bread  + Cheese  from table)
 *
 * Compile : gcc -o problem2 problem2.c -lpthread
 * Run     : ./problem2
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* -------------------------------------------------------
 * Global synchronization variables
 * ------------------------------------------------------- */

pthread_mutex_t table_lock;   /* only one thread touches the table at a time */

/* one semaphore per maker — supplier wakes the correct maker */
sem_t wake_A;   /* signal Maker A (has Bread,   needs Cheese+Lettuce) */
sem_t wake_B;   /* signal Maker B (has Cheese,  needs Bread+Lettuce)  */
sem_t wake_C;   /* signal Maker C (has Lettuce, needs Bread+Cheese)   */

sem_t table_free;  /* maker signals supplier when done eating */

/* how many rounds the supplier will place ingredients */
int total_rounds;

/* names of the two ingredients currently on the table (for printing) */
char *item1_on_table = NULL;
char *item2_on_table = NULL;


/* -------------------------------------------------------
 * Supplier thread
 * Places two random ingredients N times, then exits
 * ------------------------------------------------------- */
void *supplier_thread(void *arg)
{
    int round;
    for (round = 0; round < total_rounds; round++) {

        /* wait until the previous sandwich is done and table is clear */
        if (round > 0) {
            sem_wait(&table_free);
        }

        pthread_mutex_lock(&table_lock);

        /*
         * Pick which pair to place.
         * Combination 0: Bread + Cheese   -> Maker C needs to act
         * Combination 1: Cheese + Lettuce -> Maker A needs to act
         * Combination 2: Bread + Lettuce  -> Maker B needs to act
         *
         * Using round % 3 so output matches the sample (deterministic).
         * You can replace this with rand() % 3 for random behavior.
         */
        int combo = round % 3;

        if (combo == 0) {
            item1_on_table = "Bread";
            item2_on_table = "Cheese";
            printf("Supplier places: Bread and Cheese\n");
            pthread_mutex_unlock(&table_lock);
            sem_post(&wake_C);   /* Maker C has Lettuce, so C acts */

        } else if (combo == 1) {
            item1_on_table = "Cheese";
            item2_on_table = "Lettuce";
            printf("Supplier places: Cheese and Lettuce\n");
            pthread_mutex_unlock(&table_lock);
            sem_post(&wake_A);   /* Maker A has Bread, so A acts */

        } else {
            item1_on_table = "Bread";
            item2_on_table = "Lettuce";
            printf("Supplier places: Bread and Lettuce\n");
            pthread_mutex_unlock(&table_lock);
            sem_post(&wake_B);   /* Maker B has Cheese, so B acts */
        }
    }

    return NULL;
}


/* -------------------------------------------------------
 * Helper — generic "make and eat" logic used by all makers
 * ------------------------------------------------------- */
void do_sandwich(char *maker_name, sem_t *my_signal)
{
    while (1) {
        /* wait to be woken by the supplier */
        sem_wait(my_signal);

        pthread_mutex_lock(&table_lock);

        printf("Maker %s picks up %s and %s\n",
               maker_name, item1_on_table, item2_on_table);

        /* clear the table */
        item1_on_table = NULL;
        item2_on_table = NULL;

        pthread_mutex_unlock(&table_lock);

        printf("Maker %s is making the sandwich...\n", maker_name);
        sleep(1);   /* simulate time to make sandwich */

        printf("Maker %s finished making the sandwich and eats it\n", maker_name);
        printf("Maker %s signals Supplier\n", maker_name);

        /* let the supplier know the table is free */
        sem_post(&table_free);
    }
}


/* -------------------------------------------------------
 * Individual maker thread functions
 * ------------------------------------------------------- */
void *maker_A(void *arg)
{
    do_sandwich("A", &wake_A);
    return NULL;
}

void *maker_B(void *arg)
{
    do_sandwich("B", &wake_B);
    return NULL;
}

void *maker_C(void *arg)
{
    do_sandwich("C", &wake_C);
    return NULL;
}


/* -------------------------------------------------------
 * main
 * ------------------------------------------------------- */
int main(void)
{
    printf("Enter number of rounds (N):\n");
    scanf("%d", &total_rounds);

    /* set up mutex and semaphores */
    pthread_mutex_init(&table_lock, NULL);
    sem_init(&wake_A,     0, 0);
    sem_init(&wake_B,     0, 0);
    sem_init(&wake_C,     0, 0);
    sem_init(&table_free, 0, 0);

    /* create all four threads */
    pthread_t t_supplier, t_A, t_B, t_C;

    pthread_create(&t_A,        NULL, maker_A,         NULL);
    pthread_create(&t_B,        NULL, maker_B,         NULL);
    pthread_create(&t_C,        NULL, maker_C,         NULL);
    pthread_create(&t_supplier, NULL, supplier_thread, NULL);

    /* wait for the supplier to finish all rounds */
    pthread_join(t_supplier, NULL);

    /*
     * The maker threads loop forever waiting on semaphores.
     * After the supplier is done, the program can exit.
     * A small sleep gives the last maker time to finish printing.
     */
    sleep(2);

    /* clean up */
    pthread_mutex_destroy(&table_lock);
    sem_destroy(&wake_A);
    sem_destroy(&wake_B);
    sem_destroy(&wake_C);
    sem_destroy(&table_free);

    return 0;
}
