# keras2cpp_multithreading image segmentation
convert and use your keras trained model in c/c++ project in the easist way. Multithreaded implementation for faster execution. Tested on visual studio vc++, but can be used with any other compiler.

Step 1: Convert Your Keras Model into text files
Step 2: Include Keras.h and Keras.cpp files into your project
Step 3: Read or convert your image into keras format
Step 4: call ExecuteKerasSegmentation
Step 5: save result


#For step 1:

1a - Load your keras model 

model=keras.models.load_model('model.h5')

2a - Save your keras model into two text files

save_keras_model_as_text(model, open('modeltext.txt', 'w') )
save_keras_model_as_text(model, open('modellayersonlytext.txt', 'w') , noweight=True)

#Step 2: #include "Keras.h"

#Step 3: Open an image into keras format or convert your image into keras format

int w,h;
int *img = open_image_ppm("img1.ppm", w,h);

#Step 4:

char *modelfile ="modellayersonlytext.txt";  
char *weightfile="modeltext.txt";

int *result = ExecuteKerasSegmentation(img, h, w, 3, modelfile, weightfile);


#Step 5: Save or Use your result.
save_image_pgm("segmentation_map.pgm",result,h,w,127);

