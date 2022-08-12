# 习惯

在每個 script 的檔頭處記錄好：

- script 的功能；
- script 的版本資訊；
- script 的作者與聯絡方式；
- script 的版權宣告方式；
- script 的 History (歷史紀錄)；
- script 內較特殊的指令，使用『絕對路徑』的方式來下達；
- script 運作時需要的環境變數預先宣告與設定。

# 语法命令

## export

export **功能说明：**设置或显示[环境变量](https://so.csdn.net/so/search?q=环境变量&spm=1001.2101.3001.7020)。

**语　　法：**export [-fnp][变量名称]=[变量设置值]
**补充说明：**在shell中执行程序时，shell会提供一组环境变量。export可新增，修改或删除环境变量，供后续执行的程序使用。export的效力仅限于该次登陆操作。
**参　　数：**
　-f 　代表[变量名称]中为函数名称。
　-n 　删除指定的变量。变量实际上并未删除，只是不会输出到后续指令的执行环境中。
　-p 　列出所有的shell赋予程序的环境变量。

## 判断式

### test测试功能

bash中可以用`\&?` 或者 `&&`及`||`来进行判断

> &&与 ；的区别：
>
> ；表示命令的先后执行。而&&只有前一个命令执行成功，后一个命令才会执行。

| 測試的標誌                                                   | 代表意義                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| 1. 關於某個檔名的『檔案類型』判斷，如 test -e filename 表示存在否 |                                                              |
| -e                                                           | 該『檔名』是否存在？(常用)                                   |
| -f                                                           | 該『檔名』是否存在且為檔案(file)？(常用)                     |
| -d                                                           | 該『檔名』是否存在且為目錄(directory)？(常用)                |
| -b                                                           | 該『檔名』是否存在且為一個 block device 裝置？               |
| -c                                                           | 該『檔名』是否存在且為一個 character device 裝置？           |
| -S                                                           | 該『檔名』是否存在且為一個 Socket 檔案？                     |
| -p                                                           | 該『檔名』是否存在且為一個 FIFO (pipe) 檔案？                |
| -L                                                           | 該『檔名』是否存在且為一個連結檔？                           |
| 2. 關於檔案的權限偵測，如 test -r filename 表示可讀否 (但 root 權限常有例外) |                                                              |
| -r                                                           | 偵測該檔名是否存在且具有『可讀』的權限？                     |
| -w                                                           | 偵測該檔名是否存在且具有『可寫』的權限？                     |
| -x                                                           | 偵測該檔名是否存在且具有『可執行』的權限？                   |
| -u                                                           | 偵測該檔名是否存在且具有『SUID』的屬性？                     |
| -g                                                           | 偵測該檔名是否存在且具有『SGID』的屬性？                     |
| -k                                                           | 偵測該檔名是否存在且具有『Sticky bit』的屬性？               |
| -s                                                           | 偵測該檔名是否存在且為『非空白檔案』？                       |
| 3. 兩個檔案之間的比較，如： test file1 -nt file2             |                                                              |
| -nt                                                          | (newer than)判斷 file1 是否比 file2 新                       |
| -ot                                                          | (older than)判斷 file1 是否比 file2 舊                       |
| -ef                                                          | 判斷 file1 與 file2 是否為同一檔案，可用在判斷 hard link 的判定上。 主要意義在判定，兩個檔案是否均指向同一個 inode 哩！ |
| 4. 關於兩個整數之間的判定，例如 test n1 -eq n2               |                                                              |
| -eq                                                          | 兩數值相等 (equal)                                           |
| -ne                                                          | 兩數值不等 (not equal)                                       |
| -gt                                                          | n1 大於 n2 (greater than)                                    |
| -lt                                                          | n1 小於 n2 (less than)                                       |
| -ge                                                          | n1 大於等於 n2 (greater than or equal)                       |
| -le                                                          | n1 小於等於 n2 (less than or equal)                          |
| 5. 判定字串的資料                                            |                                                              |
| test -z string                                               | 判定字串是否為 0 ？若 string 為空字串，則為 true             |
| test -n string                                               | 判定字串是否非為 0 ？若 string 為空字串，則為 false。 註： -n 亦可省略 |
| test str1 == str2                                            | 判定 str1 是否等於 str2 ，若相等，則回傳 true                |
| test str1 != str2                                            | 判定 str1 是否不等於 str2 ，若相等，則回傳 false             |
| 6. 多重條件判定，例如： test -r filename -a -x filename      |                                                              |
| -a                                                           | (and)兩狀況同時成立！例如 test -r file -a -x file，則 file 同時具有 r 與 x 權限時，才回傳 true。 |
| -o                                                           | (or)兩狀況任何一個成立！例如 test -r file -o -x file，則 file 具有 r 或 x 權限時，就可回傳 true。 |
| !                                                            | 反相狀態，如 test ! -x file ，當 file 不具有 x 時，回傳 true |

### 判断符号

中括号的两端需要有空白字元来分隔。

```shell
[  "$HOME"  ==  "$MAIL"  ]
[□"$HOME"□==□"$MAIL"□]
 ↑       ↑  ↑       ↑
```

- 在中括號 [] 內的每個元件都需要有空白鍵來分隔；
- 在中括號內的變數，最好都以雙引號括號起來；
- 在中括號內的常數，最好都以單或雙引號括號起來。

```bash
read -p "Please input (Y/N): " yn
[ "${yn}" == "Y" -o "${yn}" == "y" ] && echo "OK, continue" && exit 0
```

### Shell script 的预设变量(\$0, \$1...)

```shell
/path/to/scriptname  opt1  opt2  opt3  opt4 
       $0             $1    $2    $3    $4
```

#### 特殊变数

- $# ：代表後接的參數『個數』，以上表為例這裡顯示為『 4 』；
- "\$@" ：代表『 "\$1" "$2" "\$3" "\$4" 』之意，每個變數是獨立的(用雙引號括起來)；
- "\$*" ：代表『 "\$1c\$2c\$3c\$4" 』，其中 c 為分隔字元，預設為空白鍵， 所以本例中代表『 "\$1 \$2 \$3 \$4" 』之意。

一般使用情況下可以直接記憶 "$@" 即可，但"\$*"还是有些不同的

```shell
echo "The script name is        ==> ${0}"
echo "Total parameter number is ==> $#"
[ "$#" -lt 2 ] && echo "The number of parameter is less than 2.  Stop here." && exit 0
echo "Your whole parameter is   ==> '$@'"
echo "The 1st parameter         ==> ${1}"
echo "The 2nd parameter         ==> ${2}"
```

```bash
[dmtsai@study bin]$ sh how_paras.sh theone haha quot
The script name is        ==> how_paras.sh       <==檔名
Total parameter number is ==> 3                  <==果然有三個參數
Your whole parameter is   ==> 'theone haha quot' <==參數的內容全部
The 1st parameter         ==> theone             <==第一個參數
The 2nd parameter         ==> haha               <==第二個參數
```

#### **shift：造成参数变数号码偏移**

```shell
echo "Total parameter number is ==> $#"
echo "Your whole parameter is   ==> '$@'"
shift   # 進行第一次『一個變數的 shift 』
echo "Total parameter number is ==> $#"
echo "Your whole parameter is   ==> '$@'"
shift 3 # 進行第二次『三個變數的 shift 』
echo "Total parameter number is ==> $#"
echo "Your whole parameter is   ==> '$@'"
```

```bash
[dmtsai@study bin]$ sh shift_paras.sh one two three four five six <==給予六個參數
Total parameter number is ==> 6   <==最原始的參數變數情況
Your whole parameter is   ==> 'one two three four five six'
Total parameter number is ==> 5   <==第一次偏移，看底下發現第一個 one 不見了
Your whole parameter is   ==> 'two three four five six'
Total parameter number is ==> 2   <==第二次偏移掉三個，two three four 不見了
Your whole parameter is   ==> 'five six'
```

## 条件判断式

### if...then

```shell
if [ 条件判断式 ]; then
	当条件判断式成立时，可以进行的指令工作内容;
fi   <==将 if 反过来写，就成为 fi 啦！结束 if 之意！
```

判断式可以用&&和||来连接多个括号

```shell
[ "${yn}" == "Y" -o "${yn}" == "y" ]
上式可以替换为
[ "${yn}" == "Y" ] || [ "${yn}" == "y" ]
```

### if...else...then

```shell
if [ 条件判断式 ]; then
	当条件判断式成立时，可以进行的指令工作内容;
else
	当条件判断式不成立时，可以进行的指令工作内容;
fi
```

更复杂时

```shell
if [ 条件判断式 ]; then
	当条件判断式一成立时，可以进行的指令工作内容;
elif[ 条件判断式二 ]; then
	当条件判断式二成立时，可以进行的指令工作内容;
else
	当条件判断式一与二均不成立时，可以进行的指令工作内容;
fi
```

### case...esac

类似switch

```shell
case $变数名称 in   
  "第一个变数内容")
  	    程序段
  	    ;;
  "第二个变数内容")
  	    程序段
  	    ;;  
  *)               #相当于default
  		exit 1
  		;;
esac
```

变数也可以通过输入获取

```shell
read -p "Input your choice: " choice   
case ${choice} in    
esac
```



### function

```shell
function fname() {
	程式段
}
```

因为 shell script 的执行方式是由上而下，由左而右， 因此在 shell script 当中的 function 一定要在程式的最前面

#### function的参数

```shell
function printit(){
	echo "Your choice is ${1}"   # 這個 $1 必須要參考底下指令的下達
}

echo "This program will print your selection !"
case ${1} in
  "one")
	printit 1  # 請注意， printit 指令後面還有接參數！
	;;
  "two")
	printit 2
	;;
  "three")
	printit 3
	;;
  *)
	echo "Usage ${0} {one|two|three}"
	;;
esac
```

printit的参数和脚本后面的one/two/three无关，他的参数时调用时后面的参数1/2/3

## 循环

### while do done, until do done

```shell
while [ condition ]  <==中括号内的状态就是判断式
do            <==do 循环的开始
	程式段落
done          <==done 循环结束
```

当 condition 条件成立時，就进行循环，直到 condition 的条件不成立才停止

```shell
until [ condition ]
do
	程式段落
done
```

与 while 相反， 当 condition 条件成立時，就终止循环，否则就持续循环

```shell
while [ "${eated}" -lt 3 ]; do #eated<3
done
```

```shell
-eq #equals等于
-ne #no equals不等于
-gt #greater than 大于
-lt #less than小于
-ge #greater equals大于等于
-le #less equals小于等于
```



### for...do...done 

```shell
for var in con1 con2 con3 ...
do
	程式段
done
```

以上面的例子來說，這個 $var 的變數內容在迴圈工作時：

1. 第一次迴圈時， $var 的內容為 con1 ；
2. 第二次迴圈時， $var 的內容為 con2 ；
3. 第三次迴圈時， $var 的內容為 con3 ；
4. ....

```shell
for animal in dog cat elephant
do
	echo "There are ${animal}s.... "
done
```

连续数字

```shell
network="192.168.1"              # 先定義一個網域的前面部分！
for sitenu in $(seq 1 100)       # seq 為 sequence(連續) 的縮寫之意
do
	# 底下的程式在取得 ping 的回傳值是正確的還是失敗的！
        ping -c 1 -w 1 ${network}.${sitenu} &> /dev/null && result=0 || result=1
	# 開始顯示結果是正確的啟動 (UP) 還是錯誤的沒有連通 (DOWN)
        if [ "${result}" == 0 ]; then
                echo "Server ${network}.${sitenu} is UP."
        else
                echo "Server ${network}.${sitenu} is DOWN."
        fi
done
```

除了使用 \$(seq 1 100) 之外，也可以使用 {1..100}來取代 \$(seq 1 100) 。 那個大括號內的前面/後面用兩個字元，中間以兩個小數點來代表連續出現的意思

找出目录权限

```shell
#!/bin/bash
# Program:
#	User input dir name, I find the permission of files.
# History:
# 2015/07/17	VBird	First release
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH

# 1. 先看看這個目錄是否存在啊？
read -p "Please input a directory: " dir
if [ "${dir}" == "" -o ! -d "${dir}" ]; then
	echo "The ${dir} is NOT exist in your system."
	exit 1
fi

# 2. 開始測試檔案囉～
filelist=$(ls ${dir})        # 列出所有在該目錄下的檔案名稱
for filename in ${filelist}
do
	perm=""
	test -r "${dir}/${filename}" && perm="${perm} readable"
	test -w "${dir}/${filename}" && perm="${perm} writable"
	test -x "${dir}/${filename}" && perm="${perm} executable"
	echo "The file ${dir}/${filename}'s permission is ${perm} "
done
```

## \$()、\${}、\$(())的区别

\$()与\` \`都是用来做命令替换用。所谓的命令替换是用来重组命令行的。完成引号/括号里的命令行，然后将其结果替换出来，再重组命令行。

```bash
$ echo the last sunday is $(date -d "last sunday" +%Y-%m-%d)
#如此便可方便得到上一星期天的日期了
```

\${ } 用来作变量替换。

\$(( )) 是用来作整数运算的

## shell script 的追蹤與 debug

`sh [-nvx] scripts.sh`

```
選項與參數：
-n  ：不要執行 script，僅查詢語法的問題；
-v  ：再執行 sccript 前，先將 scripts 的內容輸出到螢幕上；
-x  ：將使用到的 script 內容顯示到螢幕上，這是很有用的參數！
```