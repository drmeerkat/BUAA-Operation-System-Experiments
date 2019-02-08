#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node {
	int data;
	struct node* link;
};

typedef struct node* Np;
typedef struct node N;

int IsEmpty(Np);
Np creat(int numlist[], int len);
Np find(int, Np);
char* delete(int, Np);
int insert(int, Np);
void deleteall(Np);

int IsEmpty(Np head) {
	if (head == NULL) {
		printf("the node doesn't exist");
		return 1;
	}
	return head->link == NULL;
}

Np creat(int numlist[], int len) {
	Np temp, head, pre_node;
	int i;
	head = (Np)malloc(sizeof(N));
	head->link = NULL;
	pre_node = head;
	for (i = 0; i < len; i++) {
		if (numlist[i] == 0) break;
		temp = (Np)malloc(sizeof(int));
		temp->link = NULL;
		temp->data = numlist[i];
		pre_node->link = temp;
		pre_node = temp;
	}

	return head;
}

Np find(int num, Np head) {
	Np temp;
	temp = head->link;
	while (temp != NULL && temp->data != num) {
		temp = temp->link;
	}

	return temp;
}

char* delete(int num, Np head) {
	Np position, temp;
	position = head->link;
	
	while (position->link != NULL) {
		if (position->link->data == num) {
			temp = position->link;
			position->link = temp->link;
			free(temp);
			printf("delete success\n");
		}
		position = position->link;
	}
	printf("element doesn't exist\n");
}

int insert(int element, Np posi) {
	Np temp;
	if (posi == NULL)
		return 0;

	temp = (Np)malloc(sizeof(N));
	temp->data = element;
	temp->link = posi->link;
	posi->link = temp;
	return 1;

}

void deleteall(Np head) {
	Np temp, posi;
	
	posi = head->link;
	head->link = NULL; 
	while (posi != NULL) {
		temp = posi;
		posi = posi->link;
		free(temp);
	}
	
}


int maintest1() {
	Np head = NULL, temp = NULL;
	int a[10] = {1,2,3,4,5,6};
	
	head = creat(a, sizeof(a)/sizeof(a[0]));
	temp = find(3, head);
	printf("find %d\n", temp->data);
	delete(3, head);
	temp = find(3, head);
	temp = find(4, head);
	if (insert(10, temp)) {
		printf("insert success\n");
		for (temp = head->link; temp != NULL; temp = temp->link) {
			printf("the data is %d\n", temp->data);
		}
	}

	deleteall(head);
	if (IsEmpty(head))printf("free all spaces success\n");


	system("pause");
	return 0;
}