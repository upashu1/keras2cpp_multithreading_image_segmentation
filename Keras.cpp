//#include "stdafx.h"
//#include "demo.h"

#include "Keras.h"
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

/////////////////////////////////////////////////////////////////
///If not visual studio c/c++/vc++ then, uncomment below 2 lines
// #define _T(a) a
// #define TRACE printf





//Memory handling

//define global data 
memory_chunk DataMemChunk;
memory_chunk KernelMemChunk;



//Init functions
void InitMemoryChunk(memory_chunk & m, int max_size=0)
{
	for(int i=0; i <MAX_CONN; i++)
	{
		m.isfree[i]=true;
		m.mem[i]=NULL;
		m.size[i]=0;
		m.ref[i]=0;
	}
	m.max_size = max_size;
}
float *AllocateDataMemory(int size=-1)
{
	return AllocateMemoryChunk(DataMemChunk, size);
}

float *AllocateKernelMemory(int size=-1)
{

	return AllocateMemoryChunk(KernelMemChunk, size);
}

void FreeDataMemory(float *fp)
{
	FreeMemoryChunk(DataMemChunk,fp);
}

void FreeKernelMemory(float *fp)
{
	FreeMemoryChunk(KernelMemChunk,fp);
}

float *ReferDataMemory(float *fp)
{
	return ReferMemoryChunk(DataMemChunk,fp);
}

bool IsValidDataMemory(float *fp)
{
	return IsValidChunk(DataMemChunk,fp);
}
bool IsValidKernelMemory(float *fp)
{
	return IsValidChunk(KernelMemChunk,fp);
}

float *ReferMemoryChunk(memory_chunk & m, float *fp)
{
	for(int i=0; i <MAX_CONN; i++)
	{
		if(fp == m.mem[i] && fp!= NULL)
		{
			m.ref[i]++;
			break;
		}
	}
	return fp;

}


float *AllocateMemoryChunk(memory_chunk & m,int size/*=-1*/)
{
	int found=-1;
	for(int i=0; i <MAX_CONN; i++)
	{
		if(m.isfree[i] == true)
		{
			m.isfree[i]=false;
			m.ref[i]=1;
			found=1;

			if(m.mem[i] != NULL && m.size[i] >= size)
			{
				
				return m.mem[i];
			}
			else if(m.mem[i])
			{
				delete [] (m.mem[i]);
				m.size[i]= max(m.max_size, size);
				m.mem[i] = new float[m.size[i]];
				return m.mem[i];

			}
			else
			{
				m.size[i]= max(m.max_size, size);
				m.mem[i] = new float[m.size[i]];
				return m.mem[i];
			}
		}
	}
	if(size < 2)
		size = max(m.max_size, size);
	if(found == -1)
	{
		return new float[size];
	}

	return NULL;
}

bool FreeMemoryChunk(memory_chunk & m, float *p)
{
	int found=-1;
	for(int i=0; i <MAX_CONN; i++)
	{
		if(m.mem[i] == p && p!= NULL )
		{
			if(m.ref[i]<=1)
			{
				m.isfree[i]=true;
				m.ref[i]=0;
			}
			else
				m.ref[i]--;
			found=1;
		}
	}
	if(found==-1)
		delete [] p;
	return true;
}

bool IsValidChunk(memory_chunk & m, float *p)
{
	int found=-1;
	for(int i=0; i <MAX_CONN; i++)
	{
		if(m.mem[i] == p && p!= NULL )
		{
			if(m.ref[i]<=0)
			{
				return false;
			}
			else
				return true;
			found=1;
		}
	}
	
	return false;
}

//destroy functions
void ExitMemoryChunk(memory_chunk &m)
{
	for(int i=0; i <MAX_CONN; i++)
	{
		if(m.mem[i])
			delete [] m.mem[i];
		m.isfree[i]=true;
		m.mem[i]=NULL;
		m.size[i]=0;
		m.ref[i]=0;
	}
}


bool save_image_pgm(const std::string &name, int* img, int h, int w, int scale/*=1*/)
{
	if(!img)
		return false;
	std::ofstream outfile(name);
   if(!outfile.is_open())
   {
	  // AfxMessageBox(_T("Unable to open pgm image file to write"));
	   return false;
   }
  // std::string code="P3";
  
   outfile<<"P2"<<std::endl;
   outfile<<"# Ashu BCR "<<std::endl;
   // int cc=0; 
   outfile<<w<<"      "<<h<<std::endl;
   outfile<<255<<std::endl;
  
   
   
   
	   int k=0;
	   for (int y = 0; y < h; y++) {
		  for (int x = 0; x < w;x++) {
			 
			 outfile<<(unsigned int)(img[y*w+x]*scale) << "   ";
			
			// TRACE(_T("\n### pgm 8 bit outpv=%d putpv1=%d "), outpv, putpv1);
		  }
		 // std::cout<<"\n";
		  outfile<<std::endl;
	   }
   
  
   
   outfile.close();
   return true;
}

int *open_image_ppm(const std::string &name , int &w, int &h)
{
	
   std::ifstream infile(name);
   if(!infile.is_open())
   {
	  // AfxMessageBox(_T("Unable to open ppm image file to read"));
	   return NULL;
   }
   std::string code, comment;
  
   infile>>code; //read P3
   infile>>comment;
   int maxlevel;
   infile>>w>>h>>maxlevel;
   unsigned int r,g,b;
   
   int *img = new int[h*w*3];
   int *img_r = img, *img_g =&img[h*w], *img_b=&img[2*h*w];
	   int k=0;
	   for (int y = 0; y < h; y++) {
		  for (int x = 0; x < w;x++) {
			 infile>>r>>g>>b;
			 *img_r++ =r; *img_g++ =g; *img_b++ = b;
		  }
		 
	   }
   
  
   
   infile.close();
   return img;
}




std::ifstream & ashu_getline(std::ifstream & wfile, std::string & line)
{
	std::getline(wfile,line);
	if(line.length()<1)
		std::getline(wfile,line);
	if(line.length()<1)
		std::getline(wfile,line);
	if(line.length()<1)
		std::getline(wfile,line);
	if(line.length()<1)
		std::getline(wfile,line);
	//TRACE(_T("@@@@ %s\n"), line.c_str());
	return wfile;
}


int ReadNoOflayers(char *modelfile)
{
	int no_of_layers=0;
	std::ifstream infile( modelfile );
	std::string line;
	if(std::getline(infile, line))
	{
		std::istringstream iss(line);
		char a[80]; int b;
		if (!(iss >> a >> b)) {
			TRACE(_T("Unable to read data from model file\n"));
			return -1; } // error

		no_of_layers =b;
	}
	else
	{
		TRACE(_T("Unable to read model file\n"));
		return -1;
	}
	infile.close();
	return no_of_layers;
}

int ReadLayersName(struct layer_info *layers, int no_of_layers,char *modelfile)
{
	int current_layer_no=0;
	std::ifstream infile( modelfile );
	std::string line;
	int ret=1;
	while (std::getline(infile, line))
	{
		if(line[0]=='#')
		{
			if(current_layer_no >= no_of_layers)
			{
				TRACE(_T(" Error in caluculating no of layers\n"));
				return -1;
			}
			std::istringstream iss(&line[1]);
			//char a[80], b[80], c[80]; will do
			std::string a; int b;
			if(current_layer_no > 0)
			{
				if (!(iss >> a >> layers[current_layer_no].name >> b)) { 
					TRACE(_T("unable to read model file correctly\n"));
					return -1; 
				} // error
			}
			else
			{
				std::string e,f,g,h;
				if (!(iss >> a >> layers[current_layer_no].name >>e>>f>>g>> b)) { 
					TRACE(_T("unable to read model file correctly\n"));
					return -1; 
				} // error
			}
			///assign layer types Keras_Conv2D, Keras_SeparableConv2D, Keras_Activation, Keras_BatchNormalization, Keras_Add, Keras_UpSampling2D, Keras_MaxPooling2D   };
			if(a == "Conv2D")
				layers[current_layer_no].type = Keras_Conv2D;
			else if(a == "SeparableConv2D")
				layers[current_layer_no].type = Keras_SeparableConv2D;
			else if(a == "Activation")
				layers[current_layer_no].type = Keras_Activation;
			else if(a == "BatchNormalization")
				layers[current_layer_no].type = Keras_BatchNormalization;
			else if(a == "Add")
				layers[current_layer_no].type = Keras_Add;
			else if(a == "UpSampling2D")
				layers[current_layer_no].type = Keras_UpSampling2D;
			else if(a == "MaxPooling2D")
				layers[current_layer_no].type = Keras_MaxPooling2D;
			else if(a == "InputLayer")
				layers[current_layer_no].type = Keras_InputLayer;
			else
			{
				//layers[current_layer_no].type = 
				TRACE(_T(" %s Layer not implemented. "), a.c_str());
				return -1;
			}
			layers[current_layer_no].no_of_input_layers =0;
			layers[current_layer_no].no_of_output_layers =0;
			layers[current_layer_no].input_image_size_x =0;
			layers[current_layer_no].input_image_size_y =0;
			layers[current_layer_no].kernel_size_x =0;
			layers[current_layer_no].kernel_size_y =0;
			layers[current_layer_no].kernel_size_z =0;
			layers[current_layer_no].kernel_count =0;
			layers[current_layer_no].input_image_size_z =0;
			layers[current_layer_no].output_image_size_z =b;
			layers[current_layer_no].output_image_size_x =0;
			layers[current_layer_no].output_image_size_y =0;
			layers[current_layer_no].activation_type =Keras_linear;
			layers[current_layer_no].pool_size_x=0;
			layers[current_layer_no].pool_size_y =0;
			layers[current_layer_no].result_lifetime=0;
			layers[current_layer_no].result =NULL;
			for(int j=0; j< MAX_CONN; j++)
				layers[current_layer_no].input_data[j]=NULL;
			layers[current_layer_no].no_of_input_data =0;
			current_layer_no++;
			if(ret < b)
				ret =b;
		}

		// process pair (a,b)
	}
	infile.close();

	return ret;
}

int GetLayerIndexByName(std::string layername, struct layer_info *layers, int no_of_layers)
{
	for(int i=0; i < no_of_layers; i++)
	{
		std::string a= layers[i].name;
		if(a==layername)
			return i;

	}
	return -1;
}

int ReadLayersConnections(struct layer_info *layers, int no_of_layers,char *modelfile)
{
	int current_layer_no=0;
	std::ifstream infile( modelfile );
	std::string line;
	int ret=1;
	while (ashu_getline(infile, line))
	{
		if(line[0]=='#')
		{
			if(current_layer_no >= no_of_layers)
			{
				TRACE(_T(" Error in caluculating no of layers\n"));
				return -1;
			}
			std::istringstream iss(&line[1]);
			//char a[80], b[80], c[80]; will do
			std::string a, b;
			if (!(iss >> a >> b)) { 
				TRACE(_T("unable to read model file correctly in ReadLayerConnection iss>>a>>b\n"));
				return -1; 
			} // error
			if(b== std::string(layers[current_layer_no].name))
			{
				ashu_getline(infile, line); //read config and ignore
				ashu_getline(infile, line); //read input nodes
				//link input nodes
				std::istringstream iss1(&line[1]); //discarded first character >
				std::string s;
				while (std::getline(iss1, s, ' ')) {
					 int idx = GetLayerIndexByName(s, layers, no_of_layers);
					 if(idx > -1 && idx!= current_layer_no)
					 {
						bool found =false;
						//for(int k=0; k< layers[current_layer_no].no_of_input_layers ; k++)
						//	if(layers[current_layer_no].input_layer_ids[k]==idx)
						//		found = true;


						////if(idx == current_layer_no)
						//	found=true;

						if(!found)
							layers[current_layer_no].input_layer_ids[layers[current_layer_no].no_of_input_layers++]=idx;
						found =false;
						//for(int k=0; k< layers[idx].no_of_output_layers ; k++)
						//	if(layers[idx].output_layer_ids[k]==current_layer_no)
						//		found = true;
						if(!found)
							layers[idx].output_layer_ids[layers[idx].no_of_output_layers++]=current_layer_no;
					 }
				}
				//read output nodes
				ashu_getline(infile, line); //read  nodes
				//link output nodes
				std::istringstream iss2(&line[1]); //discarded first character <
				//std::string s;
				s="";
				while (std::getline(iss2, s, ' ')) {
					 int idx = GetLayerIndexByName(s, layers, no_of_layers);
					 if(idx > -1 && idx !=current_layer_no)
					 {
						bool found =false;
						//for(int k=0; k< layers[current_layer_no].no_of_output_layers ; k++)
						//	if(layers[current_layer_no].output_layer_ids[k]==idx)
						//		found = true;
						if(!found)
							layers[current_layer_no].output_layer_ids[layers[current_layer_no].no_of_output_layers++]=idx;
						found =false;
						//for(int k=0; k< layers[idx].no_of_input_layers ; k++)
						//	if(layers[idx].input_layer_ids[k]==current_layer_no)
						//		found = true;
						if(!found)
							layers[idx].input_layer_ids[layers[idx].no_of_input_layers++]=current_layer_no;
					 }
				}

			}
			else
			{
				TRACE(_T(" Error in Reading Model file. layer name is not matching. %s    %s "), b, (layers[current_layer_no].name) );
			}
			current_layer_no++;
		}

		// process pair (a,b)
	}
	infile.close();

	return 1;
}

int GetMaxMemorySize(struct layer_info *layers, int no_of_layers,int h, int w, int channels=3) //h, w input rgb image size
{
	int max_s =0, tmp=0;
	tmp = w*h*channels;
	if(max_s < tmp)
			max_s = tmp;
	for(int i=0; i < no_of_layers; i++)
	{
		layers[i].input_image_size_x =w;
		layers[i].input_image_size_y =h;
		layers[i].input_image_size_z = channels;

		

		if(layers[i].type == Keras_MaxPooling2D) //assuminmg pooling by 2
		{
			w /=2; h/=2;
		}
		if(layers[i].type == Keras_UpSampling2D) //assuminmg pooling by 2
		{
			w *=2; h *=2;
		}
		channels = layers[i].output_image_size_z;
		tmp = w*h*channels;
		if(max_s < tmp)
			max_s = tmp;
	}

	return max_s;

}

void /*unsigned long _stdcall*/ DepthwiseSepConv2D(void *lpParam ) //padding same
{
	struct thread_data *th=(struct thread_data *) lpParam;
	//for this operation no of inputs =1, kernel count=1
	unsigned int st_x = (th->kernel_size_x  - 1) >> 1;
	unsigned int st_y = (th->kernel_size_y  - 1) >> 1;
	float *img = th->inputs[0]; //single input
	for(int z=th->zfrom; z< th->zto; z++)  //in each channel perform conv2d
	{
		int kc_ind = z*th->kernel_size_x*th->kernel_size_y;
		int ic_ind = z*th->input_size_x *th->input_size_y;
		for(int y=0; y <th->input_size_y; y++)    /////swapping input sizes for testing
			for(int x=0; x < th->input_size_x; x++)
			{
				float sum=0.0f;
				for(int ky=0; ky < th->kernel_size_y; ky++)
				{
					//int ky_ind = kc_ind + (th->kernel_size_y-1-ky)* th->kernel_size_x; //flipy
					int ky_ind = kc_ind + (ky)* th->kernel_size_x; //don't flipy
					for(int kx=0; kx < th->kernel_size_x; kx++)
					{
						  //flipped kernel item index
						  //int kindex = ky_ind + (th->kernel_size_x-1 -kx); //flipx
						   int kindex = ky_ind + (kx); //don't flipx
						  if(x-int(st_x)+kx < 0) continue;
						  if(x-int(st_x)+kx > (th->input_size_x-1) ) continue;
						  if(y-int(st_y)+ky < 0) continue;
						  if(y-int(st_y)+ky > (th->input_size_y-1)) continue;
						  int iindex = ic_ind + (y-st_y+ky)*th->input_size_x + x-st_x+kx ;
						  sum += th->kernel[kindex] * img[iindex];
					}
				}
				int oindex = ic_ind + y*th->output_image_size_x +x; //No transpose here
				th->result[oindex]=sum;

			}
	}
	_endthread();
	return;
}
void /*unsigned long _stdcall*/ Conv2D(void *lpParam ) //padding same
{
	//std::ofstream logf("C:\\bcrpath\\conv2dlog_myprog.txt");
	struct thread_data *th=(struct thread_data *) lpParam;
	//for this operation no of inputs =1, kernel count=1
	unsigned int st_x = (th->kernel_size_x  - 1) >> 1;
	unsigned int st_y = (th->kernel_size_y  - 1) >> 1;
	float *img = th->inputs[0]; //single input
	//set zero output
	//memset(th->result,0,th->output_image_size_x * th->output_image_size_y* th->output_image_size_z * sizeof(float));
	for(int c=th->zfrom; c< th->zto; c++)
	{
		int kc_ind0 = c*th->kernel_size_x*th->kernel_size_y*th->kernel_size_z;
		int oindex = c*th->output_image_size_x*th->output_image_size_y;
		//memset(&(th->result[oindex]),0,th->output_image_size_x * th->output_image_size_y* sizeof(float));
		for(int z=0; z< th->input_size_z; z++)  //in each channel perform conv2d
		{
			int kc_ind = kc_ind0 + z*th->kernel_size_x*th->kernel_size_y;
			int ic_ind = z*th->input_size_x *th->input_size_y;
			for(int y=0; y <th->input_size_y; y++)    /////swapping input sizes for testing
				for(int x=0; x < th->input_size_x; x++)
				{
					float sum=0.0f;
					for(int ky=0; ky < th->kernel_size_y; ky++)
					{
						//int ky_ind = kc_ind + (th->kernel_size_y-1-ky)* th->kernel_size_x; //flipy
						int ky_ind = kc_ind + (ky)* th->kernel_size_x; //don't flipy
						for(int kx=0; kx < th->kernel_size_x; kx++)
						{
							  //flipped kernel item index
							  //int kindex = ky_ind + (th->kernel_size_x-1 -kx); //flipx
							  int kindex = ky_ind + (kx); //don't flipx
							  if(x-st_x+kx < 0) continue;
							  if(x-st_x+kx > (th->input_size_x-1) ) continue;
							  if(y-st_y+ky < 0) continue;
							  if(y-st_y+ky > (th->input_size_y-1)) continue;
							  int iindex = ic_ind + (y-st_y+ky)*th->input_size_x + x-st_x+kx ;
							  //TRACE(_T("$ Conv2D thread mult = kernel %f  * image %f -> c %d z %d y %d x %d \n"),th->kernel[kindex], img[iindex], c ,z, y, x);
							  //if(img[iindex]>0)
								//logf<<th->kernel[kindex]<<" "<<img[iindex]<<" "<<c<<" "<<z<<" "<<x<<" "<<y<<"\n";
							  sum += th->kernel[kindex] * img[iindex];
						}
					}
					//int oindex1 = oindex + y*th->output_image_size_x +x;
					int oindex1 = oindex + y*th->output_image_size_x +x; //don't transpose matrix here
					th->result[oindex1] +=sum;

				}
		}

		//add biases
		for(int y=0; y <th->output_image_size_y ; y++)
				for(int x=0; x < th->output_image_size_x; x++)
				{
					int oindex1 = oindex + y*th->output_image_size_x +x;
					th->result[oindex1] +=th->bias[c];
					//also do associated activation here
					//copy from Activation function
					switch(th->activation_type)
					{
					case Keras_relu:
						if(th->result[oindex1] < 0)
							th->result[oindex1]=0.0;
						else
							th->result[oindex1]=th->result[oindex1];
						break;
					case Keras_sigmoid:
						th->result[oindex1]= 1/(1+ exp(-th->result[oindex1]));
						break;
					case Keras_tanh:
						th->result[oindex1] = tanh(th->result[oindex1]);
						break;
					case Keras_softmax:
						th->result[oindex1] = exp(th->result[oindex1]); //step 1 is done here, step 2 is non multithreaded and implemented in caller
						break;
					default:
						//th->result[oindex] = (img[oindex]); // linear 
						break;

					}

				}
	}
	_endthread();
	return;
}

int ReadExecuteSeparableConv2DLayer(struct layer_info *layers, int current_layer_no, std::ifstream & wfile)
{
	//TRACE(_T("Reading SeparableCon2D layer %d\n"),current_layer_no);
	std::string line;
	ashu_getline(wfile, line); //read layer name type
	if(line.length() < 1)  //skip empty line or left newline character only
		ashu_getline(wfile, line); //read layer name type
	if(line[0] != '#')
	{
		TRACE(_T(" expecting to read line startwith # but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read config
	if(line[0] != '%')
	{
		TRACE(_T(" expecting to read line startwith % but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read input
	if(line[0] != '>')
	{
		TRACE(_T(" expecting to read line startwith > but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read output
	if(line[0] != '<')
	{
		TRACE(_T(" expecting to read line startwith < but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read no of input channel, no of output channel, no of weights, activation, name
	std::istringstream iss(line);
	//char a[80], b[80], c[80]; will do
	std::string act, name; int a,b,c;
	if (!(iss >> a >> b >>c >> act >> name)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteSeparableConv2DLayer iss>>a>>b\n"));
				return -1; 
			} // error
	if(name != layers[current_layer_no].name)
	{
		TRACE(_T("\n Error in matching layers name : %s   vs %s \n"), name, layers[current_layer_no].name );
		return -1;
	}
	if(act=="linear")
		layers[current_layer_no].activation_type = Keras_linear;
	else if(act=="relu")
		layers[current_layer_no].activation_type = Keras_relu;
	else if(act=="sigmoid")
		layers[current_layer_no].activation_type = Keras_sigmoid;
	else if(act=="tanh")
		layers[current_layer_no].activation_type = Keras_tanh;
	else if(act=="softmax")
		layers[current_layer_no].activation_type = Keras_softmax;
	else
		layers[current_layer_no].activation_type = Keras_linear; //default behaviour

	int kc, kz, kx, ky; //kernel count, kernel depth, size-x, size-y
	std::string padding; //presently only "same" padding
	ashu_getline(wfile, line);
	std::istringstream iss1(line);
	if (!(iss1 >> kc >> kz >>kx >> ky >> padding)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteSeparableConv2DLayer iss>>a>>b\n"));
				return -1; 
			} // error
	//for separable conv, kc==1
	if(kc !=1 )
	{
		TRACE(_T(" kc !=1 , kc =%d \n"), kc);
		return -1;
	}
	if(kz != layers[current_layer_no].input_image_size_z )
	{
		TRACE(_T(" kernel size z and input size z not matching in ReadExecuteSeparableConv2DLayer \n"));
	}
	char ch; int k_index=0; float val=0.0f;
	float *kernel1 = AllocateKernelMemory(kc*kz*kx*ky);
	for(int ikc=0; ikc<kc; ikc++)
		for(int ikz=0; ikz < kz; ikz++)
			for(int iky=0; iky < ky; iky++)
			{
				//ashu_getline(wfile, line);
				//std::istringstream iss(line);
				//iss >> a >> b
				wfile>>ch; //read [
				if(ch != '[')
				{
					TRACE(_T("expecting line start with [ but found %c\n" ), ch);
					return -1;
				}
				for(int ikx=0; ikx < kx; ikx++)
				{
					wfile>> val;
					kernel1[k_index++]=val;
				}
				wfile>>ch; //read ]
				if(ch != ']')
				{
					TRACE(_T("expecting line end with ] but found %c\n" ), ch);
					return -1;
				}
			}
	//kernel has been read
	//Now apply depthwise separable convolution in threads
	HANDLE h[30];
	int no_of_threads = min(kz,MAX_THREADS);
	struct thread_data th;
	th.activation_type = layers[current_layer_no].activation_type;
	th.no_of_inputs =1; //for sepconv only one input
	if(layers[current_layer_no].no_of_input_data != 1)
	{
		TRACE(_T("no of input data for this layer sepconv2d - should be one instead of %d. no of input layers =%d\n"), layers[current_layer_no].no_of_input_data, layers[current_layer_no].no_of_input_layers );
	}
	th.inputs[0]= layers[current_layer_no].input_data[0];
	th.input_size_x =layers[current_layer_no].input_image_size_x;
	th.input_size_y= layers[current_layer_no].input_image_size_y;
	th.input_size_z = layers[current_layer_no].input_image_size_z;
	th.kernel = kernel1;
	th.kernel_size_x = kx;
	th.kernel_size_y  = ky;
	th.kernel_size_z = kz;
	th.kernel_count =kc;
	th.output_image_size_x = th.input_size_x; //padding same
	th.output_image_size_y = th.input_size_y;
	th.output_image_size_z = th.input_size_z; 
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x);
	int zstep = kz /no_of_threads;
	int startz=0;
	struct thread_data th1[30];
	memset((th.result),0,th.output_image_size_x * th.output_image_size_y*th.output_image_size_z* sizeof(float));
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >kz)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > kz)
			th1[t1].zto = kz;
		if(t1 == no_of_threads-1)
			th1[t1].zto = kz;
		h[t1]= (HANDLE)_beginthread(DepthwiseSepConv2D,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish DepthwiseSepConv2D threads\n"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T(" sepconv2d thread %d finished\n"), t1);
	}
	// 1st step of separable conv 2d completed.
	//remove unneeded memory
	FreeKernelMemory(kernel1);
	FreeDataMemory(layers[current_layer_no].input_data[0]);




	//now 2nd step
	//pointwise conv2d
	//read kernel from file
	wfile>>kc>>kz>>ky>>kx>>padding;
	if( kx !=1 || ky !=1)
	{
		//error this should be pointwise kernel
		TRACE(_T(" Error in reading pointwise kernel kx and ky should be 1, not %d %d"), kx, ky);
	}
	float *kernel2 = AllocateKernelMemory(kc*kz*kx*ky);
	val =0; k_index =0;
	for(int ikc=0; ikc<kc; ikc++)
		for(int ikz=0; ikz < kz; ikz++)
			for(int iky=0; iky < ky; iky++)
			{
				//ashu_getline(wfile, line);
				//std::istringstream iss(line);
				//iss >> a >> b
				wfile>>ch; //read [
				if(ch != '[')
				{
					TRACE(_T("expecting line start with [ but found %c \n" ), ch);
					return -1;
				}
				for(int ikx=0; ikx < kx; ikx++)
				{
					wfile>> val;
					kernel2[k_index++]=val;
				}
				wfile>>ch; //read ]
				if(ch != ']')
				{
					TRACE(_T("expecting line end with ] but found %c\n" ), ch);
					return -1;
				}
			}

	//read bias too
	int bias_len=0;
	wfile>>bias_len;
	if(bias_len != kc)
	{
		TRACE(_T("length of bias vector should be %d, but found %d\n"), kc,bias_len);
	}
	val =0; k_index =0;
	float *bias = AllocateKernelMemory(bias_len);
	wfile>>ch; //read [
	if(ch != '[')
	{
		TRACE(_T(" expecting to read line startwith [ but found %c\n"), ch);
		return -1;
	}
	for(int ib=0; ib < bias_len; ib++)
		wfile>>bias[ib];
	wfile>>ch; //read ]
	if(ch != ']')
	{
		TRACE(_T(" expecting to read line end with ] but found %c\n"), ch);
		return -1;
	}
	//now prepare for multithreading
	///now previous step result is input to this step
	no_of_threads = min(kc,MAX_THREADS);  //kc will be used to create no of threads
	th.inputs[0] = th.result ;
	th.kernel = kernel2;
	th.kernel_size_x = kx;
	th.kernel_size_y  = ky;
	th.kernel_size_z = kz;
	th.kernel_count =kc;
	th.output_image_size_x = th.input_size_x; //padding same
	th.output_image_size_y = th.input_size_y;
	th.output_image_size_z = kc; 
	th.bias = bias;
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x);
	//memset(th->result,0,th->output_image_size_x * th->output_image_size_y* th->output_image_size_z * sizeof(float));
	zstep = kc /no_of_threads; //kc will be used to separate data
	startz=0;
	memset((th.result),0,th.output_image_size_x * th.output_image_size_y*th.output_image_size_z* sizeof(float));
	//struct thread_data th1[30];
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >kc)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > kc)
			th1[t1].zto = kc;
		if(t1 == no_of_threads-1)
			th1[t1].zto = kc;
		h[t1]= (HANDLE)_beginthread(Conv2D,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish pointwise Conv2D threads\n"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T(" pointwise conv2d thread %d finished\n"), t1);
	}

	//activation 
	if(layers[current_layer_no].activation_type == Keras_softmax)
	{
		//non multithreaded sum and divide
		float epsilon =0.001;
		for(int y=0; y < th.output_image_size_y; y++)
			for(int x=0; x < th.output_image_size_x; x++)
			{
				float sum=0.0;
				for(int z=0; z < th.output_image_size_z ; z++)
				{
					int ind = z* th.output_image_size_y*th.output_image_size_x + y*th.output_image_size_x + x;
					sum += th.result[ind];
				}
				for(int z=0; z < th.output_image_size_z ; z++)
				{
					int ind = z* th.output_image_size_y*th.output_image_size_x + y*th.output_image_size_x + x;
					th.result[ind] /= (sum + epsilon);
				}
			}
	}
	layers[current_layer_no].result = th.result;
	layers[current_layer_no].output_image_size_x =th.output_image_size_x;
	layers[current_layer_no].output_image_size_y=th.output_image_size_y;
	layers[current_layer_no].output_image_size_z=th.output_image_size_z;





	//free and assign memory
	FreeDataMemory(th.inputs[0]);
	FreeKernelMemory(kernel2);
	FreeKernelMemory(bias);

	
	for(int i=0; i < layers[current_layer_no].no_of_output_layers; i++)
	{
		int id = layers[current_layer_no].output_layer_ids[i];
		layers[id].input_data[layers[id].no_of_input_data++]= ReferDataMemory(th.result);
		if(layers[id].input_image_size_z != th.output_image_size_z )
		{
			TRACE(_T("Error - output of %s layer having z =%d while input expected by layer %d %s should have z=%d\n"),layers[current_layer_no].name, th.output_image_size_z, id,layers[id].name, layers[id].input_image_size_z );
			
		}
		layers[id].input_image_size_z = th.output_image_size_z;
		layers[id].input_image_size_y = th.output_image_size_y;
		layers[id].input_image_size_x = th.output_image_size_x;
	}
	if(layers[current_layer_no].no_of_output_layers ==0)
	{
		TRACE(_T("This is last layer. Preseving output reference. \n"));
		//calling freedatamemory will delete output.
	}
	else
		FreeDataMemory(layers[current_layer_no].result);
	return 1;
}





int ReadExecuteConv2DLayer(struct layer_info *layers, int current_layer_no, std::ifstream & wfile)
{
	//TRACE(_T("Reading Conv2D layer %d\n"),current_layer_no);
	std::string line;
	ashu_getline(wfile, line); //read layer name type
	if(line.length() < 1)  //skip empty line or left newline character only
		ashu_getline(wfile, line); //read layer name type
	if(line[0] != '#')
	{
		TRACE(_T(" expecting to read line startwith # but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read config
	if(line[0] != '%')
	{
		TRACE(_T(" expecting to read line startwith % but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read input
	if(line[0] != '>')
	{
		TRACE(_T(" expecting to read line startwith > but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read output
	if(line[0] != '<')
	{
		TRACE(_T(" expecting to read line startwith < but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read no of input channel, no of output channel, no of weights, activation, name
	std::istringstream iss(line);
	//char a[80], b[80], c[80]; will do
	std::string act, name; int a,b,c1;
	if (!(iss >> a >> b >>c1 >> act >> name)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteConv2DLayer iss>>a>>b\n"));
				return -1; 
			} // error
	if(name != layers[current_layer_no].name)
	{
		TRACE(_T("\n Error in matching layers name : %s   vs %s \n"), name, layers[current_layer_no].name );
		return -1;
	}
	if(act=="linear")
		layers[current_layer_no].activation_type = Keras_linear;
	else if(act=="relu")
		layers[current_layer_no].activation_type = Keras_relu;
	else if(act=="sigmoid")
		layers[current_layer_no].activation_type = Keras_sigmoid;
	else if(act=="tanh")
		layers[current_layer_no].activation_type = Keras_tanh;
	else if(act=="softmax")
		layers[current_layer_no].activation_type = Keras_softmax;
	else
		layers[current_layer_no].activation_type = Keras_linear; //default behaviour

	int kc, kz, kx, ky; //kernel count, kernel depth, size-x, size-y
	std::string padding; //presently only "same" padding
	ashu_getline(wfile, line);
	std::istringstream iss1(line);
	if (!(iss1 >> kc >> kz >>kx >> ky >> padding)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteConv2DLayer iss>>a>>b\n"));
				return -1; 
			} // error
	
	if(kz != layers[current_layer_no].input_image_size_z )
	{
		TRACE(_T(" kernel size z and input size z not matching in ReadExecuteConv2DLayer \n"));
	}
	char c; int k_index=0; float val=0.0f;
	float *kernel1 = AllocateKernelMemory(kc*kz*kx*ky);
	for(int ikc=0; ikc<kc; ikc++)
		for(int ikz=0; ikz < kz; ikz++)
			for(int iky=0; iky < ky; iky++)
			{
				//ashu_getline(wfile, line);
				//std::istringstream iss(line);
				//iss >> a >> b
				wfile>>c; //read [
				if(c != '[')
				{
					TRACE(_T(" expecting to read line startwith [ but found %c\n"), c);
					return -1;
				}
				for(int ikx=0; ikx < kx; ikx++)
				{
					wfile>> val;
					kernel1[k_index++]=val;
				}
				wfile>>c; //read ]
				if(c != ']')
				{
					TRACE(_T(" expecting to read line end with ] but found %c\n"), c);
					return -1;
				}
			}
	//kernel has been read
	

	//read bias too
	int bias_len=0;
	wfile>>bias_len;
	if(bias_len != kc)
	{
		TRACE(_T("length of bias vector should be %d, but found %d\n"), kc,bias_len);
	}
	val =0; k_index =0;
	float *bias = AllocateKernelMemory(bias_len);
	wfile>>c; //read [
	if(c != '[')
				{
					TRACE(_T(" expecting to read line startwith [ but found %c\n"), c);
					return -1;
				}
	for(int ib=0; ib < bias_len; ib++)
		wfile>>bias[ib];
	wfile>>c; //read ]
	if(c != ']')
				{
					TRACE(_T(" expecting to read line end with ] but found %c\n"), c);
					return -1;
				}
	//now prepare for multithreading
	///now previous step result is input to this step
	
	HANDLE h[30];
	int no_of_threads = min(kc,MAX_THREADS);
	struct thread_data th;
	th.activation_type = layers[current_layer_no].activation_type;
	th.no_of_inputs =1; //for sepconv only one input
	if(layers[current_layer_no].no_of_input_data != 1)
	{
		TRACE(_T("no of input data for this layer conv2d - should be one"));
	}
	th.inputs[0]= layers[current_layer_no].input_data[0];
	th.input_size_x =layers[current_layer_no].input_image_size_x;
	th.input_size_y= layers[current_layer_no].input_image_size_y;
	th.input_size_z = layers[current_layer_no].input_image_size_z;
	
	
	int zstep = kc /no_of_threads;
	int startz=0;
	struct thread_data th1[30];
	
	
	th.kernel = kernel1;
	th.kernel_size_x = kx;
	th.kernel_size_y  = ky;
	th.kernel_size_z = kz;
	th.kernel_count =kc;
	th.output_image_size_x = th.input_size_x; //padding same
	th.output_image_size_y = th.input_size_y;
	th.output_image_size_z = kc; 
	th.bias = bias;
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x);
	//memset(th->result,0,th->output_image_size_x * th->output_image_size_y* th->output_image_size_z * sizeof(float));
	zstep = kc /no_of_threads; //kc will be used to separate data
	startz=0;
	//struct thread_data th1[30];
	memset((th.result),0,th.output_image_size_x * th.output_image_size_y*th.output_image_size_z* sizeof(float));
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >kc)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > kc)
			th1[t1].zto = kc;
		if(t1 == no_of_threads-1)
			th1[t1].zto = kc;
		h[t1]= (HANDLE)_beginthread(Conv2D,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish  Conv2D threads"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T("  conv2d thread %d finished\n"), t1);
	}

	//activation 
	if(layers[current_layer_no].activation_type == Keras_softmax)
	{
		//non multithreaded sum and divide
		float epsilon =0.001;
		for(int y=0; y < th.output_image_size_y; y++)
			for(int x=0; x < th.output_image_size_x; x++)
			{
				float sum=0.0;
				for(int z=0; z < th.output_image_size_z ; z++)
				{
					int ind = z* th.output_image_size_y*th.output_image_size_x + y*th.output_image_size_x + x;
					sum += th.result[ind];
				}
				for(int z=0; z < th.output_image_size_z ; z++)
				{
					int ind = z* th.output_image_size_y*th.output_image_size_x + y*th.output_image_size_x + x;
					th.result[ind] /= (sum + epsilon);
				}
			}
	}
	layers[current_layer_no].result = th.result;
	layers[current_layer_no].output_image_size_x =th.output_image_size_x;
	layers[current_layer_no].output_image_size_y=th.output_image_size_y;
	layers[current_layer_no].output_image_size_z=th.output_image_size_z;





	//free and assign memory
	FreeDataMemory(th.inputs[0]);
	//remove unneeded memory
	FreeKernelMemory(kernel1);
	//FreeDataMemory(layers[current_layer_no].input_data[0]);

	//FreeKernelMemory(kernel2);
	FreeKernelMemory(bias);

	
	for(int i=0; i < layers[current_layer_no].no_of_output_layers; i++)
	{
		int id = layers[current_layer_no].output_layer_ids[i];
		layers[id].input_data[layers[id].no_of_input_data++]= ReferDataMemory(th.result);
		if(layers[id].input_image_size_z != th.output_image_size_z )
		{
			TRACE(_T("Error - output of %s layer having z =%d while input expected by layer %d %s should have z=%d\n"),layers[current_layer_no].name, th.output_image_size_z, id,layers[id].name, layers[id].input_image_size_z );
			
		}
		layers[id].input_image_size_z = th.output_image_size_z;
		layers[id].input_image_size_y = th.output_image_size_y;
		layers[id].input_image_size_x = th.output_image_size_x;
	}
	if(layers[current_layer_no].no_of_output_layers ==0)
	{
		TRACE(_T("This is last layer. Preseving output reference. \n"));
		//calling freedatamemory will delete output.
	}
	else
		FreeDataMemory(layers[current_layer_no].result);
	return 1;
}









void /*unsigned long _stdcall*/ Activation(void *lpParam ) //padding same
{
	struct thread_data *th=(struct thread_data *) lpParam;
	
	float *img = th->inputs[0]; //single input
	for(int z=th->zfrom; z< th->zto; z++)  //in each channel perform conv2d
	{
		
		int ic_ind = z*th->input_size_x *th->input_size_y;
		for(int y=0; y <th->input_size_y; y++)
			for(int x=0; x < th->input_size_x; x++)
			{
				
				int oindex = ic_ind + y*th->input_size_x +x; //input index and output index same
				//if possible in place without creating extra memory
				//No, for generality, inputs are not modified directly, so using another memory
				//th->result[oindex]=;
				switch(th->activation_type)
				{
				case Keras_relu:
					if(img[oindex] < 0)
						th->result[oindex]=0.0;
					else
						th->result[oindex]=img[oindex];
					break;
				case Keras_sigmoid:
					th->result[oindex]= 1/(1+ exp(-img[oindex]));
					break;
				case Keras_tanh:
					th->result[oindex] = tanh(img[oindex]);
					break;
				case Keras_softmax:
					th->result[oindex] = exp(img[oindex]); //step 1 is done here, step 2 is non multithreaded and implemented in caller
					break;
				default:
					th->result[oindex] = (img[oindex]); //just copy linear 
					break;

				}

			}
	}
	

	_endthread();
	return;
}


int ReadExecuteActivationLayer(struct layer_info *layers, int current_layer_no, std::ifstream & wfile)
{
	//TRACE(_T("Reading Activation layer %d\n"),current_layer_no);
	std::string line;
	ashu_getline(wfile, line);
	if(line.length() < 1)  //skip empty line or left newline character only
		ashu_getline(wfile, line); //read layer name type
	if(line[0] != '#')
	{
		TRACE(_T(" expecting to read line startwith # but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read config
	if(line[0] != '%')
	{
		TRACE(_T(" expecting to read line startwith % but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read input
	if(line[0] != '>')
	{
		TRACE(_T(" expecting to read line startwith > but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read output
	if(line[0] != '<')
	{
		TRACE(_T(" expecting to read line startwith < but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read no of input channel,output channel, no of weigths, name
	std::istringstream iss(line);
	std::string act, name; int a,b,c;
	if (!(iss >> a >> b >>c >> name)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteActivationLayer iss>>a>>b  %s\n"), line.c_str());
				return -1; 
			} // error
	if(name != layers[current_layer_no].name)
	{
		TRACE(_T("\n Error in matching layers name : %s   vs %s \n"), name, layers[current_layer_no].name );
		return -1;
	}
	//read activation
	wfile>>act;

	if(act=="linear")
		layers[current_layer_no].activation_type = Keras_linear;
	else if(act=="relu")
		layers[current_layer_no].activation_type = Keras_relu;
	else if(act=="sigmoid")
		layers[current_layer_no].activation_type = Keras_sigmoid;
	else if(act=="tanh")
		layers[current_layer_no].activation_type = Keras_tanh;
	else if(act=="softmax")
		layers[current_layer_no].activation_type = Keras_softmax;
	else
		layers[current_layer_no].activation_type = Keras_linear; //default behaviour

	HANDLE h[30];
	int no_of_threads = min(b,MAX_THREADS);
	struct thread_data th;
	th.activation_type = layers[current_layer_no].activation_type;
	if(layers[current_layer_no].no_of_input_data != 1)
	{
		TRACE(_T("no of input data for this layer activation - should be one"));
	}
	th.no_of_inputs =1; //for sepconv only one input
	th.inputs[0]= layers[current_layer_no].input_data[0];
	th.input_size_x =layers[current_layer_no].input_image_size_x;
	th.input_size_y= layers[current_layer_no].input_image_size_y;
	th.input_size_z = layers[current_layer_no].input_image_size_z;
	
	th.output_image_size_x = th.input_size_x; //padding same
	th.output_image_size_y = th.input_size_y;
	th.output_image_size_z = th.input_size_z; 
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x); //avoid if possible, but not for generality
	int zstep = b /no_of_threads;
	int startz=0;
	struct thread_data th1[30];
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >b)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > b)
			th1[t1].zto = b;
		if(t1 == no_of_threads-1)
			th1[t1].zto = b;
		h[t1]= (HANDLE)_beginthread(Activation,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish activation threads"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T(" activation thread %d finished\n"), t1);
	}

	if(layers[current_layer_no].activation_type == Keras_softmax)
	{
		//non multithreaded sum and divide
		float epsilon =0.001;
		for(int y=0; y < th.output_image_size_y; y++)
			for(int x=0; x < th.output_image_size_x; x++)
			{
				float sum=0.0;
				for(int z=0; z < th.output_image_size_z ; z++)
				{
					int ind = z* th.output_image_size_y*th.output_image_size_x + y*th.output_image_size_x + x;
					sum += th.result[ind];
				}
				for(int z=0; z < th.output_image_size_z ; z++)
				{
					int ind = z* th.output_image_size_y*th.output_image_size_x + y*th.output_image_size_x + x;
					th.result[ind] /= (sum + epsilon);
				}
			}
	}
	layers[current_layer_no].result = th.result;
	layers[current_layer_no].output_image_size_x =th.output_image_size_x;
	layers[current_layer_no].output_image_size_y=th.output_image_size_y;
	layers[current_layer_no].output_image_size_z=th.output_image_size_z;


	//free and assign memory
	FreeDataMemory(th.inputs[0]); //FreeDataMemory(layers[current_layer_no].input_data[0]);
	

	
	for(int i=0; i < layers[current_layer_no].no_of_output_layers; i++)
	{
		int id = layers[current_layer_no].output_layer_ids[i];
		layers[id].input_data[layers[id].no_of_input_data++]= ReferDataMemory(th.result);
		if(layers[id].input_image_size_z != th.output_image_size_z )
		{
			TRACE(_T("Error - output of %s layer having z =%d while input expected by layer %d %s should have z=%d\n"),layers[current_layer_no].name, th.output_image_size_z, id,layers[id].name, layers[id].input_image_size_z );
			
		}
		layers[id].input_image_size_z = th.output_image_size_z;
		layers[id].input_image_size_y = th.output_image_size_y;
		layers[id].input_image_size_x = th.output_image_size_x;
	}
	if(layers[current_layer_no].no_of_output_layers ==0)
	{
		TRACE(_T("This is last layer. Preseving output reference. \n"));
		//calling freedatamemory will delete output.
	}
	else
		FreeDataMemory(layers[current_layer_no].result);
	return 1;


}


void /*unsigned long _stdcall*/ BatchNormalization(void *lpParam ) //padding same
{
	struct thread_data *th=(struct thread_data *) lpParam;
	
	float *img = th->inputs[0]; //single input
	for(int z=th->zfrom; z< th->zto; z++)  //in each channel perform conv2d
	{
		
		int ic_ind = z*th->input_size_x *th->input_size_y;
		for(int y=0; y <th->input_size_y; y++)
			for(int x=0; x < th->input_size_x; x++)
			{
				
				int oindex = ic_ind + y*th->input_size_x +x; //input index and output index same
				//if possible in place without creating extra memory
				//No, for generality, inputs are not modified directly, so using another memory
				//th->result[oindex]=;
				th->result[oindex]= img[oindex]*th->kernel[z] + th->bias[z];

			}
	}
	

	_endthread();
	return;
}



int ReadExecuteBatchNormalizationLayer(struct layer_info *layers, int current_layer_no, std::ifstream & wfile)
{
	//TRACE(_T("Reading BN layer %d \n"),current_layer_no);
	std::string line;
	ashu_getline(wfile, line); //read layer name type
	if(line.length() < 1)  //skip empty line or left newline character only
		ashu_getline(wfile, line); //read layer name type
	if(line[0] != '#')
	{
		TRACE(_T(" expecting to read line startwith # but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read config
	if(line[0] != '%')
	{
		TRACE(_T(" expecting to read line startwith % but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read input
	if(line[0] != '>')
	{
		TRACE(_T(" expecting to read line startwith > but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read output
	if(line[0] != '<')
	{
		TRACE(_T(" expecting to read line startwith < but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read no of input channel, no of output channel, no of weights, activation, name
	std::istringstream iss(line);
	//char a[80], b[80], c[80]; will do
	std::string  name; int a,b,c1;
	if (!(iss >> a >> b >>c1 >> name)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteBatchNormalizationLayer iss>>a>>b %s\n"),line.c_str());
				return -1; 
			} // error
	if(name != layers[current_layer_no].name)
	{
		TRACE(_T("\n Error in matching layers name : %s   vs %s ---- ReadExecuteBatchNormalizationLayer\n"), name, layers[current_layer_no].name );
		return -1;
	}
	

	int  kz; //kernel count, kernel depth, size-x, size-y
	//std::string padding; //presently only "same" padding
	ashu_getline(wfile, line);
	std::istringstream iss1(line);
	if (!(iss1 >>  kz )) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteBatchNormalizationLayer iss>>a>>b\n"));
				return -1; 
			} // error
	
	if(kz != layers[current_layer_no].input_image_size_z )
	{
		TRACE(_T(" kernel size z and input size z not matching in ReadExecuteBatchNormalizationLayer \n"));
	}
	char c; int k_index=0; float val=0.0f;
	float *kernel1 = AllocateKernelMemory(kz);
	
				wfile>>c; //read [
				if(c != '[')
				{
					TRACE(_T(" expecting to read line startwith [ but found %c\n"), c);
					return -1;
				}
				for(int ikx=0; ikx < kz; ikx++)
				{
					wfile>> val;
					kernel1[k_index++]=val;
				}
				wfile>>c; //read ]
				if(c != ']')
				{
					TRACE(_T(" expecting to read line end with ] but found %c\n"), c);
					return -1;
				}
			
	//kernel has been read
	//read bias too
	int bias_len=0;
	wfile>>bias_len;
	if(bias_len != kz)
	{
		TRACE(_T("length of bias vector should be %d, but found %d\n"), kz,bias_len);
	}
	val =0; k_index =0;
	float *bias = AllocateKernelMemory(bias_len);
	wfile>>c; //read [
	if(c != '[')
				{
					TRACE(_T(" expecting to read line startwith [ but found %c\n"), c);
					return -1;
				}
	for(int ib=0; ib < bias_len; ib++)
		wfile>>bias[ib];
	wfile>>c; //read ]
	if(c != ']')
				{
					TRACE(_T(" expecting to read line end with ] but found %c\n"), c);
					return -1;
				}
	//Now apply depthwise separable convolution in threads
	HANDLE h[30];
	int no_of_threads = min(kz,MAX_THREADS);
	struct thread_data th;
	if(layers[current_layer_no].no_of_input_data != 1)
	{
		TRACE(_T("no of input data for this layer BN - should be one"));
	}
	th.no_of_inputs =1; // only one input
	th.inputs[0]= layers[current_layer_no].input_data[0];
	th.input_size_x =layers[current_layer_no].input_image_size_x;
	th.input_size_y= layers[current_layer_no].input_image_size_y;
	th.input_size_z = layers[current_layer_no].input_image_size_z;
	th.kernel = kernel1;
	th.kernel_size_x = 1;
	th.kernel_size_y  = 1;
	th.kernel_size_z = kz;
	th.kernel_count =1;
	th.bias = bias;
	th.output_image_size_x = th.input_size_x; //padding same
	th.output_image_size_y = th.input_size_y;
	th.output_image_size_z = th.input_size_z; 
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x);
	int zstep = kz /no_of_threads;
	int startz=0;
	struct thread_data th1[30];
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >kz)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > kz)
			th1[t1].zto = kz;
		if(t1 == no_of_threads-1)
			th1[t1].zto = kz;
		h[t1]= (HANDLE)_beginthread(BatchNormalization,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish BatchNormalization threads"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T(" BatchNormalization thread %d finished\n"), t1);
	}
	//  completed.
	




	
	layers[current_layer_no].result = th.result;
	layers[current_layer_no].output_image_size_x =th.output_image_size_x;
	layers[current_layer_no].output_image_size_y=th.output_image_size_y;
	layers[current_layer_no].output_image_size_z=th.output_image_size_z;





	//free and assign memory
	FreeDataMemory(th.inputs[0]);
	FreeKernelMemory(kernel1);
	FreeKernelMemory(bias);

	
	for(int i=0; i < layers[current_layer_no].no_of_output_layers; i++)
	{
		int id = layers[current_layer_no].output_layer_ids[i];
		layers[id].input_data[layers[id].no_of_input_data++]= ReferDataMemory(th.result);
		if(layers[id].input_image_size_z != th.output_image_size_z )
		{
			TRACE(_T("Error - output of %s layer having z =%d while input expected by layer %d %s should have z=%d\n"),layers[current_layer_no].name, th.output_image_size_z, id,layers[id].name, layers[id].input_image_size_z );
			
		}
		layers[id].input_image_size_z = th.output_image_size_z;
		layers[id].input_image_size_y = th.output_image_size_y;
		layers[id].input_image_size_x = th.output_image_size_x;
	}
	if(layers[current_layer_no].no_of_output_layers ==0)
	{
		TRACE(_T("This is last layer. Preseving output reference. \n"));
		//calling freedatamemory will delete output.
	}
	else
		FreeDataMemory(layers[current_layer_no].result);
	return 1;
}

void /*unsigned long _stdcall*/ Add(void *lpParam ) //padding same
{
	struct thread_data *th=(struct thread_data *) lpParam;
	
	
	for(int z=th->zfrom; z< th->zto; z++)  //in each channel perform conv2d
	{
		
		int ic_ind = z*th->input_size_x *th->input_size_y;
		memset(&(th->result[ic_ind]),0,th->output_image_size_x * th->output_image_size_y* sizeof(float));
		for(int i=0; i < th->no_of_inputs; i++)
		{
			float *img= th->inputs[i];
			for(int y=0; y <th->input_size_y; y++)
				for(int x=0; x < th->input_size_x; x++)
				{
				
					int oindex = ic_ind + y*th->input_size_x +x; //input index and output index same
					//if possible in place without creating extra memory
					//No, for generality, inputs are not modified directly, so using another memory
					//th->result[oindex]=;
					th->result[oindex] += img[oindex];

				}
		}
	}
	

	_endthread();
	return;
}


int ReadExecuteAddLayer(struct layer_info *layers, int current_layer_no, std::ifstream & wfile)
{
	//TRACE(_T("Reading ADD layer %d \n"),current_layer_no);
	std::string line;
	ashu_getline(wfile, line); //read layer name type
	if(line.length() < 1)  //skip empty line or left newline character only
		ashu_getline(wfile, line); //read layer name type
	if(line[0] != '#')
	{
		TRACE(_T(" expecting to read line startwith # but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read config
	if(line[0] != '%')
	{
		TRACE(_T(" expecting to read line startwith % but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read input
	if(line[0] != '>')
	{
		TRACE(_T(" expecting to read line startwith > but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read output
	if(line[0] != '<')
	{
		TRACE(_T(" expecting to read line startwith < but found %s\n"), line.c_str());
		return -1;
	}
	
	//Now apply depthwise separable convolution in threads
	HANDLE h[30];
	int kz =layers[current_layer_no].input_image_size_z;
	int no_of_threads = min(kz,MAX_THREADS);
	struct thread_data th;
	
	th.no_of_inputs =layers[current_layer_no].no_of_input_data;
	for(int i=0; i < th.no_of_inputs; i++)
		th.inputs[i]= layers[current_layer_no].input_data[i];
	th.input_size_x =layers[current_layer_no].input_image_size_x;
	th.input_size_y= layers[current_layer_no].input_image_size_y;
	th.input_size_z = layers[current_layer_no].input_image_size_z;
	
	th.output_image_size_x = th.input_size_x; //padding same
	th.output_image_size_y = th.input_size_y;
	th.output_image_size_z = th.input_size_z; 
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x);
	memset((th.result),0,th.output_image_size_z*th.output_image_size_x * th.output_image_size_y* sizeof(float));
	int zstep = kz /no_of_threads;
	int startz=0;
	struct thread_data th1[30];
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >kz)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > kz)
			th1[t1].zto = kz;
		if(t1 == no_of_threads-1)
			th1[t1].zto = kz;
		h[t1]= (HANDLE)_beginthread(Add,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish Add threads"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T(" add thread %d finished\n"), t1);
	}
	//  completed.
	




	
	layers[current_layer_no].result = th.result;
	layers[current_layer_no].output_image_size_x =th.output_image_size_x;
	layers[current_layer_no].output_image_size_y=th.output_image_size_y;
	layers[current_layer_no].output_image_size_z=th.output_image_size_z;





	//free and assign memory
	for(int i=0; i < th.no_of_inputs; i++)
		FreeDataMemory(th.inputs[i]);
	

	
	for(int i=0; i < layers[current_layer_no].no_of_output_layers; i++)
	{
		int id = layers[current_layer_no].output_layer_ids[i];
		layers[id].input_data[layers[id].no_of_input_data++]= ReferDataMemory(th.result);
		if(layers[id].input_image_size_z != th.output_image_size_z )
		{
			TRACE(_T("Error - output of %s layer having z =%d while input expected by layer %d %s should have z=%d\n"),layers[current_layer_no].name, th.output_image_size_z, id,layers[id].name, layers[id].input_image_size_z );
			
		}
		layers[id].input_image_size_z = th.output_image_size_z;
		layers[id].input_image_size_y = th.output_image_size_y;
		layers[id].input_image_size_x = th.output_image_size_x;
	}
	if(layers[current_layer_no].no_of_output_layers ==0)
	{
		TRACE(_T("This is last layer. Preseving output reference. \n"));
		//calling freedatamemory will delete output.
	}
	else
		FreeDataMemory(layers[current_layer_no].result);
	return 1;
}


void /*unsigned long _stdcall*/ MaxPooling2D(void *lpParam ) //padding same
{
	struct thread_data *th=(struct thread_data *) lpParam;
	
	
	for(int z=th->zfrom; z< th->zto; z++)  //in each channel perform conv2d
	{
		
		int ic_ind = z*th->input_size_x *th->input_size_y;
		int oc_ind = z*th->output_image_size_x *th->output_image_size_y;
		//memset(&(th->result[oc_ind]),0,th->output_image_size_x * th->output_image_size_y* sizeof(float));
		//for(int i=0; i < th->no_of_inputs; i++)
		//{
			float *img= th->inputs[0];
			for(int y=0; y <th->input_size_y; y+=th->pooly )
				for(int x=0; x < th->input_size_x; x+=th->poolx )
				{
					float max_val=img[ic_ind + y*th->input_size_x +x];
					for(int yy=y; yy < y+th->pooly ; yy++)
						for(int xx=x; xx < x+th->poolx ; xx++)
						{
							int iindex = ic_ind + yy*th->input_size_x +xx;
							if(img[iindex] > max_val)
								max_val = img[iindex];
						}
					int oindex = oc_ind + (y/th->pooly)*th->output_image_size_x +(x/th->poolx ); 
					th->result[oindex] = max_val;

				}
		//}
	}
	

	_endthread();
	return;
}



int ReadExecuteMaxPooling2DLayer(struct layer_info *layers, int current_layer_no, std::ifstream & wfile)
{
	//TRACE(_T("Reading MaxPooling2D layer %d\n"),current_layer_no);
	std::string line;
	ashu_getline(wfile, line); //read layer name type
	if(line.length() < 1)  //skip empty line or left newline character only
		ashu_getline(wfile, line); //read layer name type
	if(line[0] != '#')
	{
		TRACE(_T(" expecting to read line startwith # but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read config
	if(line[0] != '%')
	{
		TRACE(_T(" expecting to read line startwith % but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read input
	if(line[0] != '>')
	{
		TRACE(_T(" expecting to read line startwith > but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read output
	if(line[0] != '<')
	{
		TRACE(_T(" expecting to read line startwith < but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //output channels, input channels, len(weights), name
	std::istringstream iss(line);
	//char a[80], b[80], c[80]; will do
	std::string  name; int a,b,c;
	if (!(iss >> a >> b >>c >> name)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteMaxPooling2DLayer iss>>a>>b  from line %s\n"), line.c_str());
				return -1; 
			} // error
	if(name != layers[current_layer_no].name)
	{
		TRACE(_T("\n Error in matching layers name : %s   vs %s ---- ReadExecuteMaxPooling2DLayer\n"), name, layers[current_layer_no].name );
		return -1;
	}
	ashu_getline(wfile, line);
	std::istringstream iss1(line);
	int poolx, pooly;
	if (!(iss1 >> poolx >> pooly )) { 
				TRACE(_T("unable to read pool data correctly in ReadExecuteMaxPooling2DLayer iss>>a>>b from line %s\n"), line.c_str());
				return -1; 
			} // error
	if(a!=b || a!=layers[current_layer_no].input_image_size_z)
	{
		TRACE(_T("z shape not matching in maxpooling2d layer\n"));
	}
	//Now apply depthwise separable convolution in threads
	HANDLE h[30];
	int kz =layers[current_layer_no].input_image_size_z;
	int no_of_threads = min(kz,MAX_THREADS);
	struct thread_data th;
	if(layers[current_layer_no].no_of_input_data != 1)
	{
		TRACE(_T("no of input data for this layer maxpooling2d - should be one"));
	}

	th.no_of_inputs =layers[current_layer_no].no_of_input_data;
	for(int i=0; i < th.no_of_inputs; i++)
		th.inputs[i]= layers[current_layer_no].input_data[i];
	th.input_size_x =layers[current_layer_no].input_image_size_x;
	th.input_size_y= layers[current_layer_no].input_image_size_y;
	th.input_size_z = layers[current_layer_no].input_image_size_z;
	th.poolx = poolx; th.pooly =pooly;
	th.output_image_size_x = th.input_size_x /poolx; //padding same
	th.output_image_size_y = th.input_size_y /pooly;
	if(th.output_image_size_x * poolx != th.input_size_x  || th.output_image_size_y * pooly != th.input_size_y )
	{
		TRACE(_T(" Image size (%d,%d) in layer %d should be divided by poolx=%d, pooly=%d\n"), th.input_size_x,th.input_size_y,current_layer_no,poolx,pooly );
	}
	th.output_image_size_z = th.input_size_z; 
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x);
	//memset((th.result),0,th.output_image_size_z*th.output_image_size_x * th.output_image_size_y* sizeof(float));
	int zstep = kz /no_of_threads;
	int startz=0;
	struct thread_data th1[30];
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >kz)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > kz)
			th1[t1].zto = kz;
		if(t1 == no_of_threads-1)
			th1[t1].zto = kz;
		h[t1]= (HANDLE)_beginthread(MaxPooling2D,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish MaxPooling2D threads"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T(" MaxPooling2D thread %d finished\n"), t1);
	}
	//  completed.
	




	
	layers[current_layer_no].result = th.result;
	layers[current_layer_no].output_image_size_x =th.output_image_size_x;
	layers[current_layer_no].output_image_size_y=th.output_image_size_y;
	layers[current_layer_no].output_image_size_z=th.output_image_size_z;





	//free and assign memory
	for(int i=0; i < th.no_of_inputs; i++)
		FreeDataMemory(th.inputs[i]);
	

	
	for(int i=0; i < layers[current_layer_no].no_of_output_layers; i++)
	{
		int id = layers[current_layer_no].output_layer_ids[i];
		layers[id].input_data[layers[id].no_of_input_data++]= ReferDataMemory(th.result);
		if(layers[id].input_image_size_z != th.output_image_size_z )
		{
			TRACE(_T("Error - output of %s layer having z =%d while input expected by layer %d %s should have z=%d\n"),layers[current_layer_no].name, th.output_image_size_z, id,layers[id].name, layers[id].input_image_size_z );
			
		}
		layers[id].input_image_size_z = th.output_image_size_z;
		layers[id].input_image_size_y = th.output_image_size_y;
		layers[id].input_image_size_x = th.output_image_size_x;
	}
	if(layers[current_layer_no].no_of_output_layers ==0)
	{
		TRACE(_T("This is last layer. Preseving output reference. \n"));
		//calling freedatamemory will delete output.
	}
	else
		FreeDataMemory(layers[current_layer_no].result);
	return 1;
}


void /*unsigned long _stdcall*/ UpSampling2D(void *lpParam ) //padding same
{
	struct thread_data *th=(struct thread_data *) lpParam;
	
	
	for(int z=th->zfrom; z< th->zto; z++)  //in each channel perform conv2d
	{
		
		int ic_ind = z*th->input_size_x *th->input_size_y;
		int oc_ind = z*th->output_image_size_x *th->output_image_size_y;
		//memset(&(th->result[oc_ind]),0,th->output_image_size_x * th->output_image_size_y* sizeof(float));
		//for(int i=0; i < th->no_of_inputs; i++)
		//{
			float *img= th->inputs[0];
			for(int y=0; y <th->output_image_size_y; y++ )
				for(int x=0; x < th->output_image_size_x; x++ )
				{
					int iindex = ic_ind + (y/th->pooly)*th->input_size_x +(x/th->poolx); //size of input is smaller. y/pooly will give y index for input
					int oindex = oc_ind + (y)*th->output_image_size_x +(x ); 
					th->result[oindex] = img[iindex];

				}
		//}
	}
	

	_endthread();
	return;
}


int ReadExecuteUpSampling2DLayer(struct layer_info *layers, int current_layer_no, std::ifstream & wfile)
{
	//TRACE(_T("Reading UpSampling2D layer %d\n"),current_layer_no);
	std::string line;
	ashu_getline(wfile, line); //read layer name type
	if(line.length() < 1)  //skip empty line or left newline character only
		ashu_getline(wfile, line); //read layer name type
	if(line[0] != '#')
	{
		TRACE(_T(" expecting to read line startwith # but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read config
	if(line[0] != '%')
	{
		TRACE(_T(" expecting to read line startwith % but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read input
	if(line[0] != '>')
	{
		TRACE(_T(" expecting to read line startwith > but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //read output
	if(line[0] != '<')
	{
		TRACE(_T(" expecting to read line startwith < but found %s\n"), line.c_str());
		return -1;
	}
	ashu_getline(wfile, line); //output channels, input channels, len(weights), name
	std::istringstream iss(line);
	//char a[80], b[80], c[80]; will do
	std::string  name; int a,b,c;
	if (!(iss >> a >> b >>c >> name)) { 
				TRACE(_T("unable to read model file correctly in ReadExecuteMaxPooling2DLayer iss>>a>>b\n"));
				return -1; 
			} // error
	if(name != layers[current_layer_no].name)
	{
		TRACE(_T("\n Error in matching layers name : %s   vs %s ---- ReadExecuteMaxPooling2DLayer\n"), name, layers[current_layer_no].name );
		return -1;
	}
	ashu_getline(wfile, line);
	std::istringstream iss1(line);
	int poolx, pooly;
	if (!(iss1 >> poolx >> pooly )) { 
				TRACE(_T("unable to read pool data correctly in ReadExecuteMaxPooling2DLayer iss>>a>>b %s\n"),line.c_str());
				return -1; 
			} // error
	if(a!=b || a!=layers[current_layer_no].input_image_size_z)
	{
		TRACE(_T("z shape not matching in maxpooling2d layer\n"));
	}
	//Now apply depthwise separable convolution in threads
	HANDLE h[30];
	int kz =layers[current_layer_no].input_image_size_z;
	int no_of_threads = min(kz,MAX_THREADS);
	struct thread_data th;
	if(layers[current_layer_no].no_of_input_data != 1)
	{
		TRACE(_T("no of input data for this layer maxpooling2d - should be one"));
	}

	th.no_of_inputs =layers[current_layer_no].no_of_input_data;
	for(int i=0; i < th.no_of_inputs; i++)
		th.inputs[i]= layers[current_layer_no].input_data[i];
	th.input_size_x =layers[current_layer_no].input_image_size_x;
	th.input_size_y= layers[current_layer_no].input_image_size_y;
	th.input_size_z = layers[current_layer_no].input_image_size_z;
	th.poolx = poolx; th.pooly =pooly;
	th.output_image_size_x = th.input_size_x * poolx; //padding same
	th.output_image_size_y = th.input_size_y * pooly;
	//if(th.output_image_size_x * poolx != th.input_size_x  || th.output_image_size_y * pooly != th.input_size_y )
	//{
	//	TRACE(_T(" Image size (%d,%d) in layer %d should be divided by poolx=%d, pooly=%d\n"), th.input_size_x,th.input_size_y,current_layer_no,poolx,pooly );
	//}
	th.output_image_size_z = th.input_size_z; 
	th.result = AllocateDataMemory(th.output_image_size_z*th.output_image_size_y*th.output_image_size_x);
	memset((th.result),0,th.output_image_size_z*th.output_image_size_x * th.output_image_size_y* sizeof(float));
	int zstep = kz /no_of_threads;
	int startz=0;
	struct thread_data th1[30];
	for(int t1=0; t1<no_of_threads; t1++)
	{
		th1[t1] = th;
		th1[t1].zfrom =startz;
		th1[t1].zto = startz + zstep;
		if(startz >kz)
		{
			no_of_threads =t1;
			break;
		}
		if(th1[t1].zto > kz)
			th1[t1].zto = kz;
		if(t1 == no_of_threads-1)
			th1[t1].zto = kz;
		h[t1]= (HANDLE)_beginthread(UpSampling2D,0,&th1[t1]);

		startz += zstep;
	}

	//TRACE(_T("*** waiting to finish UpSampling2D threads"));
	for(int t1=0; t1<no_of_threads; t1++)
	{
			WaitForSingleObject(h[t1], INFINITE); //its waiting for infinite time.
			//TRACE(_T(" UpSampling2D thread %d finished\n"), t1);
	}
	//  completed.
	




	
	layers[current_layer_no].result = th.result;
	layers[current_layer_no].output_image_size_x =th.output_image_size_x;
	layers[current_layer_no].output_image_size_y=th.output_image_size_y;
	layers[current_layer_no].output_image_size_z=th.output_image_size_z;





	//free and assign memory
	for(int i=0; i < th.no_of_inputs; i++)
		FreeDataMemory(th.inputs[i]);
	

	
	for(int i=0; i < layers[current_layer_no].no_of_output_layers; i++)
	{
		int id = layers[current_layer_no].output_layer_ids[i];
		layers[id].input_data[layers[id].no_of_input_data++]= ReferDataMemory(th.result);
		if(layers[id].input_image_size_z != th.output_image_size_z )
		{
			TRACE(_T("Error - output of %s layer having z =%d while input expected by layer %d %s should have z=%d\n"),layers[current_layer_no].name, th.output_image_size_z, id,layers[id].name, layers[id].input_image_size_z );
			
		}
		layers[id].input_image_size_z = th.output_image_size_z;
		layers[id].input_image_size_y = th.output_image_size_y;
		layers[id].input_image_size_x = th.output_image_size_x;
	}
	if(layers[current_layer_no].no_of_output_layers ==0)
	{
		TRACE(_T("This is last layer. Preseving output reference. \n"));
		//calling freedatamemory will delete output.
	}
	else
		FreeDataMemory(layers[current_layer_no].result);
	return 1;
}

void print_layers(struct layer_info *layers, int no_of_layers)
{
	for(int i=0; i<no_of_layers;i++)
	{
		std::string str = "";
		str+= std::to_string(i);
		str+= " ";
		str += std::string(layers[i].name);
		str += std::string(" input from layers -> ");
		for(int j=0; j< layers[i].no_of_input_layers;j++)
			str += (std::string(layers[layers[i].input_layer_ids[j]].name) + std::string(" "));
		str += std::string(" output to layers ->  ");
		for(int j=0; j < layers[i].no_of_output_layers;j++)
			str += (std::string(layers[layers[i].output_layer_ids[j]].name) + std::string(" "));
		str+= std::string("\n");

		TRACE(_T("%s"),str.c_str());
	}
}
/*
How to use our function - simple example using CxImage. You can read images using any other library such as Opencv or CImg.


	image = new CxImage(filename); //open image file
	int sizex= image.GetWidth();
	int sizey=image.GetHeight();

	//resize image . width /height should be in multiple of 32 or any other number depending upon our keras network
	CxImage *dst=NULL;
	//sizex=256; sizey=256;
	dst = new CxImage(256,256,image.GetType());
	image->Resample2(256,256,CxImage::IM_BICUBIC2, CxImage::OM_REPEAT,dst);
	int h = dst->GetHeight();
	int w= dst->GetWidth();;
	
	int *img = new int[h*w*3];
	int *img_r = img, *img_g =&img[h*w], *img_b=&img[2*h*w];
   
	
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w;x++) {
			 RGBQUAD rgb= dst->GetPixelColor(x,y);
			 *img_r= rgb.rgbRed; img_r++;
			 *img_g= rgb.rgbGreen; img_g++; 
			 *img_b = rgb.rgbBlue; img_b++;
			
		  }
		 
	   }
	
	delete dst;

   char *modelfile ="C:\\bcrpath\\modellayersonlytext.txt";  
   char *weightfile="C:\\bcrpath\\modeltext.txt";

   int *result = ExecuteKerasSegmentation(img, h, w, 3, modelfile, weightfile);

	delete img;
	if(result==NULL)
	{
		AfxMessageBox("Keras error returned null");
		return;
	}

	save_image_pgm("C:\\bcrpath\\segmentation_map.pgm",result,h,w,127);
		
	//converting result back into CxImage format
	CxImage *newImage1 = new CxImage(image);
	newImage1->Resample2(w,h,CxImage::IM_BICUBIC2, CxImage::OM_REPEAT);
				for(int y=0;y< h; y++)
						for(int x=0; x < w;x++)
						{

							BYTE val = result[y*w + x];
							if(val <0)
								val=0;
								newImage1->SetPixelColor(x,y,RGB(val,val,val));

						}
	newImage1->Resample2(image.GetWidth(), image.GetHeight(),CxImage::IM_NEAREST_NEIGHBOUR, CxImage::OM_REPEAT);




*/

int *ExecuteKerasSegmentation(int *img, int &h, int &w, int channels, char *modelfile, char*weightfile)
{
	//Initialization of variables , reading of files
	int *result = NULL;
	int no_of_layers=ReadNoOflayers(modelfile);
	struct layer_info *layers = new struct layer_info[no_of_layers];
	int maxfilters = ReadLayersName(layers, no_of_layers,modelfile);
	int err = ReadLayersConnections(layers, no_of_layers,modelfile);
	if(maxfilters == -1 || err ==-1)
	{
		TRACE(_T(" Error - exiting keras"));
		return NULL;
	}
	//print layers
	print_layers(layers,no_of_layers);
	int max_mem_size = GetMaxMemorySize(layers, no_of_layers,h,w,channels);
	InitMemoryChunk(DataMemChunk,max_mem_size);
	InitMemoryChunk(KernelMemChunk,maxfilters*maxfilters*3*3);


	//open weightfile
	std::ifstream infile( weightfile );
	std::string line;
	ashu_getline(infile, line); //read no of layers
	ashu_getline(infile, line); //read input layer
	ashu_getline(infile, line); //read input layer config
	ashu_getline(infile, line); //read input layer input
	ashu_getline(infile, line); //read input layer output
	//prepare keras input layer
	float *input1 = AllocateDataMemory();
	//copy img to input1 and do required preprocessing
	//assuming input image img is already in keras2cpp format, if not convert it too
	/*
	float *img_r = input1, *img_g =&input1[h*w], *img_b=&input1[2*h*w];
   	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w;x++) {
			 // r= img[y*w+h] g=img[y*w+h+1] b= img[y*w+h+2]
			 *img_r= img[y*w+h]; img_r++;
			 *img_g= img[y*w+h+1]; img_g++; 
			 *img_b = img[y*w+h+@]; img_b++;
			
		  }
		 
	   }
	   */

	for(int j=0; j<h*w*channels;j++)
	{
		input1[j]= float(img[j]);
	}
	for(int j=0; j < layers[0].no_of_output_layers;j++)
	{
		int out= layers[0].output_layer_ids[j];
		layers[out].input_data[layers[out].no_of_input_data]=ReferDataMemory(input1);
		layers[out].input_image_size_x =w;
		layers[out].input_image_size_y =h;
		layers[out].input_image_size_z = channels;
		layers[out].no_of_input_data++;
	}
	//free memory from input layer
	FreeDataMemory(input1);
	//input layer processing finished.

	for(int i=1; i < no_of_layers; i++) //0th layer is input layer
	{
		switch(layers[i].type)
		{
		case Keras_SeparableConv2D:
			err=ReadExecuteSeparableConv2DLayer(layers, i, infile);
			break;
		case Keras_Activation:
			err=ReadExecuteActivationLayer(layers, i, infile);
			break;
		case Keras_BatchNormalization:
			err=ReadExecuteBatchNormalizationLayer(layers, i, infile);
			break;
		case Keras_Add:
			err=ReadExecuteAddLayer(layers, i, infile);
			break;
		case Keras_MaxPooling2D:
			err=ReadExecuteMaxPooling2DLayer(layers, i, infile);
			break;
		case Keras_UpSampling2D:
			err=ReadExecuteUpSampling2DLayer(layers, i, infile);
			break;
		case Keras_Conv2D:
			err=ReadExecuteConv2DLayer(layers, i, infile);
			break;
		default:
			//this type layer not implemented till now.
			TRACE(_T("\n Error : Unknown Layer Not Implemented \n"));
			break;
		}
		if(err ==-1)
		{
			TRACE(_T(" Error - exiting keras"));
			return NULL;
		}
	}

	//convert last layer result into prediction/segmentation map of single channel
	w = layers[no_of_layers-1].output_image_size_x;
	h = layers[no_of_layers-1].output_image_size_y;
	int zdepth = layers[no_of_layers-1].output_image_size_z;

	result = new int[h*w];
	std::string temppredfile="C:\\bcrpath\\temp_pred.txt";
	std::ofstream predtxt(temppredfile);
	float *pred = layers[no_of_layers-1].result;
	for(int y=0; y <h; y++)
		for(int x=0; x < w; x++)
		{
			int maxz=0;
			float maxv=-1e30;
			for(int z=0; z<zdepth;z++)
			{
				
				int index = z*h*w + y*w + x;
				//TRACE(_T("*** pred index %d %d %d val %f  "), x,y,z, pred[index]);
				predtxt<<pred[index]<<"\t";
				if(maxv < pred[index])
				{
					maxv = pred[index];
					maxz=z;
				}
			}
			//result[y*w+x]=maxz; //no transpose result
			result[y*w+x]=min(1,maxz);
			predtxt<<"\n";
		}
	predtxt.close();
	FreeDataMemory(pred);


	ExitMemoryChunk(DataMemChunk);
	ExitMemoryChunk(KernelMemChunk);
	delete [] layers;

	return result;
}