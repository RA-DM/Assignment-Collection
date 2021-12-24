import numpy as np
import torch
from torch.utils import data
from torch import nn
import matplotlib.pyplot as plt
#Q1.2
a=np.array([4,5,6])
print("Q1.2: ")
print(type(a))
print(a.ndim)
print(a[0])

#Q1.3
b=np.array([[4,5,6],[1,2,3]])
print("\nQ1.3: ")
print(b.ndim)
print(b[0][0],b[0][1],b[1][1])

#Q1.4
c=np.zeros((3,3),dtype=np.int)
d=np.ones((4,5),dtype=np.int)
e=np.identity(4,dtype=np.int)

#Q1.5
f=np.arange(0, 12, dtype=np.int)
f=f.reshape((3,4))
print("\nQ1.5: ")
print(f.ndim)
print(f[0].ndim)
print(f[1,:])
print(f[:,2:])
print(f[-1,-1])

#Q2.1

#得到数据集
num_observeation = 100
x=np.linspace(-3,3,num_observeation)
y=np.sin(x)+np.random.uniform(-0.5,0.5,num_observeation)
plt.scatter(x,y)
plt.show()

X=np.expand_dims(x,axis=1)
Y=y.reshape(100,-1)
def load_array(data_arrays, batch_size, is_train=True):
    dataset = data.TensorDataset(*data_arrays)
    return data.DataLoader(dataset, batch_size, shuffle=is_train)

batch_size = 10
data_iter = load_array((torch.tensor(X,dtype=torch.float), torch.tensor(Y,dtype=torch.float)), batch_size)


#一次项线性回归
class Net(nn.Module):
  def __init__(self):
    super().__init__()
    self.w=torch.nn.Parameter(torch.randn(()))
    self.b=torch.nn.Parameter(torch.randn(()))
 
  def forward(self, x:torch.FloatTensor):
    return self.b+self.w*x

net=Net()

optim=torch.optim.Adam(Net.parameters(net),lr=0.03)
Loss=nn.MSELoss()

for epoch in range(1000):
  loss=None
  for batch_x,batch_y in data_iter:
    y_predict=net(batch_x)
    loss=Loss(y_predict,batch_y)
    optim.zero_grad()
    loss.backward()
    optim.step()

  if (epoch+1)%100==0:
    print("step: {0} , loss: {1}".format(epoch+1,loss.item()))
print("w: ",net.w)
print("b: ",net.b)

#绘图
predict=net(torch.tensor(X,dtype=torch.float))
 
plt.scatter(x,y,label="fact")
plt.plot(x,predict.detach().numpy(),label="predict",color='r')
plt.title("Linear regression")
plt.xlabel("x")
plt.ylabel("sin(x)")
plt.legend()
plt.show()


#三次项线性回归
class Net_2(nn.Module):
  def __init__(self):
    super().__init__()
    self.w1=torch.nn.Parameter(torch.randn(()))
    self.w2=torch.nn.Parameter(torch.randn(()))
    self.w3=torch.nn.Parameter(torch.randn(()))
    self.b=torch.nn.Parameter(torch.randn(()))
 
  def forward(self, x:torch.FloatTensor):
    return self.b+self.w1*x+self.w2*x**2+self.w3*x**3

net_2=Net_2()

optim=torch.optim.Adam(Net_2.parameters(net_2),lr=0.03)
Loss2=nn.MSELoss()

for epoch in range(1000):
  loss=None
  for batch_x,batch_y in data_iter:
    y_predict=net_2(batch_x)
    loss=Loss2(y_predict,batch_y)
    optim.zero_grad()
    loss.backward()
    optim.step()
  # 每100次的时候打印一次日志
  if (epoch+1)%100==0:
    print("step: {0} , loss: {1}".format(epoch+1,loss.item()))
print("w1: ",net_2.w1)
print("w2: ",net_2.w2)
print("w3: ",net_2.w3)
print("b: ",net_2.b)


predict=net_2(torch.tensor(X,dtype=torch.float))

plt.scatter(x,y,label="fact")
plt.plot(x,predict.detach().numpy(),label="predict",color='r')
plt.title("Cubic polynomial regression")
plt.xlabel("x")
plt.ylabel("sin(x)")
plt.legend()
plt.show()


