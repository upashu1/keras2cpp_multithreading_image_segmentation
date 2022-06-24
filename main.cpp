#include "Keras.h"


void main()
{
	int w,h;
	int *img = open_image_ppm("img1.ppm", w,h);
	char *modelfile ="modellayersonlytext.txt";  
   	char *weightfile="modeltext.txt";

   	int *result = ExecuteKerasSegmentation(img, h, w, 3, modelfile, weightfile);

	save_image_pgm("segmentation_map.pgm",result,h,w,127);
}