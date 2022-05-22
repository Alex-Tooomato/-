# offer突击001

1. 首先应想到用减法代替除法。

   b被a减了多少次，除数就是多少，最后一个正数或0就是余数。

   除法得出的正负与除数和被除数的符号有关，可以提前确定，接着将两数都取正值

2. 考虑到int越界的问题，有符号数int的范围-2147483648到2147483647

   考虑到负数的正值可能会越界，但正数的负值不会，因此将取abs改成正值取反

3. 每次都减去被除数可能导致超时，可以减去被除数的两倍，如果小于0则回退继续减被除数

4. 用按位运算，减小复杂度(除数乘2)

```c++
class Solution {
public:
    int divide(int a, int b) {
        //越界考虑
        //32位最大值：2^31-1=2147483647;
        //32位最小值：-2^31 =-2147483648;
        //-2147483648/(-1)=2147483648>2147483647;
        if(a==INT32_MIN&&b==-1)
            return INT32_MAX;
        int sign=(a>0)^(b>0)?-1:1;
        //考虑到负数的正值可能会越界，但正数的负值不会，因此将取abs改成正值取反
//        if(a>0)a=-a;
//        if(b>0)b=-b;
        unsigned int n=0;
//        while(a<=b){
//            int value=b;
//            // 如果不用 unsigned 的话，那么当 a = -2147483648, b = 1 的时候，k 会越界
//            unsigned int k=1;
//            // 0xc0000000 是十进制 -2^30 的十六进制的表示
//            // 判断 value >= 0xc0000000 的原因：保证 value + value 不会溢出
//            // 可以这样判断的原因是：0xc0000000 是最小值 -2^31 的一半，
//            // 而 a 的值不可能比 -2^31 还要小，所以 value 不可能比 0xc0000000 小
//
//            while(value>=0xc0000000&&a<=value+value){
//                value+=value;
//                k+=k;
//            }
//            a-=value;
//            n+=k;
//        }

//按位运算，较小复杂度
unsigned int ua=abs(a);
unsigned int ub=abs(b);
for(int i=31;i>=0;i--){
    if((ua>>i)>=ub){//a>=b<<i;防止b越界，于是改成a>>i>=b
        ua=ua-(ub<<i);
        n+=1<<i;
    }
}
        return sign == 1 ? n : -n;
    }
};
```



# offer突击002

### 要点

1. 「**加法**」系列题目都不难，其实就是 **「列竖式」模拟法**。

「列竖式」模拟法就是模仿十进制数加法的计算过程。

### 十进制加法

我们先回顾一下十进制加法的计算过程，红点表示进位：

<img src="https://pic.leetcode-cn.com/1634376064-AtRiWA-%E5%8A%A0%E6%B3%95.001.jpeg" alt="加法.001.jpeg" style="zoom: 50%;" />

**使用「竖式」计算十进制的加法的方式：**

1. 两个「**加数**」的右端对齐；
2. 从最右侧开始，依次计算对应的两位数字的和。如果和大于等于 10，则把和的个位数字计入结果，并向前面进位。
3. 依次向左计算对应位置两位数字的和，如果有进位需要加上进位。如果和大于等于 10，仍然把和的个位数字计入结果，并向前面进位。
4. 当两个「加数」的每个位置都计算完成，如果最后仍有进位，需要把进位数字保留到计算结果中。

### 二进制加法

二进制加法的计算也可以采用类似的方法，与十进制不同的是，**二进制的进位规则是「逢二进一」**，即当求和结果 >= 2>=2 时，需要向前进位。

- 红点表示进位。

<img src="https://pic.leetcode-cn.com/1634377045-ydzKJa-%E5%8A%A0%E6%B3%95.002.jpeg" alt="加法.002.jpeg" style="zoom:50%;" />

```c++
string ans;
reverse(a.begin(), a.end());
reverse(b.begin(), b.end());

int n = max(a.size(), b.size()), carry = 0;
for (size_t i = 0; i < n; ++i) {
    carry += i < a.size() ? (a.at(i) == '1') : 0;
    carry += i < b.size() ? (b.at(i) == '1') : 0;
    ans.push_back((carry % 2) ? '1' : '0');
    carry /= 2;
}

if (carry) {
    ans.push_back('1');
}
reverse(ans.begin(), ans.end());

return ans;
```