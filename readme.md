# c89net

Simple deep neural network using C89, feed-forward only(for now)

使用 ASIC 实现的 DNN 网络可以在丰富的系统上部署和实验，目前仅支持前馈。在 Linux/MacOS 操作系统上使用 `make` 编译(Windows 系统建议使用 mingw)。

## LeNet
[LeNet-5, convolutional neural networks](http://yann.lecun.com/exdb/lenet/)
经典的卷积神经网络模型，用于手写字体识别任务。我们训练好了模型，保存在 `~/lenet-parameters/` 你可以直接使用。
使用 [c89net-tf-model](http://404.404/) (特别注意，目前尚未制定标准的模型数据格式，目前直接使用 little-endian 的单精度浮点数(float) 内存 dump 数据提供模型参数)训练你自己的数据，编辑 `~/lenet.c` 修改模型。