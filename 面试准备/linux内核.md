##  **Linux内核的组成部分**

Linux内核主要由五个子系统组成：进程调度，内存管理，虚拟文件系统，网络接口，进程间通信。

## **Linux系统的组成部分**

Linux系统一般有4个主要部分：

内核、shell、文件系统和应用程序。

# linux 进程内核栈

## linux进程用户空间和内核空间

https://blog.csdn.net/u014426028/article/details/106770216/

![986f37930954ecb6bb45c80220465ab5.png](https://img-blog.csdnimg.cn/img_convert/986f37930954ecb6bb45c80220465ab5.png)

![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9pbWFnZXMwLmNuYmxvZ3MuY29tL2Jsb2cvNTkzMjUzLzIwMTQwOS8yODE4NDIzMzE3MDc3ODkuanBn?x-oss-process=image/format,png)

# 字符设备驱动相关知识

https://blog.csdn.net/weixin_49298931/article/details/110345142

### 1、设备驱动分类

linux系统将设备分为3类：字符设备、块设备、网络设备。
字符设备：提供连续的数据流，应用程序可以顺序读取，通常不支持随机存取。读取数据需要按照先后数据。字符设备是面向流的设备，常见的字符设备有鼠标、键盘、串口、控制台和LED设备等。
块设备：是指可以从设备的任意位置读取一定长度数据的设备。块设备包括硬盘、磁盘、U盘和SD卡等。
每一个字符设备或块设备都在/dev目录下对应一个设备文件。linux用户程序通过设备文件（或称设备节点）来使用驱动程序操作字符设备和块设备。
在Linux中，一般通过设备文件来访问字符设备。设备文件一般存放在/dev目录下，可使用 ls | grep tty命令查看所有的终端设备tty相关信息。

### 2、字符设备、字符设备驱动与用户空间访问该设备的程序三者之间的关系

在Linux内核中：

使用cdev结构体来描述字符设备;通过其成员dev_t来定义设备号（分为主、次设备号）以确定字符设备的唯一性;通过其成员file_operations来定义字符设备驱动提供给VFS的接口函数，如常见的open()、read()、write()等;

在Linux字符设备驱动中:
模块加载函数通过 register_chrdev_region( ) 或 alloc_chrdev_region( )来静态或者动态获取设备号;通过 cdev_init( ) 建立cdev与 file_operations之间的连接，通过 cdev_add( ) 向系统添加一个cdev以完成注册;模块卸载函数通过cdev_del( )来注销cdev，通过 unregister_chrdev_region( )来释放设备号;

用户空间访问该设备的程序:
通过Linux系统调用，如open( )、read( )、write( )，来“调用”file_operations来定义字符设备驱动提供给VFS的接口函数;

## 二、相关数据结构

### 1.字符设备

在Linux内核中，使用cdev结构体来描述一个字符设备，cdev结构体的定义如下：

```c
<include/linux/cdev.h>
 
struct cdev { 
	struct kobject kobj;                  //内嵌的内核对象.
	struct module *owner;                 //该字符设备所在的内核模块的对象指针.
	const struct file_operations *ops;    //该结构描述了字符设备所能实现的方法，是极为关键的一个结构体.
	struct list_head list;                //用来将已经向内核注册的所有字符设备形成链表.
	dev_t dev;                            //字符设备的设备号，由主设备号和次设备号构成.
	unsigned int count;                   //隶属于同一主设备号的次设备号的个数.
};
```

cdev结构是内核对字符设备驱动的标准描述。在实际的设备驱动开发中，通常使用自定义的结构体来描述一个特定的字符设备。这个自定义的结构体中必然会包含cdev结构，另外还要包含一些描述这个具体设备某些特性的字段。

### 2.设备号

一个字符设备或块设备都有一个主设备号和一个次设备号。主设备号用来标识与设备文件相连的驱动程序，用来反映设备类型。次设备号被驱动程序用来辨别操作的是哪个设备，用来区分同类型的设备。二者一起为设备号。

```c
//宏定义：
#define MINORBITS    20                                             //次设备号的位数
#define MINORMASK    ((1U << MINORBITS) - 1)                        //次设备号掩码
#define MAJOR(dev)    ((unsigned int) ((dev) >> MINORBITS))         //从设备号提取主设备号
#define MINOR(dev)    ((unsigned int) ((dev) & MINORMASK))          //从设备号提取次设备号
#define MKDEV(ma,mi)    (((ma) << MINORBITS) | (mi))                //通过主次设备号组合出设备号
```

## 内核地址空间划分

通常**32位linux内核虚拟**地址空间划分0~3G为用户空间，3~4G为内核空间(注意，内核可以使用的线性地址只有1G)。注意这里是32位内核地址空间划分，64位内核地址空间划分是不同的。

### **Linux内核高端内存的由来**

当内核模块代码或线程访问[内存](https://so.csdn.net/so/search?q=内存&spm=1001.2101.3001.7020)时，代码中的内存地址都为逻辑地址，而对应到真正的物理内存地址，需要地址**一对一**的映射，如逻辑地址0xc0000003对应的物理地址为0×3，0xc0000004对应的物理地址为0×4，… …，逻辑地址与物理地址对应的关系为

物理地址 = 逻辑地址 – 0xC0000000：这是内核地址空间的地址转换关系，注意内核的虚拟地址在“高端”，但是ta映射的物理内存地址在低端。

| **逻辑地址**   | **物理内存地址**  |
| -------------- | ----------------- |
| 0xc0000000     | 0×0               |
| 0xc0000001     | 0×1               |
| 0xc0000002     | 0×2               |
| 0xc0000003     | 0×3               |
| …              | …                 |
| 0xe0000000     | 0×20000000        |
| …              | …                 |
| **0xffffffff** | **0×40000000 ??** |

假 设按照上述简单的地址映射关系，那么内核逻辑地址空间访问为0xc0000000 ~ 0xffffffff，那么对应的物理内存范围就为0×0 ~ 0×40000000，即只能访问1G物理内存。若机器中安装8G物理内存，那么内核就只能访问前1G物理内存，后面7G物理内存将会无法访问，因为内核 的地址空间已经全部映射到物理内存地址范围0×0 ~ 0×40000000。即使安装了8G物理内存，那么物理地址为0×40000001的内存，内核该怎么去访问呢？代码中必须要有内存逻辑地址 的，0xc0000000 ~ 0xffffffff的地址空间已经被用完了，所以无法访问物理地址0×40000000以后的内存。

显 然不能将内核地址空间0xc0000000 ~ 0xfffffff全部用来简单的地址映射。因此x86[架构](http://lib.csdn.net/base/architecture)中将**内核地址空间**划分三部分：ZONE_DMA、ZONE_NORMAL和 ZONE_HIGHMEM。ZONE_HIGHMEM即为高端内存，这就是内存高端内存概念的由来。

在x86结构中，三种类型的区域（从3G开始计算）如下：

**ZONE_DMA**    内存开始的16MB

**ZONE_NORMAL**    16MB~896MB

**ZONE_HIGHMEM**    896MB ~ 结束（1G）

![img](http://ilinuxkernel.com/wp-content/uploads/2011/09/091011_1614_Linux3.png)

### **Linux内核高端内存的理解**

前 面我们解释了高端内存的由来。 Linux将内核地址空间划分为三部分ZONE_DMA、ZONE_NORMAL和ZONE_HIGHMEM，高端内存HIGH_MEM地址空间范围为 0xF8000000 ~ 0xFFFFFFFF（896MB～1024MB）。那么如内核是**如何借助128MB高端内存地址空间是如何实现访问可以所有物理内存**？

当内核想访问高于896MB物理地址内存时，从0xF8000000 ~ 0xFFFFFFFF地址空间范围内找一段相应大小空闲的逻辑地址空间，借用一会。借用这段逻辑地址空间，建立映射到想访问的那段物理内存（即填充内核PTE页面表），**临时用一会，用完后归还**。这样别人也可以借用这段地址空间访问其他物理内存，实现了使用有限的地址空间，访问所有所有物理内存。如下图。

![img](http://ilinuxkernel.com/wp-content/uploads/2011/09/091011_1614_Linux4.png)

例 如内核想访问2G开始的一段大小为1MB的物理内存，即物理地址范围为0×80000000 ~ 0x800FFFFF。访问之前先找到一段1MB大小的空闲地址空间，假设找到的空闲地址空间为0xF8700000 ~ 0xF87FFFFF，用这1MB的逻辑地址空间映射到物理地址空间0×80000000 ~ 0x800FFFFF的内存。映射关系如下：

| **逻辑地址** | **物理内存地址** |
| ------------ | ---------------- |
| 0xF8700000   | 0×80000000       |
| 0xF8700001   | 0×80000001       |
| 0xF8700002   | 0×80000002       |
| …            | …                |
| 0xF87FFFFF   | 0x800FFFFF       |

当内核访问完0×80000000 ~ 0x800FFFFF物理内存后，就将0xF8700000 ~ 0xF87FFFFF内核线性空间释放。这样其他进程或代码也可以使用0xF8700000 ~ 0xF87FFFFF这段地址访问其他物理内存。

从上面的描述，我们可以知道***\*高端内存的最基本思想\****：借一段地址空间，建立临时地址映射，用完后释放，达到这段地址空间可以循环使用，访问所有物理内存。

看到这里，不禁有人会问：万一有内核进程或模块一直占用某段逻辑地址空间不释放，怎么办？若真的出现的这种情况，则内核的高端内存地址空间越来越紧张，若都被占用不释放，则没有建立映射到物理内存都无法访问了。

### **Linux内核高端内存的划分**

内核将高端内存划分为3部分：VMALLOC_START~VMALLOC_END、KMAP_BASE~FIXADDR_START和FIXADDR_START~4G。

![img](http://ilinuxkernel.com/wp-content/uploads/2011/09/091011_1614_Linux5.png)

对 于高端内存，可以通过 alloc_page() 或者其它函数获得对应的 page，但是要想访问实际物理内存，还得把 page 转为线性地址才行（为什么？想想 MMU 是如何访问物理内存的），也就是说，我们需要为高端内存对应的 page 找一个线性空间，这个过程称为高端内存映射。

对应高端内存的3部分，高端内存映射有三种方式：
**映射到”内核动态映射空间”（noncontiguous memory allocation）**
这种方式很简单，因为通过 vmalloc() ，在”内核动态映射空间”申请内存的时候，就可能从高端内存获得页面（参看 vmalloc 的实现），因此说高端内存有可能映射到”内核动态映射空间”中。

**持久内核映射（permanent kernel mapping）**
如果是通过 alloc_page() 获得了高端内存对应的 page，如何给它找个线性空间？
内核专门为此留出一块线性空间，从 PKMAP_BASE 到 FIXADDR_START ，用于映射高端内存。在 2.6内核上，这个地址范围是 4G-8M 到 4G-4M 之间。这个空间起叫”内核永久映射空间”或者”永久内核映射空间”。这个空间和其它空间使用同样的页目录表，对于内核来说，就是 swapper_pg_dir，对普通进程来说，通过 CR3 寄存器指向。通常情况下，这个空间是 4M 大小，因此仅仅需要一个页表即可，内核通过来 pkmap_page_table 寻找这个页表。通过 kmap()，可以把一个 page 映射到这个空间来。由于这个空间是 4M 大小，最多能同时映射 1024 个 page。因此，对于不使用的的 page，及应该时从这个空间释放掉（也就是解除映射关系），通过 kunmap() ，可以把一个 page 对应的线性地址从这个空间释放出来。

**临时映射（temporary kernel mapping）**
内核在 FIXADDR_START 到 FIXADDR_TOP 之间保留了一些线性空间用于特殊需求。这个空间称为”固定映射空间”在这个空间中，有一部分用于高端内存的临时映射。

这块空间具有如下特点：
（1）每个 CPU 占用一块空间
（2）在每个 CPU 占用的那块空间中，又分为多个小空间，每个小空间大小是 1 个 page，每个小空间用于一个目的，这些目的定义在 kmap_types.h 中的 km_type 中。

当要进行一次临时映射的时候，需要指定映射的目的，根据映射目的，可以找到对应的小空间，然后把这个空间的地址作为映射地址。这意味着一次临时映射会导致以前的映射被覆盖。通过 kmap_atomic() 可实现临时映射。

## 内核空间三个区域

ZONE_DMA的范围是0~16M，该区域的物理页面专门供I/O设备的DMA使用。之所以需要单独管理DMA的物理页面，是因为DMA使用物理地址访问内存，不经过MMU，并且需要连续的缓冲区，所以为了能够提供物理上连续的缓冲区，必须从物理地址空间专门划分一段区域用于DMA。

ZONE_NORMAL的范围是16M~896M，该区域的物理页面是内核能够直接使用的。

ZONE_HIGHMEM的范围是896M~结束，该区域即为高端内存，内核不能直接使用。

![wKiom1Nm_tezL0_TAACljE7bXGU278.jpg](http://s3.51cto.com/wyfs02/M02/25/CF/wKiom1Nm_tezL0_TAACljE7bXGU278.jpg)

在kernel image下面有16M的内核空间用于DMA操作。位于内核空间高端的128M地址主要由3部分组成，分别为vmalloc area，持久化内核映射区，临时内核映射区。

　　由于ZONE_NORMAL和内核线性空间存在直接映射关系，所以内核会将频繁使用的数据如kernel代码、GDT、IDT、PGD、mem_map数组等放在ZONE_NORMAL里。而将用户数据、页表(PT)等不常用数据放在ZONE_ HIGHMEM里，只在要访问这些数据时才建立映射关系(kmap())。比如，当内核要访问I/O设备存储空间时，就使用ioremap()将位于物理地址高端的mmio区内存映射到内核空间的vmalloc area中，在使用完之后便断开映射关系。

## 进程上下文和中断上下文

**上下文context： 上下文简单说来就是一个环境。**

　　用户空间的应用程序，通过系统调用，进入内核空间。这个时候用户空间的进程要传递 很多变量、参数的值给内核，内核态运行的时候也要保存用户进程的一些寄存 器值、变量等。**所谓的“进程上下文”，可以看作是用户进程传递给内核的这些参数以及内核要保存的那一整套的变量和寄存器值和当时的环境等。**

　　相对于进程而言，就是进程执行时的环境。具体来说就是各个变量和数据，包括所有的寄存器变量、进程打开的文件、内存信息等。一个进程的上下文可以分为三个部分:用户级上下文、寄存器上下文以及系统级上下文。

（1）用户级上下文: 正文、数据、用户堆栈以及共享存储区；
（2）寄存器上下文: 通用寄存器、程序寄存器(IP)、处理器状态寄存器(EFLAGS)、栈指针(ESP)；
（3）系统级上下文: 进程控制块task_struct、内存管理信息(mm_struct、vm_area_struct、pgd、pte)、内核栈。

  **当发生进程调度时，进行进程切换就是上下文切换(context switch).操作系统必须对上面提到的全部信息进行切换，新调度的进程才能运行。而系统调用进行的模式切换(mode switch)。模式切换与进程切换比较起来，容易很多，而且节省时间，因为模式切换最主要的任务只是切换进程寄存器上下文的切换。**

　　硬件通过触发信号，导致内核调用中断处理程序，进入内核空间。这个过程中，硬件的 一些变量和参数也要传递给内核，内核通过这些参数进行中断处理。**所谓的“ 中断上下文”，其实也可以看作就是硬件传递过来的这些参数和内核需要保存的一些其他环境（主要是当前被打断执行的进程环境）。**中断时，内核不代表任何进程运行，它一般只访问系统空间，而不会访问进程空间，内核在中断上下文中执行时一般不会阻塞。

## 进程创建过程

https://blog.csdn.net/xingjiarong/article/details/50920207

对于一个进程来说，PCB就好像是他的记账先生，当一个进程被创建时PCB就被分配，然后有关进程的所有信息就全都存储在PCB中，例如，打开的文件，[页表](https://so.csdn.net/so/search?q=页表&spm=1001.2101.3001.7020)基址寄存器，进程号等等。在linux中PCB是用结构task_struct来表示的，我们首先来看一下task_struct的组成。

get_free_page()用来分配存储PCB的空间，这个PCB存储在堆栈的最底层

# 主要结构源码

## 内存节点

- NUMA系统的内存节点，根据处理器和内存的距离划分； 在具有不连续内存的UMA系统中，表示比区域的级别理高的内存区域，根据物理地址是否连续划分，每块物理地址连续的内存是一个内存节点。内存节点使用一个pglist_data结构体数据类型描述内存布局。

![img](https://img-blog.csdnimg.cn/img_convert/ec89a3886fec80c66b8d97f7799e7394.png)

- 成员node_mem_map指向页描述符数组，每个物理页对应一个页描述符。node_mem_map可能不是指向数组的第一个元素，因为页描述符数组的大小必须对齐到2的(MAX_ORDER-1)次方。(MAX_ORDER-1)是页分配器可分配的最大阶数。具体pglist_ddata对应内核源码分析如下：

https://blog.csdn.net/youzhangjing_/article/details/124898650

### 系统中的每个物理页面用struct page数据结构对象来表示（内核空间mem_map的结构体）

mem_map管理着系统中所有的物理内存页面。

```c
struct page {
        unsigned long flags;         
        atomic_t _count;
        union {
                atomic_t _mapcount;
                unsigned int inuse;
        };
        union {
            struct {
                unsigned long private;
                struct address_space *mapping;
            };
            struct kmem_cache *slab;    /* SLUB: Pointer to slab */
            struct page *first_page;    /* Compound tail pages */
        };
        union {
                pgoff_t index;          /* Our offset within mapping. */
                void *freelist;         /* SLUB: freelist req. slab lock */
        };
        struct list_head lru;       

#if defined(WANT_PAGE_VIRTUAL) 
        void *virtual;
#endif         
};
```

### slab分配器

一组用来存储同一种对象（如task_struct，mm_struct等结构体）的slab叫cache

https://zhuanlan.zhihu.com/p/359247565

```c
struct slab {
	struct list_head list;        //链表结构
	unsigned long colouroff;        //对象区的起点与slab起点之间的偏移
	void *s_mem;		/* 对象区在slab中的起点 */
	unsigned int inuse;	/* 记录已分配对象空间数目的计数器 */
	kmem_bufctl_t free;        //指向了对象链中的第一个空闲对象
	unsigned short nodeid;
};
```

