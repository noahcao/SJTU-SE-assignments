import numpy as np
import random

class Layer(object):
    # the ancestor of layer module class
    def __init__(self):
        self.input_cache = None
        self.output_cache = None
        self.grad_cache = None

    def forward(self, data):
        pass

    def backward(self, data, error, lr):
        pass

    def update(self, data, label, epochs, lr):
        pass

class softmax(object):
    # softmax function
    def __init__(self):
        pass

    def forward(self, input):
        exp = np.exp(input)
        return exp / exp.sum(axis=1, keepdims=True)

class fc(Layer):
    def __init__(self, input_dim, output_dim):
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.W = np.random.randn(self.input_dim, self.output_dim) / np.sqrt(self.input_dim)
        self.b = np.zeros((1, self.output_dim))

    def forward(self, input):
        self.input_cache = input
        self.output_cache = input.dot(self.W) + self.b
        return self.output_cache

    def backward(self, grad, lr):
        dW = np.dot(self.input_cache.T, grad)
        db = np.sum(grad, axis=0)
        bp_grad = np.dot(grad, self.W.T)
        self.grad_cache = bp_grad
        self.W -= lr * dW
        self.b -= lr * db
        return self.grad_cache

# --------------- Activation Layers ---------------------

class sigmoid(Layer):
    def _sigmoid(self,x):
        return 1.0/(1+np.exp(-x))

    def forward(self, input):
        self.input_cache = input
        self.output_cache = self._sigmoid(input)
        return self.output_cache

    def backward(self, grad):
        self.grad_cache = grad * (self.output_cache *(1-self.output_cache))
        return self.grad_cache

class relu(Layer):
    def forward(self, input):
        self.input_cache = input
        self.output_cache = np.maximum(input, 0)
        return output_cache

    def backward(self, grad):
        self.grad_cache = grad
        return self.grad_cache


class tanh(Layer):
    def forward(self, input):
        self.input_cache = input
        self.output_cache = np.tanh(input)
        return self.output_cache

    def backward(self, grad):
        self.grad_cache = grad * (1 - self.output_cache ** 2)
        return self.grad_cache
