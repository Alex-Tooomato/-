# Makefile项目管理

命名：makefile Makefile

## 基本原则：

1. 若想生成目标，检查规则中的依赖条件是否存在，如不存在，则寻找（make命令执行）是否有规则用来生成该依赖文件。
2. 检查规则中的目标是否需要更新，必须先检查它的所有依赖，依赖中有任一个被更新，则目标必须更新。

## 1 个规则：

目标：依赖条件

（一个tab缩进）命令

1. 目标的时间必须晚于依赖条件的时间，否则，更新目录。
2. 依赖条件如果不存在，找寻新的规则去产生依赖。

ALL:指定终极目标

```makefile
touch makefile
vi makefile

hello:hello.c
	gcc hello.c -o hello

或者
hello:hello.o
	gcc hello.o -o hello
hello.o:hello.c
	gcc -c hello.c -o hello.o

#若由多个函数生成
a.out:hello.c add.c sub.c div1.c
	gcc hello.c add.c sub.c div1.c -o a.out
```

上面由多个函数生成的程序中，若add.c改变了，则仍需将sub.c div1.c重新编译。

因此分开，只要一次链接就可以，如下所示：

```makefile
# 第一个目标为最终目标
a.out:hello.o add.o sub.o div1.o
	gcc hello.o add.o sub.o div1.o -o a.out
hello.o:hell.c
	gcc -c hello.c -o hello.o
add.o:add.c
	gcc -c add.c -o add.o
sub.o:sub.c
	gcc -c sub.c -o sub.o
div1.o:div1.c
	gcc -c div1.c -o div1.o
```

此时若修改了div1.c，重新make后只执行

```makefile
gcc -c div1.c -o div1.o
gcc hello.o add.o sub.o div1.o -o a.out
```

修改后文件的atime ctime会改变，以此识别是否进行了修改。修改了div1.c后，div1.c比div1.o时间晚，则div1.o需要被更新

makefile中默认遇到的第一个目标为最终目标，若想修改则加入

```bash
ALL:a.outk
```

## 2. 两个函数

```makefile
src=$(wildcard *.c)
#找到当前目录下所有后缀为.c的文件，赋值给src
obj=$(patsubst %.c,%.o, $(src))
#把src变量里所有后缀为.c的文件替换成.o，将参数3中，包含参数1的部分，替换为参数2  
```

则上方的makefile可以修改为

```makefile
src = $(wildcard *.c)  # add.c sub.c div1.c hello.c 
obj = $(patsubst %.c, %.o, $(src)) # add.o sub.o div1.o hello.o
ALL:a.out
a.out:$(obj)
	gcc $(obj) -o a.out
hello.o:hell.c
	gcc -c hello.c -o hello.o
add.o:add.c
	gcc -c add.c -o add.o
sub.o:sub.c
	gcc -c sub.c -o sub.o
div1.o:div1.c
	gcc -c div1.c -o div1.o

clean:
	-rm -rf $(obj) a.out
# rm前的- 表示出错依然执行
```

加了clean后可以通过make clean删除需要的东西。

make clean -n 可以删除需要的东西，不会真的执行

## 3. 三个自动变量

$@: 在规则的命令中，表示规则中的目标

$^：在规则的命令中，表示所有依赖条件

$<：在规则的命令中，表示第一个依赖条件。如果将该变量应用在模式规则中，它可将依赖条件列表中的依赖依次取出，套用模式规则

```makefile
src = $(wildcard *.c)  # add.c sub.c div1.c hello.c 
obj = $(patsubst %.c, %.o, $(src)) # add.o sub.o div1.o hello.o
ALL:a.out
a.out:$(obj)
	gcc $^ -o $@
hello.o:hell.c
	gcc -c $< -o $@
add.o:add.c
	gcc -c $< -o $@
sub.o:sub.c
	gcc -c $< -o $@
div1.o:div1.c
	gcc -c $< -o $@

clean:
	-rm -rf $(obj) a.out
# rm前的- 表示出错依然执行
```

## 模式规则

%.o:%.c
	gcc $< -o $-@

```makefile
src = $(wildcard *.c)  # add.c sub.c div1.c hello.c 
obj = $(patsubst %.c, %.o, $(src)) # add.o sub.o div1.o hello.o
ALL:a.out
a.out:$(obj)
	gcc $^ -o $@
	
%.o:%.c
	gcc $< -o $-@
	
clean:
	-rm -rf $(obj) a.out
```

> 为了方便扩展，在函数中新增函数而不需要修改makefile

## 静态模式规则

\$(obj):%.o:%.c
	gcc \$< -o \$@

指定模式规则属于哪个obj

```makefile
src = $(wildcard *.c)  # add.c sub.c div1.c hello.c 
obj = $(patsubst %.c, %.o, $(src)) # add.o sub.o div1.o hello.o

myARGS= -Wall -g
ALL:a.out
a.out:$(obj)
	gcc $^ -o $@ $(myARGS)
	
$(obj):%.o:%.c
	gcc $< -o $-@ $(myARGS)
%.o:%.c
	gcc $< -o $-@ $(myARGS)
	
clean:
	-rm -rf $(obj) a.out
.PHONY:clean ALL
```

## 伪目标

.PHONY: clean ALL

每次执行都重新生成，哪怕依赖条件没有改变（有依赖条件会根据依赖条件的早晚选择执行，clean没有依赖条件，所以需要这条命令）

## 参数

-n：模拟执行make、make clean命令，实际不执行生效

-f：指定文件执行 make 命令

## 作业

把头文件放进inc文件夹，.o放进obj文件夹，重写makefile实现

```makefile
#普通命令
gcc -c ./src/add.c -o ./obj/add.o -I ./inc
gcc -c ./src/sub.c -o ./obj/sub.o -I ./inc
...
gcc ./obj/add.o ./obj/sub.o ... -o a.out
```

### makefile

```makefile
src = $(wildcard ./src/*.c)  # ./src/add.c ./src/sub.c ./src/div1.c ./src/hello.c 
obj = $(patsubst ./src/%.c, ./obj/%.o, $(src)) # add.o sub.o div1.o hello.o

myARGS= -Wall -g #包含编译阶段，-g gdbt

inc_path=./inc #头文件引入在预处理

ALL:a.out

a.out:$(obj)
	gcc $^ -o $@ $(myARGS) #最后链接阶段不用头文件
	
$(obj):./obj/%.o:./src/%.c
	gcc $< -o $-@ $(myARGS) -I $(inc_path)
	
%.o:%.c
	gcc $< -o $-@ $(myARGS) -I $(inc_path)
	
clean:
	-rm -rf $(obj) a.out
	
.PHONY:clean ALL
```

make -f m6 // -f指定makefile文件名

# 作业2

将当前目录下所有.c文件编译为可执行程序

```makefile
src = $(wildcard *.c)
target = $(patsubst %.c,%,$(src))

ALL:$(target)

%:%.c
	gcc $< -o $@
clean:
	-rm -rf $(target)
.PHONY:clean ALL
```

