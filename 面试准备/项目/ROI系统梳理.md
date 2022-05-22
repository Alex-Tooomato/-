# 系统梳理

这是根据LTE-V2X系统整理的，有些部分和原版的ROI有一些区别，但是整体架构一样，不影响理解

## 一、main

首先从main开始说起。main()是程序的入口，在ROI中main()主要是用产生Qt界面

下面简单说一下该部分的实现：

### 注册自定义类型

```c++
qRegisterMetaType<std::vector<std::complex<float>>> ("std::vector<std::complex<float>>");
qRegisterMetaType<std::vector<std::complex<float>>> ("std::vector<std::string>");
qRegisterMetaType<std::string> ("std::string");
```

需要注册自定义类型的原因是：当一个signal被放到队列中（queued）时，它的参数(arguments)也会被一起一起放到队列中（queued起来），这就意味着参数在被传送到slot之前需要被拷贝、存储在队列中（queue）中；为了能够在队列中存储这些参数(argument)，Qt需要去construct、destruct、copy这些对象，而为了让Qt知道怎样去作这些事情，参数的类型需要使用qRegisterMetaType来注册（如错误提示中的说明）。更多关于Qt的connect后面会提到一些。

### 准备需要发送的文件

```c++
// 准备需要发射的文件
QFile file;
// 这里的资源文件统一放到UI文件夹下，否则会出错，这个需要之后再研究下，现在没有时间看了
file.copy(":/Resources/tx_data/Zeros1000_PSBCH_20frames_PSSCH", QString::fromStdString(Config::TX_FILES[Config::ROLES::ALICE]));//Alice端
file.copy(":/Resources/tx_data/Zeros1000_PSSCH", QString::fromStdString(Config::TX_FILES[Config::ROLES::BOB]));//Bob
file.copy(":/Resources/filter/111.bin", QString::fromStdString(Config::tmp_filter_matrix_filepath));//这个好像用不到
```

需要将要发送的文件存到Config全体参数里，如上图所示，界面里没有存放发送文件的地方（这个后面需要改一下，放到界面里）

### QT GUI

```c++
    /**
     * Qt Gui
     */
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    Config::set_config("roi.ini"); // 设置global变量
    Widget *w = get_global_widget();

    w->renderBySetting();
    w->show();

	//启动
 	qDebug() << "main thread Id: " << a.thread()->currentThreadId();

    int ret = a.exec();
    return ret;
```

#### 一、QApplication

生成界面的部分，首先创建一个`QApplication`对象。QApplication类是基于QWidget的管理[GUI](https://so.csdn.net/so/search?q=GUI&spm=1001.2101.3001.7020)程序的控制流和主要设置，用来完成处理QWidget特有的初始化和结束收尾工作。最后的`a.exec()`实现

#### 二、setCodecForCStrings()

接下来的`setCodecForCStrings()`设置界面的编码方式，使得界面可以显示中文。

#### 三、set_config()

`set_config()`设置全局的变量，也就是将Config.h里声明的变量设置为界面里的值。参数是一个文件，一开始没有文件的话会创建一个，后面会将改动保存在这个文件里，下一次启动时会自动加载上一次的配置。

可以进入到这个函数里简单看一下

```c++
QSettings setting(QString::fromStdString(Config::config_file_path), QSettings::IniFormat); // 选择配置文件，若无则创建一个
    setting.beginGroup("usrp"); // usrp 设备类 QString::fromStdString(usrp_ip)
    usrp_ip_index = setting.value("usrp_ip_index", 0).toInt();
    samp_rate = setting.value("samp_rate", 30720).toFloat(); // 单位k
    carrier_freq = setting.value("carrier_freq", 5200).toDouble(); // 单位兆
    tx_carrier_freq = setting.value("tx_carrier_freq", 5200).toDouble(); // 单位兆
    rx_carrier_freq = setting.value("rx_carrier_freq", 5200).toDouble(); // 单位兆
    gain = setting.value("gain", 30).toFloat();
    latency = setting.value("latency", 0).toInt(); // 单位ms
    setting.endGroup();
/****/
```

这里只举了一个usrp设备类参数的例子，如下是刘永建学长整理的关于roi.ini一些知识

roi.ini的生成需要满足两个条件：

```c++
//条件1
QSettings setting(QString::fromStdString(Config::config_file_path), QSettings::IniFormat);
//条件2
setting.setValue("usrp_ip_index", usrp_ip_index);

//注：单独构造而并不setValue，ini文件并没有创造
```

关于配置的读取流程这里做个简要说明：

> (1)main函数中调用Config::set_config()函数，这个函数有两个作用：
>
> - 第一次运行时：此时"roi.ini"文件还没有建立，Config文件的各值都是defaultValue，随后赋值给各控件显示(renderBySetting()函数)
> - 以后运行时：此时"roi.ini"文件已经建立(通过updateConfig函数的setValue)，Config文件的各值会先从"roi.ini"文件中读值，随后赋值给各控件显示(renderBySetting()函数);
>
> (2)启动运行按钮之后，进入on_run_push_button_clicked()函数，在此通过updateConfig函数从各控件读取相应的值并进行创建/更新roi.ini文件

#### 四、Widget

这个就是界面文件，整个界面的主体，下一节会比较详细的介绍Widget类这里先简单说明一下用到的两个函数

`renderBySetting()`如上所述，就是将Config的参数赋值给各个控件显示，比如下面设置角色的控件

```c++
ptr_role_combo_box = this->findChild<QComboBox*>("role_combo_box"); //设置 角色
ptr_role_combo_box->setCurrentIndex(Config::role_index);
```

`show()`函数显示界面，无需赘述

#### 五、return a.exec()

`return a.exec()`让界面处于一个循环等待事件的状态，接下来就等待接受用户和系统的消息并进行处理，里面就包含所谓的信号槽机制。当应用程序退出时比如调用`exit()`时，`exec()`函数的值就会返回。如下为一个等待处理命令，`my1.exec()`一直循环等待，当跳出来一个对话框(QDialog)，你选择了接受后(Accepted)，那么就进入if里面进行具体事件的处理。当然也可以设置run()函数让他自己执行，我们的系统就是如此。

```c++
if(my1.exec()==QDialog::Accepted)
{
    //xxxxxx具体事件的处理
}
```

## 二、Widget类

### 有关Qt的connect

信号和槽机制是Qt的重要基础，通过将信号与槽连接起来，可以实现不同操作之间的逻辑连接、参数传递和及时反馈等。

#### 信号

signal，即信号，作用是激发槽函数执行。信号可以有Qt的图形界面对象（如QLineEdit、QPushButton、QWidget等）发出，也可以由用户自定义的**继承自QObject类**里的方法通过**emit**关键字发出。

这里注意两点：

- 继承自QObject类：信号和槽机制本身是在QObject中实现的。QObject并不局限于上面举例说的图形用户界面编程中，可以由用户自定义的继承自QObject类。
- emit发射信号。除Qt自带的图形界面编程，用户自定义的信号通过emit+信号发射。信号也是一种函数，无返回类型（void），可以带参数。

#### 信号和槽连接

通过`connect(sender,SIGNAL(signal),receiver,SLOT(slot))`建立连接。sender和receiver是指向QObject的指针。

- 一个信号可以连接多个槽，发射该信号时，会以不确定的顺序逐个调用这些槽函数；
- 一个槽可以被多个信号连接；
- 两个信号之间可以连接。
- 建立的连接可以通过disconnect取消，但是用得很少。

更多信息可以参考以下连接

[参考链接1](https://blog.csdn.net/naibozhuan3744/article/details/79210861)

[参考链接2](https://blog.csdn.net/sinat_21107433/article/details/80961733)

源码分析：[参考链接3](https://blog.csdn.net/iEearth/article/details/74025072)

信号与槽的关键就是Qt的元对象系统，通过moc编译隐藏了具体的实现细节，这些内容可以在moc_xxx.cpp中查看。元对象就是对数据进行描述的一组结构。

moc里用一个静态变量存储类内的信号和槽函数：qt_meta_stringdata_xxx。qt_meta_stringdata_是固定部分，xxx为对应的类名。该变量实则为一个结构体：qt_meta_stringdata_xxx_t。

该结构体有两个数组成员，第一个存储了类内的信号和槽，顺序为元素排列顺序为当前类、第一个信号、占位符、其它信号、其它槽，信号在槽前面，信号和槽各自的顺序以声明的顺序排列，至少有1个信号或槽时后面就有1个占位符，否则只有当前类1个元素。

当前类我理解为用来计算偏移的，后续定义信号时里的偏移应该就是相对于类名来偏移，确定信号或槽的具体位置。第一个成员数组内每个元素都使用了QT_MOC_LITERAL参数宏，第一个参数表示元素索引，第二个参数表示元素在stringdata中的偏移量，第三个参数表示元素对应的字符串长度，实际上就是对QByteArrayData进行初始化。

第二个成员stringdata存储了类名、信号名和槽名的字符串表达。用上述的偏移量来确认具体的string类型的名字。

QMetaObjectPrivate::connect用于实际的信号和槽连接，创建真正的connect对象QObjectPrivate::Connection存储所有的connect信息，然后调用addConnection保存这个connect操作。期间需要对发送和接收方进行上锁，确保这些信息没有被修改。connection函数最终用一个借助于QObjectPrivate类的双向链表connectionLists保存相关连接信息

发送信号时，实际上是调用了QMetaObject::activate函数，该函数会循环寻找包含这个signal的connect。判断当前线程与reveiver线程是否一致，并根据一致性和connect类型进行不同的处理：立即执行queued_activate或者放入消息队列postEvent等待后续处理

1. 若type为AutoConnection且接收线程不在发送线程内，或者type为QueuedConnection（一旦切换到接收线程则触发槽函数），则queued_activate：对参数转换一下，然后调用`QCoreApplication::postEvent(c.receiver, new QMetaCallEvent(c.method,sender,signal,nargs,types,args,semaphore));`这样这个signal-slot的connection就发送到receiver的消息队列中去了。
2. 若type为BlockingQueuedConnection（与 Qt::QueuedConnection 相同，只是信号线程阻塞直到槽返回），则加入等待队列。若接收线程就是本线程则告警，因为会产生死锁（阻塞到槽函数返回，但是槽函数就在本线程内，该线程已经被阻塞，无法返回）
3. 如果是本线程，则通过QConnectionSenderSwitcher sw交换发送者和接受者，然后调用信号连接的槽

位于其他线程的槽函数如何相应参考：https://blog.csdn.net/tingsking18/article/details/5127507

### 一、成员变量、信号、槽函数

成员变量基本都是界面的控件，有一个`QTimer`计数，是用来记录运行时间的，就是界面右下角那个运行时间

```c++
private:
    Ui::Widget *ui;//构造窗口
    void init_csi_graph();//csi图的初始化
signals:
    void send_secret_signal(const std::string &file_path);//这个没用到，虽然后面连接了
```

控件类型

```c++
QLabel* 			//用于显示文本或图像的窗口部件
QCustomPlot*		//用于绘图和数据可视化的部件，用于plot_csi
QComboBox*			//QComboBox小部件是一个组合按钮和弹出列表，也就是选项，比如角色alice/bob
QLineEdit*			//单行文本输入框，允许用户输入和编辑单行纯文本
QCheckBox*			//带文本标签的复选框，比如是否存在eve
QSlider*			//滑动条
QPushButton*		//普通按钮
QMessageBox*		//生成各式各样、各种用途的消息对话框
QPixmap*			//像素图，显示运行中或停止
```

这里面可编辑或者可改变的控件在运行后（执行`a.exec()`）如果修改了，都会发出相应的信号，会有默认或者槽函数进行修改，当然也可以自己设置相应的修改方式，这样的槽函数形式都如`on_控件名_特定的信号名称`。其中特定的信号名称根据控件不同是固定的，如`QPushButton`的`clicked()`就是点击后相应对应信号的槽函数，在介绍槽函数时会简单提一下这部分。

槽函数比较多，后面挑几个比较重要的介绍一下

### 二、构造函数

```c++
//声明
explicit Widget(QWidget *parent = 0);
//定义
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    init_csi_graph();
    /*
    ...
    各种控件设置
    ...
    */
    alert_model = new QMessageBox(QMessageBox::Critical, "Title", "Text",
  QMessageBox::Ok | QMessageBox::Default | QMessageBox::Cancel | QMessageBox::Escape ,0 );
    alert_model->setButtonText(QMessageBox::Ok, "确定");
    alert_model->setButtonText(QMessageBox::Cancel, "取消");//告警消息弹出控件
  //计时器创建，此时还没启动，但是用connect函数(下面会介绍)连接了信号来触发启动(就是点击运行就开始计时)
    msTimer = new QTimer(this);
    connect(msTimer,SIGNAL(timeout()),this, SLOT(TimeSlot()));
}
```

重点说一下这个`ui->setupUi(this);`，这是由.ui文件生成的类的构造函数，这个函数的作用是对界面进行初始化，它按照我们在Qt设计器里设计的样子把窗体画出来，把我们在Qt设计器里面定义的信号和槽建立起来。

同时Qt在`ui->setupUi(this)`中对label进行了内存的分配，因此要放在构造函数一开始的位置，不然会提示内存泄漏

### 三、槽函数与成员函数

#### void on_run_push_button_clicked();

在界面点击运行后，就默认启动这个函数。

在Qt里按钮控件默认对应一个on_pushButton_clicked()成员，如果想用点击信号，在代码中实现on_pushButton_clicked()成员即可。虽然没有connect函数，只定义了pushbutton,也没有看到调用on_pushButton_clicked()的语句，但编译时，moc在背后帮助完成了connect。

比如对象obj有信号A，你只需要在同一个类中完成如下形式的函数即可
`on_obj_a()`。
编译时moc就会自动在背后处理，效果就是对象obj的信号a被连接到这个函数。Qt Meta Object Compiler会读取*.h文件 生成 moc_*.cpp文件。

函数具体做的工作如下：

```c++
/**app 配置**/
    int role_index = ptr_role_combo_box->currentIndex(); // 设置角色
    int mode_index = ptr_mode_combo_box->currentIndex(); // 设置模式
/*
// 设置对等实体的ip
// 设置所需要加密图片的路径
// 设置eve的ip
// 设置是否存在eve
*/

/**usrp 配置**/
float gain = ptr_gain_slider->value(); // 获取增益
float samp_rate = ptr_samp_rate_slider->value(); // 获取采样率 MHz
float carrier_freq = ptr_carrier_freq_slider->value(); // 获取载波 MHz
float tx_carrier_freq = ptr_tx_carrier_freq_slider->value(); // 获取发射载波 MHz
float rx_carrier_freq = ptr_rx_carrier_freq_slider->value(); // 获取接收载波 MHz
int usrp_ip_index = ptr_usrp_ip_combo_box->currentIndex(); // 获取usrp的ip
int latency = ptr_latency_slider->value(); // 获取usrp的延迟

/**Key 存储配置**/
bool save_append = ptr_save_append_check_box->isChecked();// 获取是否需要连续存储
std::string csi_dir = ptr_store_csi_dir_line_edit->text().toStdString(); // 获取存储csi的目录
if (csi_dir.empty()) { // 若未选择图片路径，则报错
   alert_msg("Input Error", "Please Choose CSI Store Path");
   return;
}
if (!is_dir_exist(csi_dir)) {
   alert_msg("Input Error", "CSI store directory does not exist");
   return;
}
// 获取存储key的目录
/*...*/
```

只贴了部分代码，具体的可以看程序。运行到这里说明已经点击了开始运行，那么需要重新设置以下参数，因为可能经过了修改，比如调整增益，或者设置角色等。区别于set_config()，这一步是运行前的更改。如前介绍控件类型时所述，每次在界面上进行修改，都会发出对应的信号，而触发的槽函数会修改掉对应参数的值。因此里面的值会被改掉，需要重新设置。下一步更新setting和global variables，也就是`roi.ini`

```c++
// 更新setting和global variables
QSettings setting(QString::fromStdString(Config::config_file_path), QSettings::IniFormat); // 选择配置文件，若无则创建一个

setting.beginGroup("usrp"); // usrp 设备类 QString::fromStdString(usrp_ip)
/*......*/
```

然后重新设置全局参数

```c++
Config::set_config(Config::config_file_path);
```

获取当前时间并启动计时器，关于计时器的详情可以参考[定时器QTimer的使用](https://www.itread01.com/content/1547841453.html)。

```c++
get_global_current_time(get_current_time());
StartTimer();
```

最后停止当前工作线程（如果有的话），然后创建新线程并启动。WorkThread后面会介绍。

```c++
// 停止工作线程
stop_work_thread();
std::cout<<"start new work thread"<<std::endl;
create_work_thread(); // 创建work_thread所指涉的对象
start_work_thread();
```

#### void on_stop_push_button_clicked();

停止工作线程，调用的是`stop_work_thread();`

#### void create_work_thread();

创建工作线程，并连接对应的信号与槽函数，这里的信号与槽大部分是工作线程传递给Qt界面的，用来传出告警或者绘制csi。

```c++
void Widget::create_work_thread() {
    if (work_thread != nullptr) {
        stop_work_thread();
    }
work_thread = make_shared<WorkThread>();//创建WorkThread

connect(work_thread.get(), SIGNAL(plot_csi_signal(const std::vector<std::complex<float>> &)), this, SLOT(plot_csi(const std::vector<std::complex<float>> &)));
connect(this, SIGNAL(send_secret_signal(const std::string&)), work_thread.get(), SLOT(send_secret(const std::string&)));//这里没用到
connect(work_thread.get(), SIGNAL(stop_work_thread_signal()), this, SLOT(stop_work_thread()));
connect(work_thread.get(), SIGNAL(alert_msg_signal(const QString&, const QString&)), this, SLOT(alert_msg(const QString&, const QString&)));
connect(&work_thread->transceiver, SIGNAL(alert_msg_signal(const QString&, const QString&)), this, SLOT(alert_msg(const QString&, const QString&)));

}
```

此时只是创建了work_thread类，具体的线程还没有产生

#### void start_work_thread();

```c++
void Widget::start_work_thread() {
    try {
        work_thread->start();
    } catch (std::exception e) {
        stop_work_thread(); // 销毁
        alert_msg("exception", e.what());
        return;
    } catch (std::runtime_error e) {
        stop_work_thread(); // 销毁
        alert_msg("runtime error", e.what());
        return;
    }
    set_run_status_ui();
}
```

`work_thread->start();`会调用类里的run函数，产生线程执行run函数内部逻辑

#### void stop_work_thread();

```c++
void stop_work_thread() {
   set_stop_status_ui();
   if (work_thread != nullptr && work_thread->started()) {
       std::cout << "stop work thread" << std::endl;
       work_thread->stop_server_thread();
       work_thread->stop_transceiver();
       work_thread->terminate();
       work_thread->wait();
       work_thread->set_started_status(false);
        }
   std::cout << "work thread has been stopped" << std::endl;
   work_thread = nullptr;
}
```

这里结束线程是通过work_thread调用terminate()申请结束，由操作系统调度合适结束，wait()会阻塞一直等到workthread线程结束再执行下面的代码。注意，结束的是线程，而不是删除workthread这个指针。

#### void role_or_mode_combo_box_currentIndexChanged(int role_index, int mode_index);

根据角色不同（Alice/Bob）以及工作模式不同（TDD/FDD）改变界面部分显示。

#### void alert_msg(const QString& title, const QString& text);

用来产生告警窗口，发生错误时（比如设备连接错误）会发出信号，触发该槽函数来打出告警窗口。

## 三、WorkThread

工作线程，整个系统都是有此线程控制，其中包括其他线程：gnuradio、客户端/服务器

### 成员变量

```c++
public:
    Transceiver transceiver;//gnuradio 线程
private:
    semaphore sema_loop;//信号量，没用到
    bool status_start;//工作线程开始状态标志
    SocketClientThread client;//客户端
    SocketServerThread server_thread;//服务器线程
    std::vector<std::complex<float>> csi;
    std::mutex mu;//线程锁
```

### 构造函数

```c++
WorkThread (): client(), server_thread(Config::port), status_start(false), sema_loop(1){};
```

`sema_loop`没用到，`status_start`初始化为false表示线程未开始，客户端和服务器的没搞明白呢回头再补上，不过Alice端是client，Bob端是server。每次成功存储信息并计算出CSI后，alice端连接向服务器发送crc，如果匹配成功，则发送加密文字和加密图片，文字用turbo码传送

### 信号和槽函数

```c++
signals:
    void plot_csi_signal(const std::vector<std::complex<float>> &csi);//画图的信号
    void alert_msg_signal(const QString& title, const QString& text);//告警信号
private slots:
    void send_turbo_encoded_msg (const std::string& msg);//发送加密文字
	void send_secret (const std::string &file_path);//发送加密图片
```

这里的两个槽函数其实只当成了普通的私有成员使用。

`send_turbo_encoded_msg`函数先获取界面里填入的文字信息，然后把文字转换成turbo码并加密。然后创建客户端socket，连接服务器，发送加密文字信息，然后关闭这个socket。

`send_secret`创建客户端socket，连接服务器，发送加密图片信息，然后关闭这个socket。

该部分在Network，我还没搞明白，后面再补上。

### 成员函数

#### void run()；

这就是`work_thread->start();`后实际工作的run函数

该函数分为三种角色：Alice，Bob，Eve。

第一个if判断工作线程有没有启动，如果没有则启动线程：

1. 设置状态为true；
2. 开启gnuradio线程进行信号采集，这里为了防止stop_transceiver出现数据竞险的情况，需要加锁来保证gnuradio开启的原子性。（可能产生stop_transceiver的情况有关闭界面，或者点击停止强行中断工作线程）
3. 开启服务器线程（Bob端开启有用，Alice端似乎没啥用）

第二个if判断工作角色来决定工作内容

- Alice端：循环执行以下步骤
  1. 更新全局的时钟，标注本次交互开始的时间
  2. 调用send_frame()发送信号
  3. 计算csi并画出图像
  4. 发送crc，若匹配一致则发送加密后的文字或图片信息
- Bob端：TDD，循环执行如下操作
  1. 以100ms的频率来实现slave机文件无效，也就是将status_file置false。status_file确保此次接收到的是新的文件，也就是usr/rx_data是最新接收到的，而不是之前的。
- Eve端：同Bob

#### bool send_frame ();

1. 先确保发送状态为false：`transceiver.set_file_status(false);`
1. 判断文件状态（Alice端是否成功存储文件），若收到则退出循环，进行下面的步骤
2. 上锁，判断usrp是否连接好
3. 如果连接好，则发送数据，
4. 如果未连接好，报错，等待线程被杀死。
5. 等待间隔时间，重复2-6

#### void compute_csi ();

调用CSI文件夹下函数计算csi

#### bool send_crc ();

1. 连接服务器端(Bob)，进行crc校验，获取crc_marks，然后关闭连接
2. 计算正确率，如果超过阈值进行3，反之返回false跳出
3. 如果eve存在并且匹配程度很高，则给eve发送crc_marks，返回true

## 四、Transceiver

顶层文件，也就是gnuradio流图，在这部分完成流图的构建和启动。而GModule里存的是我们要在流图里使用的模块。gnuradio的模块创建都由**make()**函数完成

关于gnuradio内核运作这个链接讲的比较细了：https://www.cnblogs.com/nickchan/archive/2011/11/19/3104453.html，下面介绍top_block成员变量是会整理一下里面的内容

### 成员变量

```c++
private:
    gr::top_block_sptr top_block;//流图的指针

    // 发射的blocks
    gr::uhd::usrp_sink::sptr block_usrp_sink;//usrp发出的模块
    gr::blocks::multiply_const_vcc::sptr block_multiply_const;//乘法器，用来限制输入输出大小
    gr::roi::file_source_roi::sptr block_file_source_roi;//发送模块

    // 接收的blocks
    gr::uhd::usrp_source::sptr block_usrp_source;//usrp接收的模块
    gr::roi::file_sink_roi::sptr block_file_sink_roi;//接收模块

    // 显示所要用的blocks，没用上
    gr::qtgui::const_sink_c::sptr block_constellation;
    gr::qtgui::freq_sink_c::sptr block_spectrum;
    gr::qtgui::waterfall_sink_c::sptr block_waterfall;

    gr::blocks::throttle::sptr block_throttle;//这也是显示用的，没用上

    struct uhd::stream_args_t stream_args;//usrp的流参数

    std::string ip;
    std::map<std::string,std::string> info;//X310需要设置时钟，需要传入两个参数
```

#### top_block

`gr::top_block_sptr`是top_block类的智能指针，这个类里面封装了包括start()，wait()等的函数用于控制流图的启动，有个具体执行的成员变量top_block_impl类型的指针。具体的执行过程可以找到一个叫top_block_impl.cc的文件。

> gnuradio里面具体的执行函数好像都是叫xxx_impl这样的后缀

top_block_impl.cc中的start()函数主要调用了d_scheduler = make_scheduler(d_ffg);来创建**GNU Radio的调度器**

这个make_scheduler(d_ffg);调用了一个scheduler_maker类型的函数来创建**scheduler_table**

```c++
typedef scheduler_sptr(*scheduler_maker)(flat_flowgraph_sptr ffg,
                                           int max_noutput_items);
  static scheduler_sptr
  make_scheduler(flat_flowgraph_sptr ffg, int max_noutput_items)
  {
    static scheduler_maker factory = 0;
       if(factory == 0) {
      char *v = getenv("GR_SCHEDULER");
      if(!v)
        factory = scheduler_table[0].f;	// use default
           //...
       }
	return factory(ffg, max_noutput_items);
  }
```

`factory = schedulertable[i].f`里的**scheduler_table**是一个结构体

```c++
static struct scheduler_table {
  const char       *name;
  scheduler_maker f;
} scheduler_table[] = {
  { "TPB", scheduler_tpb::make }, // first entry is default
  { "STS", scheduler_sts::make }
};
```

它指向一个在scheduler类里边的成员函数，make。简单理解：检查是否存在Linux 环境变量：GR_SCHEDULER。如果没有，就是用默认的调度器；否则，是用用户选择的。只有如下两个：

1. TPB (default): multi-threaded scheduler.

2. STS: single-threaded scheduler.

默认使用TPB，scheduler_tpb::make调用`new scheduler_tpb(ffg, max_noutput_items)`来创建TPB调度器，scheduler_tpb构造函数会统计流图内所用的模块数量，然后设置模块细节，并为每个模块创建多线程

```c++
  scheduler_tpb::scheduler_tpb(flat_flowgraph_sptr ffg,
                               int max_noutput_items)
    : scheduler(ffg, max_noutput_items)
  {
    int block_max_noutput_items;

    // Get a topologically sorted vector of all the blocks in use.
    // Being topologically sorted probably isn't going to matter, but
    // there's a non-zero chance it might help...

    basic_block_vector_t used_blocks = ffg->calc_used_blocks();
    used_blocks = ffg->topological_sort(used_blocks);
    block_vector_t blocks = flat_flowgraph::make_block_vector(used_blocks);

    // Ensure that the done flag is clear on all blocks

    for(size_t i = 0; i < blocks.size(); i++) {
      blocks[i]->detail()->set_done(false);
    }

    // Fire off a thead for each block

    for(size_t i = 0; i < blocks.size(); i++) {
	//...
      d_threads.create_thread(//使用boost::thread创建线程，括号里的函数就是线程运行的函数
	    gr::thread::thread_body_wrapper<tpb_container>
            (tpb_container(blocks[i], block_max_noutput_items),
             name.str()));
    }
  }
```

create_thread()具体实现

```c++
boost::thread* thread_group::create_thread(const boost::function0<void>& threadfunc)
{
    // No scoped_lock required here since the only "shared data" that's
    // modified here occurs inside add_thread which does scoped_lock.
    std::auto_ptr<boost::thread> thrd(new boost::thread(threadfunc));//创建线程
    add_thread(thrd.get());//加入线程调度
    return thrd.release();
}
void thread_group::add_thread(boost::thread* thrd)
{
    boost::lock_guard<boost::shared_mutex> guard(m_mutex);

    // For now we'll simply ignore requests to add a thread object
    // multiple times. Should we consider this an error and either
    // throw or return an error value?
    std::list<boost::thread*>::iterator it =
        std::find(m_threads.begin(), m_threads.end(), thrd);
    BOOST_ASSERT(it == m_threads.end());
    if (it == m_threads.end())
        m_threads.push_back(thrd);//在线程的链表里寻找，如果没找到，就将当前线程放在线程链表最后
}
```



> **总结GNU RADIO 调度器作用：**
>
> 1. 分析在 gr_top_block中每个block
>
> 2. 默认调度器是TPB，用来创造每个blocks里边的多线程
>
> 3. 调度器为每个block创建线程
>
> 4. 对于每个block， 线程入口都在gr_tpb_thread_body 里边

每个block的线程都被thread_body_wrapper包裹，在其中会利用重载operate()调用**tpb_container**类型的成员变量`d_f`，tpb_container调用tpb_thread_body作为线程入口，在这个线程中循环执行。

循环中，首先处理所有的信号，然后执行`run_one_iteration();`，这是整个线程的核心，这个函数主要完成：

1. 检查是否存在足够的输出数据空间
2. 检查是否存在足够的可靠输入数据
3. 如果又足够的输入数据和足够的输出空间，代码就会执行block里的**general_work()**

> **总结每个block执行逻辑**
>
> 1. 每个block的线程都有一个while(1)循环
>
> 2. 循环处理信号并且运行主要函数 run_one_iteration()
>
> 3. run_one_iteration() 检查 是否有足够的数据输入和为这个block输出足够的可靠的空间
>
> 4. 如果都可以，调用general_work() 去跑这个block主要的功能。否则，返回BLKD_OUT, BLKD_IN,或者其他。

```c++
//block读取输入，ninputs()代表有多少输入端，我们的模块只有1个
for (int i = 0; i < d->ninputs(); i++)
      d_input_items[i] = d->input(i)->read_pointer();//d是block_detail类型的指针，可以看出模块里的in就是buffer里的指针
```

#### block_usrp_sink

usrp的发射模块，根据地址信息和stream_args流参数创建发射的block

stream_args流参数包括输入输出类型，设备地址参数（没太看明白这个是怎么产生设备地址的），以及天线（默认单天线0）

#### block_usrp_source

同sink

#### block_file_sink_roi

在GModule中，其实是提前安装进gnuradio里了，需要在cmakelist中find_package(roi)来找到定义

#### block_file_source_roi

在GModule中

### 成员函数

这个类就是用来创建流图的，构造函数就被替换成了成员函数`void construct_top_block()`

#### void construct_top_block()

1. 设置参数：usrp：采样率，载波频率，收发增益，天线，带宽，收发间隔，收发文件名，stream_args；roi_sink：fft_size

2. 创建顶层指针`top_block`：`top_block = gr::make_top_block("Transceiver");`

3. 创建usrp发射模块

   1. ```c++
              block_usrp_sink = gr::uhd::usrp_sink::make(
                      info["addr="+ip]="master_clock_rate=184.32e6",//这是LTE-V2X版本的设置
                      stream_args
              );
      ```

   2. 发射模块参数设置：采样率，时钟，载波频率，发射增益，天线，带宽

4. 创建roi_source

   ```c++
   //Alice和Bob需要该部分，Eve不用
   if(Config::role_index != Config::ROLES::EVE){
       block_file_source_roi = gr::roi::file_source_roi::make(sizeof(gr_complex), source_filename.c_str(), false);
   }
   ```

5. 创建usrp接收模块

   1. ```c++
      		block_usrp_source = gr::uhd::usrp_source::make(
                      info["addr="+ip]="master_clock_rate=184.32e6",
                      stream_args
              );
      ```

   2. 接收模块参数设置：采样率，时钟，载波频率，发射增益，天线，带宽

6. 创建roi_sink

   1. ```c++
      block_file_sink_roi = gr::roi::file_sink_roi::make(
          sink_filename.c_str(), false, Config::cell_id, Config::threshold, Config::threshold_DMRS,1,fft_size, true, gr::fft::window::blackmanharris(fft_size), false, 2,Config::energe, Config::energe_DMRS,latency,Config::time_slot,Config::alice_or_not);
      ```

   2. 设置为缓存模式：`block_file_sink_roi->set_unbuffered(false);`

7. 流图连接

   ```c++
       if (Config::role_index == Config::ROLES::ALICE || Config::role_index == Config::ROLES::BOB) { // 仅当alice和bob才会发射信号，eve只有接收信号
           top_block->connect(block_file_source_roi,0,block_usrp_sink,0);
       }
   // receive connections
       top_block->connect(block_usrp_source, 0, block_file_sink_roi, 0);
       /***alice的发送由send_frame()控制***/
       if (Config::rx_tx_connect) //只有Bob端需要连接msg，用来触发文件的发送 
           top_block->msg_connect(block_file_sink_roi, "msg_status_file", block_file_source_roi, "msg_status_file");
   ```

这样的话顶层文件就算建立完成了，如果在gnuradio软件里实现，就相当于拖出来四个模块进行参数设置和连接。由于我们没用到流图（.grc）文件，所以应该不需要python来辅助进行。

#### Transceiver控制相关的函数

```c++
void Transceiver::run() {top_block->run();}
void Transceiver::wait() {top_block->wait();}
void Transceiver::stop() {top_block->stop();}
void Transceiver::start() {
    try {
        construct_top_block();
    } catch (QString text) {
        throw text;
    }
    top_block->start();
}
```

run和wait好像没用上，主要就是start和stop

#### bool check_usrp_connection ();

```c++
bool Transceiver::check_usrp_connection () {
    try { // 每次发射前，检查usrp是否断开，这里现用这种方法代替，真实判断是否连接的代码在驱动中，这里我没有办法拿到，能追溯到的最底层的代码就是工厂模式make一个指针
        gr::uhd::usrp_sink::sptr block_usrp_sink = gr::uhd::usrp_sink::make(
                info["addr="+ip]="master_clock_rate=184.32e6",
                stream_args
        );
    } catch (uhd::key_error e) {
        return false;
    }
    return true;
}
```

其实这种方法可以实现检查的功能，追溯make到最后可以追溯到一个device.cpp文件中的make

```c++
device::sptr device::make(const device_addr_t &hint, device_filter_t filter, size_t which){
//....
    //create a unique hash for the device address
    device_addr_t dev_addr; make_t maker;
    boost::tie(dev_addr, maker) = dev_addr_makers.at(which);
    size_t dev_hash = hash_device_addr(dev_addr);
    UHD_LOGGER_TRACE("UHD") << boost::format("Device hash: %u") % dev_hash ;

    //copy keys that were in hint but not in dev_addr
    //this way, we can pass additional transport arguments
    for(const std::string &key:  hint.keys()){
        if (not dev_addr.has_key(key)) dev_addr[key] = hint[key];
    }

    //map device address hash to created devices
    static uhd::dict<size_t, boost::weak_ptr<device> > hash_to_device;

    //try to find an existing device
    if (hash_to_device.has_key(dev_hash) and not hash_to_device[dev_hash].expired()){
        return hash_to_device[dev_hash].lock();
    }
    else {
        // Add keys from the config files (note: the user-defined keys will
        // always be applied, see also get_usrp_args()
        // Then, create and register a new device.
        device::sptr dev = maker(prefs::get_usrp_args(dev_addr));
        hash_to_device[dev_hash] = dev;
        return dev;
    }
}

```

这个函数会吕勇设备地址计算hash并存到一个hash_to_device 表中，如果该hash已经存在，就会返回表中的指针，反之则创建。所以不用担心重复创建设备指针会造成问题。

#### void transmit_file();

发送文件，主要是把source_roi的**tx_file**参数设为true，以执行后续代码发出文件。

work和general_work一致，只是因为继承的基类不同，source_roi继承自sync_block，该基类里同时有work()和general_work()，work()被定义为纯虚函数，所以必须被重新定义，但调用还是调用的general_work，因为在general_work中会调用work()函数，两者参数一致，就像套了个壳

```c++
  int
  sync_block::general_work(int noutput_items,
                           gr_vector_int &ninput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
  {
    int r = work(noutput_items, input_items, output_items);
    if(r > 0)
      consume_each(r);
    return r;
  }
```

如下为source_roi的work部分

```c++
     int file_source_roi_impl::work(int noutput_items, gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items) {
          cnt++;

          char *o = (char*)output_items[0];
          int i;
          int size = noutput_items;
          /**
           * 如果不发射文件内容, 则不产生输出(即不往外送数据)
           */
          if (!tx_file) {
              return 0;
          }
          /*实际发送部分*/
}
```

#### bool check_file_status();

```c++
bool Transceiver::check_file_status() {
    return block_file_sink_roi->get_status_file();//获取status_file标志，判断文件有效性（新旧）
}
```

#### void set_file_status(bool status_file);

设置status_file标志

```c++
void set_file_status(bool status_file){
    block_file_sink_roi->set_status_file(status_file);
};
```

## 补充：

### scheduler_tpb

线程调度表，具体线程调度的过程

used_blocks：basic_block_vector_t类，其实就是vector<basic_block_ptr>存了所有block的指针

```
输入是vector<void *>类型
输出是vector<const void *>类型，这个vector存的是对应天线的，0表示天线1
```

输入输出的缓冲区只有一个buffer_read_sptr和buffer_sptr，consume的时候是直接加减计算的，应该是一段连续的空间

具体创建buffer的函数是，buffer.cc里的buffer::allocate_buffer

然后需要调用vmcircbuf.cc，同时下面vmcircbuf_createfilemapping.cc以及vmcircbuf_mmap_shm_open.cc，猜测和地址映射有关

https://www.cnblogs.com/moon1992/p/5959806.html

GNU Radio采用两种机制: Message passing and stream tag，在block 之间传输信息，例如接收时间，中心频率，子帧号或者特定的协议信息。其中Stream tag是同步标签，只能单向传输。Message是异步消息，可以向任何方向传输。在GNU Radio中，Stream tag中实线表示，Message用虚线表示。注意：Stream tag与数据流是并行传输模式，不是插入到原始数据流，也不会改变原始数据流，而是绑定到数据流的某一个样点，只能在block之间传递消息，不能通过天线发送出去！

异步消息传递：

- 当消息发出，它被放在每一个订阅队列
- 消息处理在general_work之前
- 调度器调度消息
  - 查看是否有处理该消息的函数
    - 如果没有处理程序，则保留一个max_nmsgs队列
    - 如果队列中超过 max_nmsgs，则丢弃最旧的消息
    - max_nmsgs 在 [DEFAULT]:max_messages 的首选项文件中设置
  - pop掉消息
  - 通过调用块的处理程序调度消息

流tag：

- 向数据流添加控制、逻辑和元数据层。

- 标签携带与特定样本相关的键/值数据。

- 标签通过每个块向下游传播
- 标签随数据速率变化而更新

在block.h 中定义了3种tag传播策略：

```c++
enum tag_propagation_policy_t {
    TPP_DONT = 0,
    TPP_ALL_TO_ALL = 1,
    TPP_ONE_TO_ONE = 2
};
```

其中默认是TPP_ALL_TO_ALL，即输入端口收到的tag 会传输到每一个输出端口。TPP_ONE_TO_ONE模式是输入端口 i 收到的tag只能传出到输出端口 i 。TPP_DONE表示该block 收到tag 之后不再向后面的模块传输。

缓冲区是循环缓冲区

缓冲区在运行时创建

- 当调用 start 时，流程图被展平并创建连接。
- 创建 gr::block_details 并为每个输出创建一个 gr::buffer。通过附加 gr::buffer_reader 连接输入。
  - 缓冲区大小计算为要保存的项目数。
    - 应用最小/最大限制（如果设置）
