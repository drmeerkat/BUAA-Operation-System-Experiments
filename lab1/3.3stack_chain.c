#include "test.h"


int IsStackEmpty(Np stack) {
	return stack->next == NULL;
}

Np initialize(int data, Np next) {
	Np temp;
	temp = (Np)malloc(sizeof(N));
	temp->data = data;
	temp->next = next;
	return temp;
}

Np CreatStack(int numlist[], int len) {
	Np temp, posi, head;
	int i;
	head = initialize(0, NULL);
	posi = head;
	for (i = 0; i < len; i++, posi = posi->next) {
		temp = initialize(numlist[i], NULL);
		posi->next = temp;
	}

	return head;
}

int pop(Np stack) {
	Np temp;
	int data;
	if (IsEmpty(stack)) {
		printf("the stack is already empty");
		return NULL;
	}
	else {
		temp = stack->next;
		data = temp->data;
		stack->next = stack->next->next;
		free(temp);
		return data;
	}
}

void push(Np stack, int data) {
	Np temp;
	temp = initialize(data, stack->next);
	stack->next = temp;
}

int Top(Np stack) {
	if (IsStackEmpty(stack)) {
		printf("the stack is empty");
		return NULL;
	}
	return stack->next->data;
}




int main() {
	int a[5] = { 2,3,4,5,6 };
	int data, i;
	Np stack;
	stack = CreatStack(a, 5);
	printf("the top of the stack is %d\n", Top(stack));
	push(stack, 8);
	printf("after push, the top of the stack is %d\n", Top(stack));
	while (!IsStackEmpty(stack))
		printf("the stack pops %d out\n", pop(stack));

	printf("the stack is %s empty\n", (IsStackEmpty(stack)) ? "really" : "not");

	system("pause");
	return 0;
}