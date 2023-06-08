#include <stdio.h>
#include <stdlib.h>

void swap(int* x, int* y) {
	
	int tmp = *x;
	*x = *y;
	*y = tmp;
}

int main(int argc, char** argv) {
	
	int x = 10;
	int y = 5;
	int *px = &x;
	/*
	int tmp = x;
	x = y;
	y = tmp;
	*/
	
	swap(&x, &y);
	
	printf("%d\n", *px);
	printf("%p\n", px);
	
	printf("%d %d\n", x, y);
	
	exit(EXIT_SUCCESS);
}
