#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
	int data;
	struct node* next;
};


typedef struct node* Np;
typedef struct node N;

int IsStackEmpty(Np stack);
Np CreatStack(int numlist[], int len);
int pop(Np stack);
Np initialize(int data, Np next);
void push(Np stack, int data);
int Top(Np stack);