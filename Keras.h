
//main function to call
int *ExecuteKerasSegmentation(int *img, int &h, int &w, int channels, char *modelfile, char*weightfile);


bool save_image_pgm(const std::string &name, int* img, int h, int w, int scale=1);
int *open_image_ppm(const std::string &name, int &w, int &h);

//define layer types
enum KerasLayerType {Keras_InputLayer, Keras_Conv2D, Keras_SeparableConv2D, Keras_Activation, Keras_BatchNormalization, Keras_Add, Keras_UpSampling2D, Keras_MaxPooling2D   };
//it means #define Conv2D	0, #define SeparableConv2D		1, #define Activation			2 etc....
enum KerasAcitvationType {Keras_linear, Keras_relu, Keras_sigmoid, Keras_tanh, Keras_softmax   };
//define activation type

//define padding type
#define Keras_same				0
#define Keras_valid				1
	
//define limits variables
#define MAX_CONN			40
#define MAX_THREADS         8
//define struct for layers

struct layer_info {
	char name[80];
	enum KerasLayerType type; //classname
	int no_of_input_layers;
	int input_layer_ids[MAX_CONN];
	int no_of_output_layers;
	int output_layer_ids[MAX_CONN];
	//int no_of_input_channels, no_of_output_channels; //size z
	int input_image_size_x, input_image_size_y, input_image_size_z; //z is depth no of channels in images
	float *input_data[MAX_CONN];
	int no_of_input_data;
	//layer specific params
	enum KerasAcitvationType activation_type;
	int kernel_size_x, kernel_size_y, kernel_size_z, kernel_count;
	int pool_size_x, pool_size_y;
	int padding_type;
	//layer specific weights/biases

	//variable to store result's lifetime and result /output
	int result_lifetime;
	float *result;
	int output_image_size_x, output_image_size_y, output_image_size_z; //z is depth no of channels in images
};

struct memory_chunk{
	float *mem[MAX_CONN];
	int size[MAX_CONN];
	bool isfree[MAX_CONN];
	int ref[MAX_CONN];
	int max_size;

};


struct thread_data{
	float *inputs[MAX_CONN];
	int no_of_inputs;
	int input_size_x, input_size_y, input_size_z;
	int zfrom, zto; //limit of z to be processed by this thread

	int optype; //1 = sepconv 2= pointwise conv2d 3 = add bias 4=activation 5=BN-multiply 6=add_layer etc
	//params weights
	float *kernel;
	int kernel_size_x, kernel_size_y, kernel_size_z, kernel_count;
	enum KerasAcitvationType activation_type;
	int poolx, pooly;
	//temp memory
	float *bias;
	//output
	float *result;
	int output_image_size_x, output_image_size_y, output_image_size_z; //z is depth no of channels in images
};

//auxiliary functions
float *AllocateMemoryChunk(memory_chunk & m,int size=-1);
bool FreeMemoryChunk(memory_chunk & m, float *p);
float *ReferMemoryChunk(memory_chunk & m, float *fp);
bool IsValidChunk(memory_chunk & m, float *p);