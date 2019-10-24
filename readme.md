# c89net

Simple deep neural network using C89, feed-forward only(for now)

使用 ASIC 实现的 DNN 网络可以在丰富的系统上部署和实验，目前仅支持前馈。在 Linux / MacOS 操作系统上使用 `make` 编译(Windows 系统建议使用 mingw)。如你所知，使用 c89 几乎可以让它兼容在所有 32 位操作系统上(亦可以不依赖操作系统，独立运行)。

## LeNet
[LeNet-5, convolutional neural networks](http://yann.lecun.com/exdb/lenet/)
经典的卷积神经网络模型，用于手写字体识别任务。我们训练好了模型，保存在 `~/lenet-parameters/` 你可以直接使用。
使用 [c89net-tf-model](http://404.404/) 训练你自己的数据，编辑 `~/lenet.c` 修改模型。

## 注意事项

*特别注意 1，由于未使用第三方图形库，LeNet 的测试请使用位图(bmp)格式。*

*特别注意 2，目前尚未制定标准的模型数据格式，目前直接使用 **little-endian** 的单精度浮点数 float32 内存 dump 数据提供模型参数*

*特别注意 3，除非我说可以了，否则任何你在这个项目里看到的代码都可能是我没睡醒时摸的... *

*特别注意 4，这个项目大概率 **不会** 咕咕咕*