import torch
from torch.nn.modules.activation import ReLU
import torch.utils.data as data
from torch import nn
from torch.utils.data import dataloader
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
num_input=784
num_output=10
net = nn.Sequential(nn.Flatten(),
                    nn.Linear(num_input, 256),
                    nn.ReLU(),
                    nn.Linear(256, 256),
                    nn.ReLU(),
                    nn.Linear(256, num_output))
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
        train_acc_count+=(y_predict.argmax(dim=1)==y).sum().item()
        train_loss+=l.item()
        n+=y.shape[0]
    print('epoch %d, train loss %.4f, train acc %.3f'%(epoch, train_loss/n, train_acc_count/n))
    trainLoss.append(train_loss/n)
    trainAcc.append(train_acc_count/n)

    n=0
    for X,y in test_iter:
        y_predict=net.forward(X)
        test_acc_count+=(y_predict.argmax(dim=1)==y).sum().item()
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


#CNN
convnet=nn.Sequential(
    nn.Conv2d(in_channels=1,out_channels=32,kernel_size=5,padding='same'),
    nn.ReLU(inplace=True),
    nn.MaxPool2d(kernel_size=2, stride=2),
    nn.Conv2d(in_channels=32,out_channels=64,kernel_size=5,padding='same'),
    nn.ReLU(inplace=True),
    nn.MaxPool2d(kernel_size=2, stride=2),
    nn.Flatten(),
    nn.Linear(7*7*64,1024),
    nn.ReLU(),
    nn.Dropout(p=0.25),
    nn.Linear(1024, num_output)
)

def init_weights(m):
    if type(m) == nn.Linear:
        nn.init.normal_(m.weight, std=0.01)

convnet.apply(init_weights)

loss2=nn.CrossEntropyLoss()
optimizer2=torch.optim.SGD(convnet.parameters(),lr=0.1)

device=torch.device("cuda:0")
def to_device(data,device):
    if(isinstance(data,(list,tuple))):
        return [to_device(x,device) for x in data]
    else:
        return data.to(device,non_blocking=True)

class gpuDataLoader():
    def __init__(self,dataloader,device) -> None:
        self.dataloader=dataloader
        self.device=device

    def __iter__(self):
        for data in self.dataloader:
            yield to_device(data,self.device)

    def __len__(self):
        return len(self.dataloader)

num_epochs=10
x=[i for i in range(1,num_epochs+1)]
trainLoss=[]
trainAcc=[]
testLoss=[]
testAcc=[]
train_iter=gpuDataLoader(train_iter,device)
test_iter=gpuDataLoader(test_iter,device)

to_device(convnet,device)
for epoch in range(1,num_epochs+1):
    test_acc_count=0
    test_loss=0
    train_acc_count=0
    train_loss=0

    n=0
    for X,y in train_iter:
        y_predict=convnet.forward(X)
        l=loss2(y_predict,y).sum()
        optimizer2.zero_grad()
        l.backward()
        optimizer2.step()
        train_acc_count+=(y_predict.argmax(axis=1)==y).sum().item()
        train_loss+=l.item()
        n+=y.shape[0]
    print('epoch %d, train loss %.4f, train acc %.3f'%(epoch, train_loss/n, train_acc_count/n))
    trainLoss.append(train_loss/n)
    trainAcc.append(train_acc_count/n)

    n=0
    for X,y in test_iter:
        y_predict=convnet.forward(X)
        test_acc_count+=(y_predict.argmax(axis=1)==y).sum().item()
        test_loss+=loss2(y_predict,y).sum().item()
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