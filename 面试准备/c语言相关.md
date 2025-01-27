## strcopy安全性

### strncpy()函数
strncpy（）函数类似于strcpy（）函数，不同之处在于最多复制了src个n字节。如果src的前n个字符中没有NULL字符，则放置在dest中的字符串将不会以NULL终止。如果src的长度小于n，则strncpy（）将另外的NULL字符写入dest以确保总共写入了n个字符。

但	如果src的前n个字符中没有空字符，则放置在dest中的字符串将不会以空字符结尾。因此，strncpy（）不保证目标字符串将以NULL终止。终止的strlen（）字符串可能导致段错误。换句话说，C / C ++中的非终止字符串只是一个定时炸弹，正等待破坏代码。 

### snprintf()、strlcpy()函数

这两个函数都保证目标字符串将以NULL终止。类似地，snprintf（）函数，strlcpy函数最多将dest_size-1个字符（dest_size是目标字符串缓冲区的大小）从src复制到dst，并在必要时截断src。结果始终为空终止。该函数返回strlen（src）。缓冲区溢出可以按以下方式检查：

如果（strlcpy（dst，src，dstsize）> = dest_size），返回-1

## 标准IO和文件IO的区别

标准ＩＯ：标准I/O是ANSI C建立的一个标准I/O模型，是一个标准函数包和stdio.h头文件中的定义，具有一定的可移植性。标准IO库处理很多细节。例如缓存分配，以优化长度执行IO等。标准的IO提供了三种类型的缓存。

（1）全缓存：当填满标准IO缓存后才进行实际的IO操作。
（2）行缓存：当输入或输出中遇到新行符时，标准IO库执行IO操作。
（3）不带缓存：stderr就是了。

文件ＩＯ：文件ＩＯ称之为不带缓存的IO（unbuffered I/O)。不带缓存指的是每个read，write都调用内核中的一个系统调用。也就是一般所说的低级I/O——操作系统提供的基本IO服务，与os绑定，特定于linix或unix平台。

### 区别

首先：两者一个显著的不同点在于，**标准I/O默认采用了缓冲机制**，比如调用fopen函数，不仅打开一个文件，而且建立了一个缓冲区（读写模式下将建立两个缓冲区，用户空间），还创建了一个包含文件和缓冲区相关数据的数据结构。**低级I/O**一般没有采用缓冲，需要自己创建缓冲区，不过其实在linix或unix系统中，都是有使用称**为内核缓冲**的技术用于提高效率，读写调用是在内核缓冲区和进程缓冲区之间进行的数据复制。

其次从操作的设备上来区分，文件I/O主要针对文件操作，读写硬盘等，它操作的是文件描述符（FILE * 文件指针并不是指向实际的文件，而是一个关于文件信息的数据包，其中包括文件使用的缓冲区信息，fopen失败返回NULL），标准I/O针对的是控制台，打印输出到屏幕等，它操作的是字符流（open失败返回-1）。对于不同设备得特性不一样，必须有不同api访问才最高效。

来看下他们使用的函数

|      | 标准ＩＯ                              | 文件ＩＯ(低级IO) |
| ---- | ------------------------------------- | ---------------- |
| 打开 | fopen,freopen,fdopen                  | open             |
| 关闭 | fclose                                | close            |
| 读   | getc,fgetc,getchar fgets,gets fread   | read             |
| 写   | putc,fputc,putchar fputs,puts, fwrite | write            |

https://wenku.baidu.com/view/ce12cff28762caaedc33d40a.html

## new和malloc

做嵌入式，对于内存是十分在意的，因为可用内存有限，所以嵌入式笔试面试题目，内存的题目高频。 

1. malloc和free是[c++]()/c语言的库函数，需要头文件支持stdlib.h；new和delete是C++的关键字，不需要头文件，需要编译器支持； 

2. 使用new操作符申请内存分配时，无需指定内存块的大小，编译器会根据类型信息自行计算。而malloc则需要显式地支持所需内存的大小。 

3. new操作符内存分配成功时，返回的是对象类型的指针，类型严格与对象匹配，无需进行类型转换，故new是符合类型安全性的操作符。而malloc内存分配成功则是返回void*，需要通过强制类型转换将void*指针转换成我们需要的类型。 

4. new内存分配失败时，会抛出bad_alloc异常。malloc分配内存失败时返回NULL。
5. malloc分配完内存后，最好使用memset初始化，不然里面存的是野值。memset函数按字节对内存块进行初始化，所以不能用它将int数组初始化为0和-1之外的其他值（除非该值高字节和低字节相同）。无论你用哪个值进行初始化，都只会取该值的低1字节去初始化每个字节。

## 在1G内存的计算机中能否malloc(1.2G)？为什么？

答：是有可能申请1.2G的内存的。 

解析：回答这个问题前需要知道malloc的作用和原理，应用程序通过malloc函数可以向程序的虚拟空间申请一块虚拟地址空间，与物理内存没有直接关系，得到的是在虚拟地址空间中的地址，之后程序运行所提供的物理内存是由操作系统完成的。

## **extern”C” 的作用**

我们可以在C++中使用C的已编译好的函数模块，这时候就需要用到extern”C”。也就是extern“C” 都是在[c++]()文件里添加的。 

 extern在链接阶段起作用（四大阶段：预处理--编译--汇编--链接）。链接阶段负责处理各个文件内对其他文件里函数或变量的调用

## **strcat、strncat、strcmp、strcpy哪些函数会导致内存溢出？如何改进？**

strcpy函数会导致内存溢出。 

 strcpy拷贝函数不安全，他不做任何的检查措施，也不判断拷贝大小，不判断目的地址内存是否够用。 

```c
 char *strcpy（char *strDest，const char *strSrc）
```

 strncpy拷贝函数，虽然计算了复制的大小，但是也不安全，没有检查目标的边界。 

```c
 strncpy(dest, src, sizeof(dest)); 
```

 strncpy_s是安全的 

 strcmp(str1,str2)，是比较函数，若str1=str2，则返回零；若str1<str2，则返回负数；若str1>str2，则返回正数。（比较字符串） 

 strncat()主要功能是在字符串的结尾追加n个字符。 

```c
 char * strncat(char *dest, const char *src, size_t n);
```

 strcat()函数主要用来将两个char类型连接。例如： 

```c
 char d[20]="Golden";  char s[20]="View";  strcat(d,s);  //打印d  printf("%s",d);
```

 输出 d 为 GoldenView （中间无空格） 

 延伸： 

 memcpy拷贝函数，它与strcpy的区别就是memcpy可以拷贝任意类型的数据，strcpy只能拷贝字符串类型。 

 memcpy 函数用于把资源内存（src所指向的内存区域）拷贝到目标内存（dest所指向的内存区域）；有一个size变量控制拷贝的字节数；

## **static的用法（定义和用途）**（必考）

1. 用static修饰局部变量：使其变为静态存储方式(静态数据区)，那么这个局部变量在函数执行完成之后不会被释放，而是继续保留在内存中。 
2. 用static修饰全局变量：使其只在本文件内部有效，而其他文件不可连接或引用该变量。 
3. 用static修饰函数：对函数的连接方式产生影响，使得函数只在本文件内部有效，对其他文件是不可见的（这一点在大工程中很重要很重要，避免很多麻烦，很常见）。这样的函数又叫作静态函数。使用静态函数的好处是，不用担心与其他文件的同名函数产生干扰，另外也是对函数本身的一种保护机制。

## const的用法（定义和用途）（必考）

const主要用来修饰变量、函数形参和类成员函数： 

1. 用const修饰常量：定义时就初始化，以后不能更改。 

2. 用const修饰形参：func(const int a){};该形参在函数里不能改变 

3. 用const修饰类成员函数：该函数对成员变量只能进行只读操作，就是const类成员函数是不能修改成员变量的数值的。 

 被const修饰的东西都受到强制保护，可以预防意外的变动，能提高程序的健壮性。 

 参考一个大佬的回答: 

 我只要一听到被面试者说："const意味着常数"，我就知道我正在和一个业余者打交道。去年Dan Saks已经在他的文章里完全概括了const的所有用法，因此ESP(译者：Embedded Systems Programming)的每一位读者应该非常熟悉const能做什么和不能做什么.如果你从没有读到那篇文章，只要能说出const意味着"只读"就可以了。尽管这个答案不是完全的答案，但我接受它作为一个正确的答案。如果应试者能正确回答这个问题，我将问他一个附加的问题：下面的声明都是什么意思？ 

```c
 const int a;  int const a;  const int *a;  int * const a;  int const * a const;
```

 前两个的作用是一样，a是一个常整型数。 

 第三个意味着a是一个指向常整型数的指针（也就是，整型数是不可修改的，但指针可以）。 

 第四个意思a是一个指向整型数的常指针（也就是说，指针指向的整型数是可以修改的，但指针是不可修改的）。 

 最后一个意味着a是一个指向常整型数的常指针（也就是说，指针指向的整型数是不可修改的，同时指针也是不可修改的）。

## **volatile作用和用法**

一个定义为volatile的变量是说这变量可能会被意想不到地改变，这样，编译器就不会去假设这个变量的值了。精确地说就是，优化器在用到这个变量时必须每次都小心地重新读取这个变量在内存中的值，而不是使用保存在寄存器里的备份（虽然读写寄存器比读写内存快）。 

 回答不出这个问题的人是不会被雇佣的。这是区分C程序员和嵌入式系统程序员的最基本的问题。搞嵌入式的家伙们经常同硬件、中断、RTOS等等打交道，所有这些都要求用到volatile变量。不懂得volatile的内容将会带来灾难。 

 以下几种情况都会用到volatile： 

 1、并行设备的硬件寄存器（如：状态寄存器） 

 2、一个中断服务子程序中会访问到的非自动变量 

 3、多线程应用中被几个任务共享的变量

## **const常量和#define的区别（编译阶段、安全性、内存占用等）**

用#define max 100 ; 定义的常量是没有类型的（不进行类型安全检查，可能会产生意想不到的错误），所给出的是一个立即数，编译器只是把所定义的常量值与所定义的常量的名字联系起来，define所定义的宏变量在预处理阶段的时候进行替换，在程序中使用到该常量的地方都要进行**拷贝替换**； 

 用const int max = 255 ; 定义的常量有类型（编译时会进行类型检查）名字，存放在内存的静态区域中，在编译时确定其值。在程序运行过程中const变量只有一个拷贝，而#define所定义的宏变量却有多个拷贝，所以宏定义在程序运行过程中所消耗的内存要比const变量的大得多

## **变量的作用域（全局变量和局部变量）**

全局变量：在所有函数体的外部定义的，程序的所在部分（甚至其它文件中的代码）都可以使用。全局变量不受作用域的影响（也就是说，全局变量的生命期一直到程序的结束）。 

 局部变量：出现在一个作用域内，它们是局限于一个函数的。局部变量经常被称为自动变量，因为它们在进入作用域时自动生成，离开作用域时自动消失。关键字auto可以显式地说明这个问题，但是局部变量默认为auto，所以没有必要声明为auto。 

 局部变量可以和全局变量重名，在局部变量作用域范围内，全局变量失效，采用的是局部变量的值。

## **sizeof 与strlen （字符串，数组）**

1. 如果是数组 

```c
 #include<stdio.h>  
int main()  
{      
    int a[5]={1,2,3,4,5};      
    printf(“sizeof  数组名=%d\n”,sizeof(a));      
    printf(“sizeof *数组名=%d\n”,sizeof(*a));  
}
```

 运行结果 

```c
 sizeof  数组名=20  sizeof *数组名=4
```

2. 如果是指针，sizeof只会检测到是指针的类型，指针都是占用4个字节的空间（32位机）。 

 sizeof是什么？是一个操作符,也是关键字，就不是一个函数，这和strlen()不同，strlen()是一个函数。 

 那么sizeof的作用是什么？返回一个对象或者类型所占的内存字节数。我们会对sizeof()中的数据或者指针做运算吗？基本不会。例如sizeof(1+2.0),直接检测到其中类型是double,即是sizeof(double) = 8。如果是指针，sizeof只会检测到是指针的类型，指针都是占用4个字节的空间（32位机）。 

```c
char *p = "sadasdasd";
sizeof(p):4
sizeof(*p):1//指向一个char类型的
```

 除非使用strlen()，仅对字符串有效，直到'\0'为止了，计数结果不包括\0。 

 要是非要使用sizeof来得到指向内容的大小，就得使用数组名才行， 如 

```c
char a[10];
sizeof(a):10  //检测到a是一个数组的类型。
```

 关于strlen()，它是一个函数，考察的比较简单： 

```c
strlen(“\n\t\tag\AAtang”);
```

 答案：11

## **经典的sizeof(struct)和sizeof(union)内存对齐**

内存对齐作用： 

1. 平台原因(移植原因)：不是所有的硬件平台都能访问任意地址上的任意数据的；某些硬件平台只能在某些地址处取某些特定类型的数据，否则抛出硬件异常。 
2. 性能原因：数据结构(尤其是栈)应该尽可能地在自然边界上对齐。原因在于，为了访问未对齐的内存，处理器需要作两次内存访问；而对齐的内存访问仅需要一次访问。读取内存总是一页页的读取，这也是为何链接部分需要对数据段进行合并

## **inline函数**

在C语言中，如果一些函数被频繁调用，不断地有函数入栈，即函数栈，会造成栈空间或栈内存的大量消耗。为了解决这个问题，特别的引入了inline修饰符，表示为内联函数。 

 大多数的机器上，调用函数都要做很多工作：调用前要先保存寄存器，并在返回时恢复，复制实参，程序还必须转向一个新位置执行C++中支持内联函数，其目的是为了提高函数的执行效率，用关键字 inline 放在函数定义(注意是定义而非声明)的前面即可将函数指定为内联函数，内联函数通常就是将它在程序中的每个调用点上“内联地”展开。 

 内联是以代码膨胀（复制）为代价，仅仅省去了函数调用的开销，从而提高函数的执行效率。

## **内存四区，什么变量分别存储在什么区域，堆上还是栈上。**

文字常量区，叫.rodata，不可以改变，改变会导致段错误 

```c
int a0=1;
static int a1;
const static a2=0;
extern int a3;

void fun(void)
{
 int a4;
 volatile int a5;
 return;
}
```

 a0 ：全局初始化变量；生命周期为整个程序运行期间；作用域为所有文件；存储位置为data段。 

 a1 ：全局静态未初始化变量；生命周期为整个程序运行期间；作用域为当前文件；储存位置为BSS段。 

 a2 ：全局静态变量 

 a3 ：全局初始化变量；其他同a0。 

 a4 ：局部变量；生命周期为fun函数运行期间；作用域为fun函数内部；储存位置为栈。 

 a5 ：局部易变变量；

## **使用32位编译情况下，给出判断所使用机器大小端的方法。**

联合体方法判断方法：利用union结构体的从低地址开始存，且同一时间内只有一个成员占有内存的特性。大端储存符合阅读习惯。联合体占用内存是最大的那个，和结构体不一样。 

 a和c公用同一片内存区域，所以更改c，必然会影响a的数据 

```c
#include<stdio.h>

int main(){
  union w
  {
      int a;
      char b;
  }c;
  c.a = 1;
  if(c.b == 1)
   printf("小端存储\n");
  else
   printf("大端存储\n");
 return 0;
}
```

 指针方法 

 通过将int强制类型转换成char单字节，p指向a的起始字节（低字节） 

```c
#include <stdio.h>
int main ()
{
    int a = 1;
    char *p = (char *)&a;
    if(*p == 1)
    {
        printf("小端存储\n");
    }
    else
    {
        printf("大端存储\n");
    }
    return 0;
}
```

## **用变量a给出下面的定义**

```
a) 一个整型数； int a
b）一个指向整型数的指针； int *a;
c）一个指向指针的指针，它指向的指针是指向一个整型数；int ** a;
d）一个有10个整型的数组； int a[10];
e）一个有10个指针的数组，该指针是指向一个整型数；int * a[10];
f）一个指向有10个整型数数组的指针； int a[10],*p=a;
g）一个指向函数的指针，该函数有一个整型参数并返回一个整型数； int (*a)(int)
h）一个有10个指针的数组，该指针指向一个函数，该函数有一个整型参数并返回一个整型数
int(*a[10])(int)
答案：
a)int a
b)int *a;
c)int **a;
d)int a[10];
e)int *a [10];
f) int a[10], *p=a;
g)int (*a)(int)
h) int( *a[10])(int)
```

## **与或非，异或。运算符优先级**

sum=a&b<<c+a^c;

## 写一个“标准”宏MIN

```c
#define MIN(A,B) ((A)<=(B)?(A):(B))
```

## 不支持重载！！！

