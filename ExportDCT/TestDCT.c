#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include "jerror.h"
#include "jconfig.h"
#include "ExportDCT.h"

int main( int argc, const char* argv[]){
	int * a;
	a= getDCT("./demo.jpg");
	
	int i;
	printf("%i \n",a[0]);
	printf("%i \n",a[1]);
	printf("%i \n",a[1602]);
	return 0;
}
