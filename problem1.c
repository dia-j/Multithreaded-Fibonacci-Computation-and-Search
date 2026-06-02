/*
 * CSE 321 - Lab Assignment 1
 * Problem 1: Multithreaded Fibonacci Computation and Search
 *
 * Compile : gcc -o problem1 problem1.c -lpthread
 * Run     : ./problem1
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* -------------------------------------------------------
 * Structs to pass data into threads
 * ------------------------------------------------------- */

/* Data needed by the fibonacci generator thread */
typedef struct {
    int   total;        /* how many terms to compute (0 .. total) */
} GenData;

/* Data needed by the search thread */
typedef struct {
    long long *seq;     /* pointer to the computed fibonacci array  */
    int        limit;   /* highest valid index in the array          */
    int       *queries; /* array of indices the user wants to find   */
    int        qcount;  /* number of queries                         */
    long long *answers; /* results are stored here by the thread     */
} SearchData;


/* -------------------------------------------------------
 * Thread 1 function — builds the fibonacci sequence
 * Allocates memory on the heap and returns the pointer
 * ------------------------------------------------------- */
void *build_fibonacci(void *input)
{
    GenData *info = (GenData *)input;
    int size = info->total;

    /* make room for indices 0 through size */
    long long *arr = (long long *)malloc((size + 1) * sizeof(long long));

    /* fill in the sequence the usual way */
    if (size >= 0) arr[0] = 0;
    if (size >= 1) arr[1] = 1;

    int k;
    for (k = 2; k <= size; k++) {
        arr[k] = arr[k - 1] + arr[k - 2];
    }

    /* returning the pointer so main can grab it after join */
    return (void *)arr;
}


/* -------------------------------------------------------
 * Thread 2 function — looks up values in the sequence
 * Writes -1 for any index that is out of range
 * ------------------------------------------------------- */
void *lookup_values(void *input)
{
    SearchData *sd = (SearchData *)input;
    int i;

    for (i = 0; i < sd->qcount; i++) {
        int pos = sd->queries[i];

        /* check if the index is within the valid range */
        if (pos >= 0 && pos <= sd->limit) {
            sd->answers[i] = sd->seq[pos];
        } else {
            sd->answers[i] = -1;  /* invalid index */
        }
    }

    return NULL;
}


/* -------------------------------------------------------
 * main
 * ------------------------------------------------------- */
int main(void)
{
    /* step 1: ask user how many terms */
    int n;
    printf("Enter the term of fibonacci sequence:\n");
    scanf("%d", &n);

    /* step 2: launch thread 1 to generate the sequence */
    GenData  gd;
    gd.total = n;

    pthread_t tid1;
    void *ret_ptr = NULL;

    pthread_create(&tid1, NULL, build_fibonacci, &gd);
    pthread_join(tid1, &ret_ptr);   /* ret_ptr now holds the array */

    long long *fib_seq = (long long *)ret_ptr;

    /* print the sequence */
    int i;
    for (i = 0; i <= n; i++) {
        printf("a[%d] = %lld\n", i, fib_seq[i]);
    }

    /* step 3: get search queries from the user */
    int num_q;
    printf("How many numbers you are willing to search?:\n");
    scanf("%d", &num_q);

    int       *query_list  = (int *)      malloc(num_q * sizeof(int));
    long long *result_list = (long long *)malloc(num_q * sizeof(long long));

    for (i = 0; i < num_q; i++) {
        printf("Enter search %d:\n", i + 1);
        scanf("%d", &query_list[i]);
    }

    /* step 4: launch thread 2 to perform the searches */
    SearchData sd;
    sd.seq     = fib_seq;
    sd.limit   = n;
    sd.queries = query_list;
    sd.qcount  = num_q;
    sd.answers = result_list;

    pthread_t tid2;
    pthread_create(&tid2, NULL, lookup_values, &sd);
    pthread_join(tid2, NULL);

    /* step 5: show the results */
    for (i = 0; i < num_q; i++) {
        printf("result of search #%d = %lld\n", i + 1, result_list[i]);
    }

    /* step 6: free everything */
    free(fib_seq);
    free(query_list);
    free(result_list);

    return 0;
}
