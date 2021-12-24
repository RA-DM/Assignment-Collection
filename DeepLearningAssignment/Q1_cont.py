import torch
import torch.utils.data as data
from torch import nn
import torchvision
from torchvision import datasets,transforms
import numpy as np
import matplotlib.pyplot as plt

dataset = torchvision.datasets.MNIST(root='./data',
                                     train=True,
                                     transform=transforms.ToTensor(),
                                     download=True)

dataset2 = torchvision.datasets.MNIST(root='./data',
                                     train=False,
                                     transform=transforms.ToTensor(),
                                     download=True)

train_iter = torch.utils.data.DataLoader(dataset=dataset,
                                          batch_size=256,
                                          shuffle=True)
                                          

test_iter = torch.utils.data.DataLoader(dataset=dataset2,
                                          batch_size=256,
                                          shuffle=True)

#Q1(cont.)
num_input=784
num_output=10
net = nn.Sequential(nn.Flatten(), nn.Linear(num_input, num_output))

def init_weights(m):
    if type(m) == nn.Linear:
        nn.init.normal_(m.weight, std=0.01)

net.apply(init_weights)

loss=nn.CrossEntropyLoss()
optimizer=torch.optim.SGD(net.parameters(),lr=0.1)

num_epochs=10
x=[i for i in range(1,num_epochs+1)]
trainLoss=[]
trainAcc=[]
testLoss=[]
testAcc=[]
for epoch in range(1,num_epochs+1):
    test_acc_count=0
    test_loss=0
    train_acc_count=0
    train_loss=0

    n=0
    for X,y in train_iter:
        y_predict=net.forward(X)
        l=loss(y_predict,y).sum()
        optimizer.zero_grad()
        l.backward()
        optimizer.step()
        train_acc_count+=(y_predict.argmax(axis=1)==y).sum().item()
        train_loss+=l.item()
        n+=y.shape[0]
    print('epoch %d, train loss %.4f, train acc %.3f'%(epoch, train_loss/n, train_acc_count/n))
    trainLoss.append(train_loss/n)
    trainAcc.append(train_acc_count/n)

    n=0
    for X,y in test_iter:
        y_predict=net.forward(X)
        test_acc_count+=(y_predict.argmax(axis=1)==y).sum().item()
        test_loss+=loss(y_predict,y).sum().item()
        n+=y.shape[0]
    print('epoch %d, test loss %.4f, test acc %.3f'%(epoch, test_loss/n, test_acc_count/n))
    testLoss.append(test_loss/n)
    testAcc.append(test_acc_count/n)

plt.plot(x,trainLoss,label="train")
plt.plot(x,testLoss,label="test",color='r')
plt.legend()
plt.show()
plt.plot(x,trainAcc,label="train")
plt.plot(x,testAcc,label="test",color='r')
plt.legend()
plt.show()
