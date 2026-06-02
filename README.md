# CSE 321: Operating Systems — Lab Assignment 1

This repository contains the C implementations for Lab Assignment 1, focusing on multithreading and process synchronization using the POSIX threads (`pthread`) library.

---

## Files Included

* **`problem1.c`**: Multithreaded Fibonacci Sequence Generator and Search.
* **`problem2.c`**: The Sandwich Maker Synchronization Problem.

---

## Compilation and Execution

Both programs require linking the `pthread` library during compilation. Use the following commands in your terminal:

### Problem 1: Fibonacci Computation and Search

```bash
gcc -o problem1 problem1.c -lpthread
./problem1

```

### Problem 2: The Sandwich Maker Synchronization

```bash
gcc -o problem2 problem2.c -lpthread
./problem2

```

---

## Detailed Project Overview

### Problem 1: Multithreaded Fibonacci Computation and Search

This program showcases basic thread creation, waiting (`pthread_join`), and dynamic memory sharing between threads.

* **Thread 1 (Generator):** Takes an integer input $n$ ($0 \le n \le 40$) from the user, dynamically allocates memory on the heap, generates the Fibonacci sequence up to the $n$-th term, and returns the array pointer to the main thread.
* **Thread 2 (Searcher):** Receives the generated sequence and performs a user-defined number of queries to search for specific indices, returning the Fibonacci value or `-1` if the index is out of bounds.

### Problem 2: The Sandwich Maker Synchronization Problem

A variation of the classic concurrent "Cigarette Smokers Problem" utilizing **Mutex Locks** and **POSIX Semaphores** to eliminate busy-waiting, deadlocks, and starvation.

* **The Supplier:** Randomly (deterministically matched to sample output via `round % 3`) places two ingredients on a shared table and signals the appropriate maker.
* **The Makers (A, B, C):** Each possesses an infinite supply of one unique ingredient (Bread, Cheese, or Lettuce). They wait for the supplier's signal, safely grab the missing components using mutual exclusion, assemble/eat the sandwich, and signal the supplier that the table is clear.

> **Note:** The maker threads run in a continuous loop. Once the supplier completes its specified $N$ rounds, the main program safely cleans up all synchronization constructs and terminates.
