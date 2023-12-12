#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_LEN 256

typedef struct {
    int **mtx;
    int n;
    int m;
} Mtx;

void fail(char *msg) {
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

char *takeStr(char *msg) {
    char *str = malloc(MAX_BUF_LEN * sizeof(char));
    if (str == NULL) {
        fail("takeStr: malloc failed");
    }
    printf("%s", msg);
    scanf(" %s", str);
    return str;
}

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

void freeMtx(Mtx *mtx) {
    int i;
    for (i = 0; i < mtx->n; ++i) {
        free(mtx->mtx[i]);
    }
    free(mtx->mtx);
    free(mtx);
}

int max(int num1, int num2) {
    if (num1 > num2) {
        return num1;
    } else {
        return num2;
    }
}

int max3(int num1, int num2, int num3) {
    if (num3 > max(num1, num2)) {
        return num3;
    } else {
        return max(num1, num2);
    }
}

void fillMtx(Mtx *mtx, Mtx *chosen, char *str1, int len1, char *str2,
             int len2) {
    int i, j;
    for (i = 1; i < mtx->n; ++i) {
        for (j = 1; j < mtx->m; ++j) {
            if (str1[j - 1] == str2[i - 1]) {
                mtx->mtx[i][j] = mtx->mtx[i - 1][j - 1] + 1;
                chosen->mtx[i][j] = 0;
            } else if (mtx->mtx[i - 1][j] > mtx->mtx[i][j - 1]) {
                mtx->mtx[i][j] = mtx->mtx[i - 1][j];
                chosen->mtx[i][j] = 1;
            } else if (mtx->mtx[i - 1][j] < mtx->mtx[i][j - 1]) {
                mtx->mtx[i][j] = mtx->mtx[i][j - 1];
                chosen->mtx[i][j] = 2;
            } else {
                mtx->mtx[i][j] = mtx->mtx[i][j - 1];
                chosen->mtx[i][j] = 3;
            }
        }
        printf("Length matrix after filling row %d:\n\n", i);
        printMtx(mtx);
        printf("Chosen matrix after filling row %d:\n\n", i);
        printMtx(chosen);
        printf("------------------------------------------\n\n");
    }
}

void printLongestLen(Mtx *mtx) {
    printf("Length of the longest common subsequence: %d\n\n",
           mtx->mtx[mtx->n - 1][mtx->m - 1]);
}

void printLcs(Mtx *chosen, char *str1, int len1, char *str2, int len2, int i,
              int j, char *lcs, int lcsLen) {
    // printf("printLcs: called with i: %d, j: %d\n", i, j);
    if (i == 0 || j == 0) {
        printf("%s\n", lcs);
        return;
    }
    if (chosen->mtx[i][j] == 0) {
        lcs[lcsLen - 1] = str1[j - 1];
        printLcs(chosen, str1, len1, str2, len2, i - 1, j - 1, lcs, lcsLen - 1);
    } else if (chosen->mtx[i][j] == 1) {
        printLcs(chosen, str1, len1, str2, len2, i - 1, j, lcs, lcsLen);
    } else if (chosen->mtx[i][j] == 2) {
        printLcs(chosen, str1, len1, str2, len2, i, j - 1, lcs, lcsLen);
    } else {
        printLcs(chosen, str1, len1, str2, len2, i - 1, j, lcs, lcsLen);
        printLcs(chosen, str1, len1, str2, len2, i, j - 1, lcs, lcsLen);
    }
}

int main() {
    char *str1 = takeStr("Enter string 1: ");
    char *str2 = takeStr("Enter string 2: ");
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int m = len1 + 1;
    int n = len2 + 1;
    Mtx *mtx = makeMtx(n, m);
    Mtx *chosen = makeMtx(n, m);
    int lcsSize = max(len1, len2) + 1;
    char *lcs = malloc(sizeof(lcsSize * sizeof(char)));
    if (lcs == NULL) {
        fail("main: malloc failed");
    }
    printf("\nInitial state of matrix: \n\n");
    printMtx(mtx);
    fillMtx(mtx, chosen, str1, len1, str2, len2);
    int longestLen = mtx->mtx[mtx->n - 1][mtx->m - 1];
    lcs[longestLen] = 0;
    printf("Length of the longest common subsequence: %d\n\n", longestLen);
    printf("Longest common subsequences: \n\n");
    printLcs(chosen, str1, len1, str2, len2, mtx->n - 1, mtx->m - 1, lcs,
             longestLen);
    printf("\n");
    free(lcs);
    freeMtx(chosen);
    freeMtx(mtx);
    free(str1);
    free(str2);
    return 0;
}
