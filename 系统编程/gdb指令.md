## 基础指令：

  -g：使用该参数编译可以执行文件，得到调试表。

  gdb ./a.out

  list： list 1 列出源码。根据源码指定 行号设置断点。

  b： b 20  在20行位置设置断点。

  run/r: 运行程序

  n/next: 下一条指令（会越过函数）

  s/step: 下一条指令（会进入函数）

  p/print：p i 查看变量的值。

  continue：继续执行断点后续指令。

  finish：结束当前函数调用。

  quit：退出gdb当前调试。

## 其他指令：

  run：使用run查找段错误出现位置。

  set args： 设置main函数命令行参数 （在 start、run 之前）

  run 字串1 字串2 ...: 设置main函数命令行参数

  info b: 查看断点信息表

  b 20 if i = 5：  设置条件断点。

  ptype：查看变量类型。

  bt：列出当前程序正存活着的栈帧。

  frame： 根据栈帧编号，切换栈帧。

  display：设置跟踪变量

  undisplay：取消设置跟踪变量。 使用跟踪变量的编号。