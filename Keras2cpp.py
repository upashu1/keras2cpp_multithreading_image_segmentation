from os import truncate
from tensorflow.keras import backend as K
import numpy as np
def save_keras_model_as_text(model,fout, noweight=False):
  #with open(textfilename, 'w') as fout:
    n=len(model.layers)
    fout.write('layers ' + str(len(model.layers)) + '\n')
    layers = model.layers
    for layer in layers:
      layer_type = type(layer).__name__
      print('# ' + str(layer_type) + ' ' + str(layer.name))
      fout.write('# ' + str(layer_type) + ' ' + str(layer.name) + ' ' +str( layer.output_shape[-1]) + '\n')
      config = layer.get_config()
      print('% ' + str(config))
      fout.write('% ' + str(config) + '\n')
      fout.write('> ')
      for int_node in layer._inbound_nodes:
        l = int_node.inbound_layers
        if isinstance(l, list):
          for k in l:
            fout.write(str(k.name) + '   ')
        else:
          fout.write(str(l.name))
      fout.write('\n<  ')  
      #for int_node in layer._outbound_nodes:
      #  l = int_node.inbound_layers
      #  if isinstance(l, list):
      #    for k in l:
      #      fout.write(str(k.name)+ '   ')
        #else:
        #  fout.write(str(l.name))
      fout.write('\n')  
      if layer_type in ['Model', 'Sequential', 'Functional']:
        save_keras_model_as_text(layer,fout)
      if noweight==True:
        continue
      if layer_type=='Conv2D':
         print(layer.input_shape[-1], layer.output_shape[-1], len(layer.weights), layer.name)
         fout.write(str(layer.input_shape[-1]) + ' ' +str( layer.output_shape[-1]) + ' '+ str(len(layer.weights)) + ' ' + str(config['activation']) + ' ' +str( layer.name) + '\n' )
         W = layer.get_weights()[0]
         print(W.shape)
         W = np.asarray(K.permute_dimensions(W, [3,2,1,0]))
         #W= np.asarray(W)
         fout.write(str(W.shape[0]) + ' ' + str(W.shape[1]) + ' ' + str(W.shape[2]) + ' ' + str(W.shape[3]) + ' ' + layer.padding + '\n')
         print(W.shape)
         #fout.write(str(W.flatten()) + '\n')
         for i in range(W.shape[0]):
                for j in range(W.shape[1]):
                    for k in range(W.shape[2]):
                        fout.write(str(W[i,j,k]) + '\n')
                    
         W1 = layer.get_weights()[1]
         fout.write(str(W1.shape[0]) + '\n')
         fout.write(str(W1.flatten()) + '\n')

      if layer_type=='SeparableConv2D':
          print(layer.input_shape[-1], layer.output_shape[-1], len(layer.weights), layer.name)
          fout.write(str(layer.input_shape[-1]) + ' ' +str( layer.output_shape[-1]) + ' '+ str(len(layer.weights)) + ' ' + str(config['activation']) + ' '  +str( layer.name) +'\n' )
          W = layer.get_weights()[0]
          print(W.shape)
          W = np.asarray(K.permute_dimensions(W, [3,2,1,0]))
          fout.write(str(W.shape[0]) + ' ' + str(W.shape[1]) + ' ' + str(W.shape[2]) + ' ' + str(W.shape[3]) + ' ' + layer.padding + '\n')
          print(W.shape)
          #fout.write(str(W.flatten()) + '\n')
          for i in range(W.shape[0]):
                for j in range(W.shape[1]):
                    for k in range(W.shape[2]):
                        fout.write(str(W[i,j,k]) + '\n')
          W2 = layer.get_weights()[1]
          W2 = np.asarray(K.permute_dimensions(W2, [3,2,1,0]))
          fout.write(str(W2.shape[0]) + ' ' + str(W2.shape[1]) + ' ' + str(W2.shape[2]) + ' ' + str(W2.shape[3]) + ' ' + layer.padding + '\n')
          print(W2.shape)
          W=W2
          #fout.write(str(W2.flatten()) + '\n')
          for i in range(W.shape[0]):
                for j in range(W.shape[1]):
                    for k in range(W.shape[2]):
                        fout.write(str(W[i,j,k]) + '\n')
          W1 = layer.get_weights()[2]
          fout.write(str(W1.shape[0]) + '\n')
          fout.write(str(W1.flatten()) + '\n')

      if layer_type=='BatchNormalization':
          print(layer.input_shape[-1], layer.output_shape[-1], len(layer.weights), layer.name)
          fout.write(str(layer.input_shape[-1]) + ' ' +str( layer.output_shape[-1]) + ' '+ str(len(layer.weights)) + ' ' +str( layer.name) +'\n')
          moving_mean = K.get_value(layer.moving_mean)
          moving_variance = K.get_value(layer.moving_variance)
          beta=0.0
          gamma=0.0
          if layer.center:
            beta = K.get_value(layer.beta)
          
          if layer.scale:
            gamma = K.get_value(layer.gamma)
          print(moving_mean.shape,moving_variance.shape,layer.center,beta.shape, layer.scale, gamma.shape )
          weights = gamma / np.sqrt(moving_variance + layer.get_config()['epsilon'])
          biases = beta - moving_mean * weights
          fout.write(str(weights.shape[0])+'\n')
          fout.write(str(weights.flatten())+ '\n')
          fout.write(str(biases.shape[0])+'\n')
          fout.write(str(biases.flatten())+ '\n')
          #fout.write(str(moving_mean)+ ' ' + str(moving_variance)+ ' ' + str(layer.center) + ' ' + str(beta)+ ' ' + str(layer.scale) + ' ' + str(gamma))

          #fout.write(str(moving_mean.shape[0])+'\n')
          #fout.write(str(moving_mean)+ '\n')
          #fout.write(str(moving_variance.shape[0])+'\n')
          #fout.write(str(moving_variance)+ '\n')
          #fout.write(str(beta.shape[0])+'\n')
          #fout.write(str(beta)+ '\n')
          #fout.write(str(gamma.shape[0])+'\n')
          #fout.write(str(gamma)+ '\n')

      if layer_type=='Activation':
        print(layer.input_shape[-1], layer.output_shape[-1], len(layer.weights), layer.name)
        fout.write(str(layer.input_shape[-1]) + ' ' +str( layer.output_shape[-1]) + ' '+ str(len(layer.weights)) + ' ' +str( layer.name) + '\n')
        #print(layer)
        config = layer.get_config()
        print(config['activation'])
        fout.write(str(config['activation']) + '\n')

      if layer_type=='UpSampling2D':
        fout.write(str(layer.input_shape[-1]) + ' ' +str( layer.output_shape[-1]) + ' '+ str(len(layer.weights)) + ' ' +str( layer.name) + '\n')
        fout.write(str(config['size'][0]) + ' ' + str(config['size'][1]) + '\n')
      if layer_type=='MaxPooling2D':
        fout.write(str(layer.input_shape[-1]) + ' ' +str( layer.output_shape[-1]) + ' '+ str(len(layer.weights)) + ' ' +str( layer.name) + '\n')
        fout.write(str(config['pool_size'][0]) + ' ' + str(config['pool_size'][1]) + '\n')





model=keras.models.load_model('/content/drive/MyDrive/ashu_unet2_b32_nosizeX.h5')
#model=keras.models.load_model('/content/drive/MyDrive/ashu_keras_test1.h5')
#model=keras.models.load_model('/content/drive/MyDrive/ashu_keras_test8.h5') 
save_keras_model_as_text(model, open('/content/modeltext.txt', 'w') )
save_keras_model_as_text(model, open('/content/modellayersonlytext.txt', 'w') , noweight=True)