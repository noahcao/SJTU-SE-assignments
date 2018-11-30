import numpy as np
import random
from sklearn import preprocessing
import LayerModule

def loss_function(prediction, ground_truth):
    return prediction - ground_truth

class FlexibleNN(object):
    def __init__(self, layer_list):
        self.layers = []
        input_dim = None
        assert isinstance(layer_list[0], list)
        for layer in layer_list:
            if isinstance(layer, list):
                # the definition of fc-layers
                assert len(layer) > 1
                if input_dim == None:
                    # the beginning of network
                    input_dim = layer[0]
                for i in range(len(layer) - 1):
                    output_dim = layer[i+1]
                    self.layers.append(LayerModule.fc(input_dim, output_dim))
                    input_dim = layer[i+1]
            elif layer == 'tanh':
                self.layers.append(LayerModule.tanh())
            elif layer == 'relu':
                self.layers.append(LayerModule.relu())
            elif layer == 'sigmoid':
                self.layers.append(LayerModule.sigmoid())
            else:
                # illegal item in the layer setting 
                assert(0)

        self.softmax = LayerModule.softmax()
        self.layer_num = len(self.layers)
    
    def forward(self, x):
        for layer in self.layers:
            x = layer.forward(x)
        return self.softmax.forward(x)

    def backward(self, grad, lr):
        for i in range(self.layer_num):
            layer_index = self.layer_num - i - 1
            layer = self.layers[layer_index]
            if isinstance(layer, LayerModule.fc):
                grad = layer.backward(grad, lr)
            else:
                # no learning rate is needed for actication layers
                grad = layer.backward(grad)

    def update(self, data, label, epochs, lr):
        ins_num = len(label)
        for epoch in range(epochs):
            output = self.forward(data)
            error = loss_function(output, label)
            self.backward(error, lr)
            pred = self.forward(data)
            hit = 0.0
            for i in range(ins_num):
                pred_cls = np.argmax(pred[i])
                gt_cls = np.argmax(label[i])
                if pred_cls == gt_cls:
                    hit += 1
            print("epoch: {} | acc: {}".format(epoch, hit/ins_num))     


class NerualNet(object):
    def __init__(self, input_dim, hidden_dim, out_cls):
        self.fc1 = LayerModule.fc(input_dim, hidden_dim)
        self.tanh = LayerModule.tanh()
        self.fc2 = LayerModule.fc(hidden_dim, out_cls)
        self.Softmax = LayerModule.softmax()

    def forward(self, data):
        x = self.fc1.forward(data)
        x = self.tanh.forward(x)
        x = self.fc2.forward(x)
        prob = self.Softmax.forward(x)
        return prob

    def backward(self, error, lr):
        grad = self.fc2.backward(error, lr)
        grad = self.tanh.backward(grad)
        grad = self.fc1.backward(grad, lr)


    def update(self, data, label, epochs, lr):
        ins_num = len(label)
        for epoch in range(epochs):
            output = self.forward(data)
            error = loss_function(output, label)
            self.backward(error, lr)
            pred = self.forward(data)
            hit = 0.0
            for i in range(ins_num):
                pred_cls = np.argmax(pred[i])
                gt_cls = np.argmax(label[i])
                if pred_cls == gt_cls:
                    hit += 1
            print("epoch: {} | acc: {}".format(epoch, hit/ins_num))

def loadfile_iris():
    label_dict = {'Iris-setosa':0, 'Iris-versicolor':1, 'Iris-virginica':2,}
    datafile = 'iris.txt'
    f = open(datafile, 'r')
    lines = f.readlines()
    data = []
    label = []
    for line in lines:
        items = line.split(',')
        y = label_dict[items[-1].strip()]
        x = [float(i) for i in items[:-1]]
        data.append(x)
        gt = [0,0,0]
        gt[int(y)] = 1
        label.append(gt)
    return np.array(data), np.array(label)
        

def main():
    train_data, train_label = loadfile_iris()
    train_data = preprocessing.scale(train_data)
    #nn = NerualNet(4,3,3)
    nn = FlexibleNN([[4,3], 'tanh', [3,3]])
    nn.update(train_data, train_label, 100, 0.1)
        
if __name__ == "__main__":
    main()