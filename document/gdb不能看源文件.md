### gdb不能看源文件

我们用gdb调试的时候，可能遇到**gdb无法打断点**、**gdb不能看源文件**、**gdb不能单步调试**等诸如此类问题，非常让人头疼...

这种问题可能有很**多种原因**，今天就几种常见的情况来做个总结，后面遇到了再补充，如果您发现了其他场景，欢迎评论区补充。

#### 一、打断点方式

先来说一下给程序打断点的方式：

1. **b test.cpp:127** //在test.cpp的127行打断点
2. **b Test::func(int, int*)** //在Test::func处打断点（由于C++支持重载，所以必须指定参数列表类型）
3. **b Test<int>::func(int, int*)** //如果是模板类，必须指定模板类型
4. **b NameSpace::Test::func(int, int*)** //如果有自定义的namespace,必须手动指定

> 提示：以上指令在输入过程中都可以按TAB自动补全。

#### 二、错误提示

遇到问题时，可能出现如下此类的错误提示：

```
Reading symbols from /home/XX/XX...(no debugging symbols found)...done.
```

```
No source file named file.c. 
Make breakpoint pending on future shared library load? (y or [n]) n 
```

```
gdb single stepping until exit from function *, which has no line number information
```

#### 三、错误原因及解决办法

##### 3.1 没有加-g

这是最常见的情况，八成是这个原因。

如果你是用的gcc/g++命令直接编译的，直接加-g选项即可，像这样：

 gcc **-g** test.c -o test

如果你用的是Makefile，那你需要去找里面的编译选项，一般是CFLAGS/CXXFLAGS这种名字，在里面加上-g，像这样：

CXX_FLAGS =  **-g** -fPIC -finline-functions -Wall -Winline -pipe

如果你用的是cmake，那你需要去CMakelists.txt里找对应的编译选项变量，像这样：

SET(CMAKE_CXX_FLAGS "**-g** -fPIC -finline-functions -Wall -Winline -pipe")

SET(CMAKE_CXX_FLAGS_DEBUG "**-g** -fPIC -finline-functions -Wall -Winline -pipe")

##### 3.2 执行环境上没有源文件

如果你的程序是在**一台机器上编译，另一台机器上执行**，那可能会出现这种问题，因为执行文件中并不含源代码，只有源代码的路径信息，所以必须要在执行环境，即调试环境上对应路径下有源代码才可以。如果有权限，直接把源代码拷贝过来就可以（注意放的路径一定要跟编译环境一样）。如果没权限，那就没招儿了，巧妇难为无米之炊。

这种情况还适用于各种**系统库**或者**三方库**，如果你要调试的部分不是你写的，你看不到源代码，那肯定是没办法调试的，就别费劲了。

##### 3.3 gcc gdb版本不匹配

这个是看到别人博客写的，自己没遇到过，写在这里以备查询。

##### 3.4 -fvisibility=hidden以及-Wl,-Bsymbolic

添加上面的编译选项可能导致问题，也是别人博客看到的。

##### 3.5 多个库相互链接导致的问题

这种问题一般是出现在工程中，项目结构比较复杂的时候。也就是你最后使用的库可能不是用-g编译出来的库，什么情况会出现这种问题呢？如果你编译了一个A.a 然后 B.a链接了A.a 最后A.a 跟B.a 又都被C.a链接...这个时候就会到导致有好多个A.a的版本。如果你之前编译的是Release版本的A.a，后面想调试，改编了Debug版本，但这个时候B.a C.a中的版本并没有更新。而链接顺序不合理又导致最后真正用的不是A.a中的版本，所以就出现了非常奇怪的现象：明明编译的是debug版本，最后gdb却没办法使用。

这种情况比较难搞，需要根据具体的情况排查，常见的辅助定位的指令有：

nm + grep 查看一个库里的符号，比如 nm -A C.a | grep xxx

ar -x 拆分一个库，比如 ar -x libxxx.a

最后如果实在分析不出来，就把所有库全部都重新编译一遍debug版本的，全部替换就可以了。







