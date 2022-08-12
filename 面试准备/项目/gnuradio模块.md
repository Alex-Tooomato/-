Bob端：

使用到的参数：

```cpp
int d_timeslot          //PSSCH所在子帧位置
int d_save_status       //判断此时接收状态：-1 代表尚未检测到PSS，并持续对收到的数据进行检             测；0 代表检测到PSS，开始消耗空数据；1 开始存储PSSCH
int d_waitslot          //记录PSBCH到PSSCH有多少个采样点，每次循环加input_items_num，当d_waitslot==规定的采样点数时，开始存储(Bob端)或开始
int d_receive_length    //接受长度，一般为1，即1个子帧
int d_detect_wait       //检测等待，若检测PSS失败则跳过d_detect_wait次检测，或者接收成功后跳过d_detect_wait次检测，在实际环境中可不用。
int First_Detection     //检测PSS之前的能量检测次数
int shift_index         //根据fft大小决定PSS相关检测偏移
bool corr_start         //当剩余数据不够进行PSS相关时启用，下一次循环直接开始做相关
float d_energe          //PSS能量检测阈值
float d_threshold       //PSS相关的阈值
bool pss_found          //是否找到PSS
```

通过检测PSBCH中的PSS序列进行同步，随后消耗掉相应点数的空数据（根据A端发送的数据决定），接收PSSCH，流程如下：

第一步——case -1：先进行3次能量检测(detect_energe函数)，First_Detection==3时进行相关计算（由于每次循环读入的数据只有9000-10000个采样点，可能需要在下一次循环中进行相关计算，此时启用corr_start）。找出相关结果中的最大值(find_max函数)，比较阈值，通过则将pss_found置为true，d_save_status置0，开始消耗空数据

第二步——csse 0:通过d_waitslot记录一共消耗了多少点数，ret为需要在本次循环结束后消耗的点数，每次循环若未达到要求的点数则d_waitslot+= input_items_num。总过消耗的点数包括PSBCH中除去第一段PSS结束之前剩下的点数，加上PSBCH与PSSCH中间间隔的子帧。因此有多段判别，主要是为了指示PSBCH的结束。当然后续也可以直接合并，不用指示PSBCH的结束。消耗完将d_save_status置 1 ，开始存储PSSCH

第三步——case 1:进行存储，存储完d_save_status置 -1 。

Alice端：

使用到的参数：

```cpp
int d_timeslot          //PSSCH所在子帧位置
int d_save_status       //判断此时接收状态：-1 代表尚未检测到PSS，并持续对收到的数据进行检测；0 代表检测到PSS，开始消耗空数据；1 开始存储PSSCH
int d_waitslot          //记录PSBCH到PSSCH有多少个采样点，每次循环加input_items_num，当d_waitslot==规定的采样点数时，开始存储(Bob端)或开始
int d_receive_length    //接受长度，一般为1，即1个子帧
int d_detect_wait       //检测等待，若检测PSS失败则跳过d_detect_wait次检测，或者接收成功后跳过d_detect_wait次检测，在实际环境中可不用。
int First_Detection     //检测PSS之前的能量检测次数
int shift_index         //根据fft大小决定PSS相关检测偏移
bool corr_start         //当剩余数据不够进行PSS相关时启用，下一次循环直接开始做相关
bool pss_found          //是否找到PSS
bool d_alice            //是否是alice
float d_energe          //PSS能量检测阈值
float d_energe_DMRS     //DMRS能量检测阈值
float d_threshold       //PSS相关的阈值
float d_threshold_DMRS  //DMRS能量检测阈值
bool d_alice_rec        //alice是否可以开始检测DMRS
```

第一步——case -1：先进行3次能量检测(detect_energe函数)，First_Detection==3时进行相关计算（由于每次循环读入的数据只有9000-10000个采样点，可能需要在下一次循环中进行相关计算，此时启用corr_start）。找出相关结果中的最大值(find_max函数)，比较阈值，通过则将pss_found置为true，d_save_status置0，开始消耗空数据

第二步——case 0:区别于Bob端，alice端自检测PSS可能会出现问题，若未检测到PSS，则按照推测值进行消耗，否则和Bob端一致

第三步——case 1:存储前先检测能量，若检测到PSSCH的能量，则进行DMRS的相关，若相关结果超过阈值，则表示PSSCH同步成功，之后按照点数进行存储。否则按照能量检测的位置进行存储。若未检测到能量，则直接存（错了就等待下一个A->B轮次）