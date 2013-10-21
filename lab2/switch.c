#include<stdio.h>
int main(){
	int i;
	scanf("%d",&i);
	switch(i){
	case 103:
		i=i+3;
		break;
	case 102:			
		i=i+12;
		break;
	case 101:
		i=i-1;
	case 104:
		i=4;
		break;
	case 108:
		i=18;
		break;
	case 106:
		i=i*2;
		break;
	default:
		i=1;
	}
	printf("%d",i);
	return 0;

}
