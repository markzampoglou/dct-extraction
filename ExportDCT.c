#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include "jerror.h"
#include "jconfig.h"

int testInOut(int in) {

	printf("Called successfully \n");
	return in*4;
}


int * getDCT(char *InputFileName)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * input_file;
	JBLOCKARRAY dct_buffer;
	JCOEFPTR coeff_ptr;
	jvirt_barray_ptr * coef_arrays;
	int components, ci;
	
	input_file = fopen(InputFileName, "rb");
  	cinfo.err = jpeg_std_error(&jerr);
  	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, input_file);
	(void) jpeg_read_header(&cinfo, TRUE);

	//Stolen from Matlab's jpeg_read
	  switch (cinfo.out_color_space) {
    case JCS_GRAYSCALE:
      cinfo.out_color_components = 1;
      break;
    case JCS_RGB:
      cinfo.out_color_components = 3;
      break;
    case JCS_YCbCr:
      cinfo.out_color_components = 3;
      break;
    case JCS_CMYK:
      cinfo.out_color_components = 4;
      break;
    case JCS_YCCK:
      cinfo.out_color_components = 4;
      break;
    case JCS_UNKNOWN:
      cinfo.out_color_components = 3;
      printf("Unknown color space");
      break;
    }
	
	coef_arrays=jpeg_read_coefficients (&cinfo);
	components=cinfo.num_components;
	jpeg_component_info *compptr;

	//Component id. Laziest extension is to create separate methods for each.
	ci=0;
	compptr = cinfo.comp_info+ci;

		
	int * a;
	a=malloc((size_t) sizeof(int)*((compptr->height_in_blocks)*(compptr->width_in_blocks)*64+2));
	a[0]=(int)compptr->height_in_blocks;
	a[1]=(int)compptr->width_in_blocks;

	int Ind_a,i;
	Ind_a=2;
	
	JDIMENSION blk_x,blk_y;
	for (blk_y = 0; blk_y < compptr->height_in_blocks; blk_y++) {
      dct_buffer = (cinfo.mem->access_virt_barray)
    	((j_common_ptr) &cinfo, coef_arrays[ci], blk_y, 1, FALSE);
      for (blk_x = 0; blk_x < compptr->width_in_blocks; blk_x++) {
        coeff_ptr = dct_buffer[0][blk_x];
		for (i=0;i<DCTSIZE*DCTSIZE;i++){
			a[Ind_a]=(int) coeff_ptr[i];
			Ind_a++;
		}
}
}
/*
	// Alternative code to store data in file
	long j;
	FILE *OutFile;
	char *mode = "w";
	char FileName[]="Resultsx.txt";
	FileName[7]=(char)(((int)'9')+ci);
	OutFile = fopen(FileName, mode);
	
	JDIMENSION blk_x,blk_y;
	for (blk_y = 0; blk_y < compptr->height_in_blocks; blk_y++) {
      dct_buffer = (cinfo.mem->access_virt_barray)
    	((j_common_ptr) &cinfo, coef_arrays[ci], blk_y, 1, FALSE);
      for (blk_x = 0; blk_x < compptr->width_in_blocks; blk_x++) {
        coeff_ptr = dct_buffer[0][blk_x];
		for (i=0;i<DCTSIZE*DCTSIZE;i++){
		fprintf(OutFile,"%i \n", coeff_ptr[i]);
	};
	fclose(OutFile);
	*/
	
	/*
	dct_buffer=(cinfo.mem->access_virt_barray) 
	((j_common_ptr) &cinfo, coef_arrays[ci], (JDIMENSION) 0, (JDIMENSION) compptr->v_samp_factor, FALSE);
	coeff_ptr=dct_buffer[0][0];
	
	long j;
	
	// Alternative code to store data in file
	FILE *OutFile;
	char *mode = "w";
	char FileName[]="Resultsx.txt";
	FileName[7]=(char)(((int)'9')+ci);
	OutFile = fopen(FileName, mode);
	
	for(j=0;j<(compptr->height_in_blocks+1)*(compptr->width_in_blocks+1)*64;j=j+1){ // ;j<1050580;j++){//
		//fprintf(OutFile,"%i \n", coeff_ptr[j]);
	};
	fclose(OutFile);
	//
	

	int * a;
	a=malloc((size_t) sizeof(int)*((compptr->height_in_blocks+1)*(compptr->width_in_blocks+1)*64+2));

	a[0]=(int)compptr->height_in_blocks+1;
	a[1]=(int)compptr->width_in_blocks+1;

	for(j=0;j<(compptr->height_in_blocks+1)*(compptr->width_in_blocks+1)*64;j=j+1){ //1050580;j++){//
		if(j>1){
		printf("Loop %i \n",j);
		}
			a[j+2]=(int)coeff_ptr[j];
	};
	printf("Loop %i \n",j);
	printf("Theoretical limit: %i \n",j);
	fclose(input_file);
*/
	return a;	
}
