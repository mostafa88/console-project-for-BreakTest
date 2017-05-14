#include "bazinama.h"
#include <stdio.h>

bazinama::bazinama()
{
}


bazinama::~bazinama()
{
}

void bazinama::testCrash()
{
	int b = 12;
	int c = 12;
	int a = b - c;
	printf("%d",1/a);
// 	int a = 0;
// 	printf("%d",1/a);
}

