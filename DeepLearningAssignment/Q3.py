import os
import torch
import torch.nn as nn
import torch.utils.data as data
import torchvision
from torchvision import datasets,transforms
from torchvision.utils import save_image
import matplotlib.pyplot as plt

 
# 配置GPU或CPU设置
device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
 
# 创建目录保存生成的图片
sample_dir = 'minist_sample'
if not os.path.exists(sample_dir):
    os.makedirs(sample_dir)
 
# 超参数设置
image_size = 784   #图片大小
h_dim = 512
z_dim = 2
num_epochs = 10   #15个循环
learning_rate = 0.01  #学习率


dataset = torchvision.datasets.MNIST(root='./data',
                                     train=True,
                                     transform=transforms.ToTensor(),
                                     download=False)
 

train_iter = torch.utils.data.DataLoader(dataset=dataset,
                                          batch_size=256,
                                          shuffle=True)


 
 
# VAE模型
class VAE(nn.Module):
    def __init__(self, image_size=784, h_dim=512, z_dim=2):
        super(VAE, self).__init__()
        self.fc1 = nn.Linear(image_size, 256)
        self.fc1_hiden = nn.Linear(256, h_dim)
        self.fc2 = nn.Linear(h_dim, z_dim)
        self.fc3 = nn.Linear(h_dim, z_dim)
        self.fc4 = nn.Linear(z_dim, h_dim)
        self.fc5 = nn.Linear(h_dim, image_size)
 

    def encode(self, x):
        h = torch.relu(self.fc1(x))
        t=torch.tanh(self.fc1_hiden(h))
        return self.fc2(t), self.fc3(t)
 

    def reparameterize(self, mu, log_var):
        std = torch.exp(log_var / 2)
        eps = torch.randn_like(std)
        return mu + eps * std
 
 
    def decode(self, z):
        h = torch.relu(self.fc4(z))
        return torch.sigmoid(self.fc5(h))
 

    def forward(self, x):
        mu, log_var = self.encode(x)  
        z = self.reparameterize(mu, log_var)  
        x_reconst = self.decode(z)  
        return x_reconst, mu, log_var 
 
 

model = VAE().to(device)

optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)

reconstLoss=[]
klDiv=[]
for epoch in range(num_epochs):
    n,reconst_loss_epoch,kl_div_epoch=0,0,0
    for i, (x, _) in enumerate(train_iter):
        x = x.to(device).view(-1,image_size)  
        x_reconst, mu, log_var = model(x)  

        reconst_loss = torch.nn.functional.binary_cross_entropy(x_reconst, x, size_average=False)
        kl_div = - 0.5 * torch.sum(1 + log_var - mu.pow(2) - log_var.exp())
        
        kl_div_epoch+=kl_div.item()
        reconst_loss_epoch+=reconst_loss.item()
        n += 256
        loss = reconst_loss + kl_div
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        
 
    
    print('epoch %d, reconst loss %.4f, kl div %.3f'%(epoch+1, reconst_loss_epoch/n, kl_div_epoch/n))
    reconstLoss.append(reconst_loss_epoch/n)
    klDiv.append(kl_div_epoch/n)
 
 
    # 保存重构值
    # 将batch_size*748的x输入模型进行前向传播计算，获取重构值out
    out, _, _ = model(x)
    # 将输入与输出拼接在一起输出保存  batch_size*1*28*（28+28）=batch_size*1*28*56
    x_concat = torch.cat([x.view(-1, 1, 28, 28), out.view(-1, 1, 28, 28)], dim=3)
    save_image(x_concat, os.path.join(sample_dir, 'reconst-{}.png'.format(epoch + 1)))

for epoch in range(0,num_epochs):
    z = torch.randn(256, z_dim).to(device)  # z的大小为batch_size * z_dim = 128*20
    # 对随机数 z 进行解码decode输出
    out = model.decode(z).view(-1, 1, 28, 28)
    # 保存结果值
    save_image(out, os.path.join(sample_dir, 'sampled-{}.png'.format(epoch + 1)))
epochs=[epoch for epoch in range(1,11)]
plt.plot(epochs,reconstLoss,label="reconst loss")
plt.legend()
plt.show()
plt.plot(epochs,klDiv,label="kl div")
plt.legend()
plt.show()