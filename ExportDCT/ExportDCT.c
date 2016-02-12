/* ExportDCT.c
 * This code takes the path/name of a JPEG file as chad input, and returns
 * a 1D array containing the image size(/8) in the first two places, and the
 * values of the block DCT coefficients in the rest of the array, in serial order.
 * It was built as a bridge between jpeglib (the only way I found to access raw DCT
 * JPEG coefficients) and Java.
 * This code should work for the vast majority of JPEG images, but a lot of work 
 * would be needed before it could cover every possible scenario.

 * This software is based in part on the work of the Independent JPEG Group.
 * You can build the IJG's JPEG Tools code library from
 * http://www.ijg.org/files/jpegsrc.v8d.tar.gz, but the version of the JPEG 
 * library (libjpeg) shipped with most Linux versions should work fine. See the
 * README for more details.
 *
 * I have also incorporated code from Phil Sallee's MATLAB JPEG Toolbox, 
 * Copyright (c) 2003 The Regents of the University of California. 
 *
 * You are free to use this code for any purpose. There is no guarrantee that it
 * will work at all. I am not liable for any damage caused by this code.
 *
 * Markos Zampoglou, 2016
 * CERTH-ITI, Thessaloniki, Greece
 */


#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include "jerror.h"
#include "jconfig.h"

int testInOut(int in) {
	//dummy function just to test C functionality in your system
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
        //int components;
        int ci;
	
	input_file = fopen(InputFileName, "rb");
  	cinfo.err = jpeg_std_error(&jerr);
  	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, input_file);
	(void) jpeg_read_header(&cinfo, TRUE);

	//Stolen from Phil Sallee's MATLAB JPEG Toolbox jpeg_read.c
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
	//components=cinfo.num_components;
	jpeg_component_info *compptr;

	//Component id. Laziest extension is to create separate methods for each.
	ci=0;
	compptr = cinfo.comp_info+ci;

		
	int * outputArray;
	// the output array will contain the image/coeff matrix width and height 
	// (measured in blocks of 8x8 pixels) in the first two positions
	outputArray=malloc((size_t) sizeof(int)*((compptr->height_in_blocks)*(compptr->width_in_blocks)*64+2));
	outputArray[0]=(int)compptr->height_in_blocks;
	outputArray[1]=(int)compptr->width_in_blocks;

	int Ind_output,i;
	Ind_output=2;
	
	// the rest of the array will contain coefficient values, in serial order
	JDIMENSION blk_x,blk_y;
	for (blk_y = 0; blk_y < compptr->height_in_blocks; blk_y++) {
      dct_buffer = (cinfo.mem->access_virt_barray)
    	((j_common_ptr) &cinfo, coef_arrays[ci], blk_y, 1, FALSE);
      for (blk_x = 0; blk_x < compptr->width_in_blocks; blk_x++) {
        coeff_ptr = dct_buffer[0][blk_x];
		for (i=0;i<DCTSIZE*DCTSIZE;i++){
			outputArray[Ind_output]=(int) coeff_ptr[i];
			Ind_output++;
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
