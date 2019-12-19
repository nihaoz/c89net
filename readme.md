# c89net

Simple deep neural networks implemented with C89, feed-forward only(for now)

使用 ANSI C 实现的 DNN 网络可以在丰富的系统上部署和实验，目前仅支持前馈。在 Linux / MacOS 操作系统上使用 `make` 编译(Windows 系统建议使用 mingw)。如你所知，使用 c89 几乎可以让它兼容在所有 32 位操作系统上(亦可以不依赖操作系统，独立运行)。我们提供了一些简单的网络以及训练好的参数用于功能测试。

## LeNet
Key Features: *Spatial Convolution 2D, Max Pooling, Fully Connetion, Relu*

[LeNet-5, convolutional neural networks](http://yann.lecun.com/exdb/lenet/)
经典的卷积神经网络模型，用于手写字体识别任务。我们训练好了模型，保存在 `~/lenet-parameters/` 你可以直接使用。这是一个修改过的 LeNet，并不是 LeNet-5，它包含两个卷积层和两个全连接层，适用于 0 ~ 9 的手写数字识别任务。
使用 [c89net-tf-model](https://github.com/evidence0john/c89net-tf-model) 训练你自己的数据，编辑 `~/cnn/lenet/lenet.c` 修改模型。

## TinyNet
Key Features: *Depthwise Separable Convolution 2D, Batch Normalization, Relu6*

c89net 现在支持 Depthwise Separable Convolution 和 Batch Normalization(BN) 等操作，这就意味着可以运行一些当下流行的深度网络了。TinyNet 仍然是一个手写字体识别的简单网络，它的结构和 [MoblieNetV1](https://arxiv.org/abs/1704.04861) 类似。它包括 14 个卷积层(7 个深度可分离卷积，每个深度可分离卷积含有 1 个 depthwise 卷积层和一个 pointwise 卷积层)和一个全连接层，在 pointwise 卷积之后还有一个 BN 层。TinyNet 能够达到和 LeNet 相当的精度，而它仅有 4.7 k 个可训练参数，参数上约是 LeNet 的 0.1%，在 CPU 上，计算速度比 LeNet 快 10 倍以上。

使用 [c89net-tf-model](https://github.com/evidence0john/c89net-tf-model) 训练你自己的数据，编辑 `~/cnn/tinynet/tinynet.c` 修改模型。

## 注意事项

*1，由于未使用第三方图形库，测试请使用位图(bmp)格式的图像文件。*

*2，目前尚未制定标准的模型数据格式，目前直接使用 **little-endian** 的单精度浮点数 float32 内存 dump 数据提供模型参数*

*3，除非我说可以了，否则任何你在这个项目里看到的代码都可能是我没睡醒时摸的*

*4，这个项目大概率 **不会** 咕咕咕*
