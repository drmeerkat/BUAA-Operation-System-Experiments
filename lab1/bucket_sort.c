#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* bucket_sort(int* arr, int range, int len);



int* bucket_sort(int* arr, int range, int len) {
	int *bucket = (int*)malloc(sizeof(int)*range);
	int i;
	for (i = 0; i < range; i++)
		bucket[i] = 0;

	for (i = 0; i < len; i++) {
		bucket[*(arr+i)] += 1;
	}

	return bucket;
}









int maintest() {
	int test[10] = { 1, 4, 5, 6, 0, 9, 8 ,8 };
	int *result, i, j;
	result = bucket_sort(test, 10, 8);
	for (i = 0; i < 10; i++) {
		//printf("result is %d\n", *(result + i));
		for (j = 0; j < *(result + i); j++) {
			printf("%d\n",i);

		}
	}
	





	system("pause");
	return 0;
}