六、使用说明

第十三个版本的改进在于将所有分支程序集合在了一个.cpp文件内，以方便使用者的下载编译运行！
如果想在您计算机上的IDE直接编译运行本程序，请添加以下图形库：
图形库链接：https://pan.baidu.com/s/1o9U8YFK 密码：t0wc
图形库可以选择手动安装，将 include 和 lib 文件夹下的文件分别拷贝到IDE安装目录下 VC 对应的 include 和 lib 文件夹内即可。

本程序在读取输入文件时采用按照字符串读入并处理的方法，在等位线连接时采用分组连接的方法，打印不同电位值的等位线时采用渐变色加以区分，
同时程序提供一定程度的错误检查。
1.输入文件：
从文件读入各项参数，读取文件的路径和名称可在程序的宏定义处修改。以下是文件读入参数设置的示例：

δ =0.8mm;//电极厚度
n =5;//电极个数
zi =4.5,  6.5,  6.5,  6.5,  4.5mm;//相邻电极间的距离
Ni =10, 16, 16, 16, 10;//相邻电极之间要划分的步长数
di = 6.3,  6,  6,  6.3mm;//电极的长度
Li  = 14, 20, 20, 14;//电极的要划分的步长数
Vi =20, 62, 38,  81,100; //电极电位
r =20mm;//电极孔径半径 
M=50;//r放范围内等步长划分的网格数
ε=0.000005V; //迭代控制精度
NST=2;//NST=2时隔行隔列打印各网格点电位值，NST=1时全部打印
b =8, 16, 25, 42, 62, 70, （后面是电极电位值）20, 62, 38, 81, 100 ;//扫描电位（b只有一个数字时，将按照该数字的间隔画等位线，例：b=10;
另外如果需要画出电极，请将电极电位添加进扫描电位）

文件输入时的注意：请不要在输入文件中添加不必要的任何英文字母，一组数据以英文分号作为结束。连续的数字之间请全部采用英文标点符号，包括但不限于逗号、分号。
由于采用按字符串读入并处理的读入方法，其余部分可随意添加中文。此外各参数输入的先后对程序无影响。
同时程序提供一定程度的错误检查，防止读取到的文件输入不合法或者数据之间不匹配，实际测试其他人的输入文件将会得到具体的错误提示。

2.输出文件：
和输入文件类似，输出文件的名称和路径也可在程序的宏定义处修改。以下是关于输出文件的说明：
首先，程序会把读取到的参数全部输出一遍，以提供特别情况下的错误检查。
其次，程序会将求解加速因子过程中的变化的加速因子输出，直至输出最后已经固定的加速因子w。
这之后，程序将会输出求解加速因子的迭代轮次，以及固定加速因子后，最终求解出各点电位的迭代轮次。
最后，程序将会按照NST值的情况把最终的各点电位输出。

若有其他问题或者更好的建议，可发邮件与我联系，个人邮箱：angelpoint@foxmail.com
本文档最后更新日期：2018.1.12_0:46 by Cooper Liu