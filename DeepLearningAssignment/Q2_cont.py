import numpy as np
import matplotlib.pyplot as plt

def sigmoid(x):
    return 1.0/(1.0+np.exp(-x))


def MSE(y,y_predict):
    return sum((y-y_predict)**2)/2

def accuracy(y,y_predict):
    count=0
    y_predict=[1 if i > 0.5 else 0 for i in y_predict]
    for i in range(0,len(y)):
        if y[i]==y_predict[i]:
            count+=1
    return count/len(y)

w1=np.random.normal(size=(2,2))
w2=np.random.normal(size=(2,1))

b1=np.random.normal(size=(1,2))
b2=np.random.normal(size=(1,1))


X=np.array([[0, 0], [0, 1], [1, 0], [1, 1]])
y=np.array([[0], [1], [1], [0]])

num_epochs=10000
lr=0.2
trainLoss,trainAcc,epochs=[],[],[]
for epoch in range(1,num_epochs+2):
    if(epoch==num_epochs+1):
        z1=X@w1+b1
        a1=sigmoid(z1)
        z2=a1@w2+b2
        a2=sigmoid(z2)

        loss=MSE(y,a2)
        acc=accuracy(y,a2)
        print('test loss %.4f, test acc %.3f'%(loss, acc))
        break
    epochs.append(epoch)
    z1=X@w1+b1
    a1=sigmoid(z1)

    z2=a1@w2+b2
    a2=sigmoid(z2)

    loss=MSE(y,a2)
    acc=accuracy(y,a2)
    trainLoss.append(loss)
    trainAcc.append(acc)

    if(epoch%100==0):
        print('epoch %d, train loss %.4f, train acc %.3f'%(epoch, loss, acc))
    


    delta=(y-a2)*a2*(1-a2)
    w2_grad=a1.T@delta
    b2_grad=np.sum(delta,axis=0)
    delta=delta@w2.T*(a1*(1-a1))
    w1_grad=X.T@delta
    b1_grad=np.sum(delta,axis=0)

#更新参数
    w1+=lr*w1_grad
    b1+=lr*b1_grad
    w2+=lr*w2_grad
    b2+=lr*b2_grad

plt.plot(epochs,trainLoss,label="loss")
plt.legend()
plt.show()
plt.plot(epochs,trainAcc,label="accuracy")
plt.legend()
plt.show()