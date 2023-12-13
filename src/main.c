#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_LEN 256
#define MAX_LCS_LEN 32
#define MAX_LCS_BUF_LEN 256

typedef struct {
    char *str;
} HashEntry;

typedef struct {
    int **mtx;
    int n;
    int m;
} Mtx;

typedef enum { CharEqual, UpBigger, LeftBigger, Equal } ChosenState;

/*
 @brief Prints a message and exits current
    process with failure error code

 @param msg Message to be printed

 @return
*/
void fail(char *msg) {
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

/*
 @brief Checks if a given number is prime

 @param num Number to be checked

 @return 1 if number is prime, 0 if it is not
*/
int checkPrime(int num) {
    int i;
    int isPrime = 1;
    if (num <= 3) {
        return 1;
    }
    for (i = 2; i * i <= num && isPrime == 1; ++i) {
        if (num % i == 0) {
            isPrime = 0;
        }
    }
    return isPrime;
}

/*
 @brief Computes m value given n and load
 @param n Number of elements
 @param lf Load factor

 @return m value to be used in hashing
*/
int compM(int n, double lf) {
    int quotient = (int)ceil(n / lf);
    int m;
    int isPrime = 0;
    m = quotient - 1;
    while (isPrime == 0) {
        ++m;
        isPrime = checkPrime(m);
    }
    return m;
}

/*
 @brief Converts a string to number key using Horner's rule

 @param str String to be converted
 @param strLen Number of chars in the string

 @return Converted number key for the string
*/
int strToNum(char *str, int strLen) {
    double num = 0;
    int i;
    int r = 3;
    for (i = 0; i < strLen; ++i) {
        int power = strLen - i - 1;
        double powerRes = pow(r, power);
        int charVal;
        charVal = str[i] - '0' + 1;
        num = num + powerRes * charVal;
    }
    while (num > INT_MAX) {
        num -= INT_MAX;
    }
    return (int)num;
}

/*
 @brief Computes output h1 function

 @param key Key to be used in hashing
 @param m Number of cells in hash table

 @return Output of h1 function
*/
int h1(int key, int m) { return key % m; }

/*
 @brief Computes output h2 function given

 @param key Key to be used in hashing
 @param m Number of cells in hash table

 @return Output of h2 function
*/
int h2(int key, int m) {
    int m2 = m - 2;
    return 1 + (key % m2);
}

/*
 @brief Computes index in the hash table based on h1 value,
    h2 value, index i, and m value

 @param h1Val Output of h1 function
 @param h2Val Output of h2 function
 @param i Current iteration counter that is used in hashing functions
 @param m Number of cells in hash table

 @return Computed index in the hash table
*/
int compHashIdx(int h1Val, int h2Val, int i, int m) {
    return (h1Val + i * h2Val) % m;
}

/*
 @brief Prints the given hash table

 @param hash Hash table
 @param m Number of cells in hash table

 @return
*/
void printHash(HashEntry *hash, int m) {
    int i;
    printf("\nHash table:\n");
    for (i = 0; i < m; ++i) {
        if (hash[i].str != 0) {
            printf("\t%d:\t%s\n", i, hash[i].str);
        } else {
            printf("\t%d:\t(null)\n", i);
        }
    }
}

/*
 @brief Frees the hash table and its substructures

 @param hash Hash table
 @param m Number of cells in hash table

 @return
*/
void freeHash(HashEntry *hash, int m) {
    int i;
    for (i = 0; i < m; ++i) {
        free(hash[i].str);
    }
    free(hash);
}

/*
 @brief Adds a str to the hash table

 @param hash Hash table
 @param m Number of cells in hash table
 @param str String to be added
 @param strLen Length of str to be added

 @return Index of the newly added element, -1 if could not add
*/
int add(HashEntry *hash, int m, char *str, int strLen) {
    int key;
    int i = 0;
    int inserted = 0;
    int hashIdx;
    int h1Val;
    int h2Val;
    int exists = 0;
    key = strToNum(str, strLen);
    h1Val = h1(key, m);
    h2Val = h2(key, m);
    while (inserted == 0 && exists == 0 && i < m) {
        hashIdx = compHashIdx(h1Val, h2Val, i, m);
        if (hash[hashIdx].str == 0) {
            hash[hashIdx].str = calloc(strLen + 1, sizeof(char));
            if (hash[hashIdx].str == NULL) {
                fail("add: malloc failed");
            }
            strncpy(hash[hashIdx].str, str, strLen);
            inserted = 1;
        } else if (strcmp(hash[hashIdx].str, str) == 0) {
            exists = 1;
        }
        ++i;
    }
    if (inserted == 1) {
    } else if (exists == 1) {
        hashIdx = -1;
    } else {
        hashIdx = -1;
    }
    return hashIdx;
}

/*
 @brief Searches a string from hash table

 @param hash Hash table
 @param m Number of cells in hash table
 @param str String to be searched
 @param strLen Length of the string

 @return 0 if string is not in hash, 1 if string is in hash
*/
int search(HashEntry *hash, int m, char *str, int strLen) {
    int key;
    int h1Val;
    int h2Val;
    int found = 0;
    int i = 0;
    int hashIdx;
    if (str == NULL) {
        fail("search: calloc failed");
        exit(EXIT_FAILURE);
    }
    if (strLen >= MAX_LCS_LEN) {
        str[MAX_LCS_LEN] = 0;
        strLen = MAX_LCS_LEN;
    }
    key = strToNum(str, strLen);
    h1Val = h1(key, m);
    h2Val = h2(key, m);
    while (found == 0 && i < m) {
        hashIdx = compHashIdx(h1Val, h2Val, i, m);
        if (hash[hashIdx].str != 0 && strcmp(hash[hashIdx].str, str) == 0) {
            found = 1;
        }
        ++i;
    }
    return found;
}

/*
 @brief Takes a string from stdin after printing a message

 @param msg Message to be printed

 @return Input from stdin
*/
char *takeStr(char *msg) {
    char *str = malloc(MAX_BUF_LEN * sizeof(char));
    if (str == NULL) {
        fail("takeStr: malloc failed");
    }
    printf("%s", msg);
    scanf(" %s", str);
    return str;
}

/*
 @brief Allocate a zero filled matrix

 @param n Length of string 2
 @param m Length of string 1

 @return Allocated matrix
*/
Mtx *makeMtx(int n, int m) {
    int i;
    Mtx *mtx = calloc(1, sizeof(Mtx));
    if (mtx == NULL) {
        fail("makeMtx: calloc failed");
    }
    mtx->n = n;
    mtx->m = m;
    mtx->mtx = calloc(n, sizeof(int *));
    if (mtx->mtx == NULL) {
        fail("makeMtx: calloc failed");
    }
    for (i = 0; i < n; ++i) {
        mtx->mtx[i] = calloc(m, sizeof(int));
        if (mtx->mtx[i] == NULL) {
            fail("makeMtx: calloc failed");
        }
    }
    return mtx;
}

/*
 @brief Prints a matrix

 @param mtx Pointer to mtx to be printed

 @return
*/
void printMtx(Mtx *mtx) {
    int i;
    int j;
    for (i = 0; i < mtx->n; ++i) {
        for (j = 0; j < mtx->m; ++j) {
            printf("%d ", mtx->mtx[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/*
 @brief Frees the matrix

 @param mtx Pointer to mtx to be freed

 @return
*/
void freeMtx(Mtx *mtx) {
    int i;
    for (i = 0; i < mtx->n; ++i) {
        free(mtx->mtx[i]);
    }
    free(mtx->mtx);
    free(mtx);
}

/*
 @brief Finds bigger of two values

 @param num1 First number
 @param num1 Second number

 @return Bigger of values
*/
int max(int num1, int num2) {
    if (num1 > num2) {
        return num1;
    } else {
        return num2;
    }
}

void printTables(Mtx *mtx, Mtx *chosen, int i) {
    printf("Length matrix after filling row %d:\n\n", i);
    printMtx(mtx);
    printf("Chosen matrix after filling row %d:\n\n", i);
    printMtx(chosen);
    printf("------------------------------------------\n\n");
}

/*
 @brief Fills the Dynamic Programming table with subsolutions

 @param mtx DP matrix to be filled with solutions
 @param chosen Matrix that holds whether each character of string 1 and string 2
 is chosen or passed over
 @param str1 First string
 @param len1 Length of first string
 @param str2 Second string
 @param len2 Length of second string

 @return
*/
void fillMtx(Mtx *mtx, Mtx *chosen, char *str1, int len1, char *str2,
             int len2) {
    int i, j;
    for (i = 1; i < mtx->n; ++i) {
        for (j = 1; j < mtx->m; ++j) {
            int up = mtx->mtx[i - 1][j];
            int left = mtx->mtx[i][j - 1];
            int upperLeft = mtx->mtx[i - 1][j - 1];
            int maxNeighbor = max(up, left);
            if (str1[j - 1] == str2[i - 1]) {
                mtx->mtx[i][j] = upperLeft + 1;
                chosen->mtx[i][j] = CharEqual;
            } else {
                mtx->mtx[i][j] = maxNeighbor;
                if (up > left) {
                    chosen->mtx[i][j] = UpBigger;
                } else if (up < left) {
                    chosen->mtx[i][j] = LeftBigger;
                } else if (up == left) {
                    chosen->mtx[i][j] = Equal;
                }
            }
        }
        printTables(mtx, chosen, i);
    }
}

/*
 @brief Prints all solutions

 @param chosen Matrix that holds whether each character of string 1 and string 2
 is chosen or passed over
 @param str1 First string
 @param len1 Length of first string
 @param str2 Second string
 @param len2 Length of second string
 @param i Horizontal index
 @param j Vertical index
 @param lcs Current solution string
 @param lcsLen Current length of lcs
 @param hash Hash that holds solutions
 @param m Length of hash table
 @param longestLen Length of the longest solution

 @return
*/
void printLcs(Mtx *chosen, char *str1, int len1, char *str2, int len2, int i,
              int j, char *lcs, int lcsLen, HashEntry *hash, int m,
              int longestLen) {
    if (i == 0 || j == 0) {
        if (search(hash, m, lcs, lcsLen) == 0) {
            printf("%s\n", lcs);
            add(hash, m, lcs, longestLen);
        }
        return;
    }
    if (chosen->mtx[i][j] == CharEqual) {
        lcs[lcsLen - 1] = str1[j - 1];
        printLcs(chosen, str1, len1, str2, len2, i - 1, j - 1, lcs, lcsLen - 1,
                 hash, m, longestLen);
    } else if (chosen->mtx[i][j] == UpBigger) {
        printLcs(chosen, str1, len1, str2, len2, i - 1, j, lcs, lcsLen, hash, m,
                 longestLen);
    } else if (chosen->mtx[i][j] == LeftBigger) {
        printLcs(chosen, str1, len1, str2, len2, i, j - 1, lcs, lcsLen, hash, m,
                 longestLen);
    } else {  // chosen->mtx[i][j] == Equal
        printLcs(chosen, str1, len1, str2, len2, i - 1, j, lcs, lcsLen, hash, m,
                 longestLen);
        printLcs(chosen, str1, len1, str2, len2, i, j - 1, lcs, lcsLen, hash, m,
                 longestLen);
    }
}

/*
 @brief Main program that gets inputs (string 1 and string 2) from user
    and runs the algorithm and prints the results

 @return Error code (0 if returned successfully)
*/
int main() {
    char resp;
    do {
        char *str1 = takeStr("\nEnter string 1: ");
        char *str2 = takeStr("Enter string 2: ");
        int len1 = strlen(str1);
        int len2 = strlen(str2);
        int m = len1 + 1;
        int n = len2 + 1;
        Mtx *mtx = makeMtx(n, m);
        Mtx *chosen = makeMtx(n, m);
        int lcsSize = max(len1, len2) + 1;
        int mHash = compM(lcsSize, 1.0);
        char *lcs = malloc(sizeof(lcsSize * sizeof(char)));
        int longestLen;
        HashEntry *hash;
        if (lcs == NULL) {
            fail("main: malloc failed");
        }
        hash = calloc(mHash, sizeof(HashEntry));
        if (hash == NULL) {
            fail("main: calloc failed");
        }
        printf("\nInitial state of matrix: \n\n");
        printMtx(mtx);
        fillMtx(mtx, chosen, str1, len1, str2, len2);
        longestLen = mtx->mtx[mtx->n - 1][mtx->m - 1];
        lcs[longestLen] = 0;
        printf("Length of the longest common subsequence: %d\n\n", longestLen);
        printf("Longest common subsequences: \n\n");
        printLcs(chosen, str1, len1, str2, len2, mtx->n - 1, mtx->m - 1, lcs,
                 longestLen, hash, mHash, longestLen);
        printf("\n");
        freeHash(hash, mHash);
        free(lcs);
        freeMtx(chosen);
        freeMtx(mtx);
        free(str1);
        free(str2);
        printf("Continue? (enter 'y' for yes): ");
        scanf(" %c", &resp);
    } while (resp == 'y');
    return 0;
}
