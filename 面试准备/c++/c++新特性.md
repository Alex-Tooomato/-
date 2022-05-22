# c++ 11新特性

1. auto

2. decltype

3. lambda

   编译器会为lambda表达式产生一个类，以实现具体功能

   ```c++
   //源函数
   int main() {
     int x = 5;
     int y = 6;
     auto fun = [=]() { printf("%d, %d\n", x, y); };
     fun();
     return 0;
   }
   //编译后
   class __lambda_9_14
     {
       public: 
       inline /*constexpr */ void operator()() const
       {
         printf("%d, %d\n", x, y);
       }
       
       private: 
       int x;
       int y;
       
       public:
       __lambda_9_14(int & _x, int & _y)
       : x{_x}
       , y{_y}
       {}   
     };
   //对于[=]捕获方式，如果函数体内没有使用的变量，编译器不会生成对应的成员变量。
   ```

   operator()函数默认为const 也就是不能对成员进行改变，如果想改变，需要加上mutable

   ```c++
   auto fun = [=]()mutable { printf("%d, %d\n", x, y); };
   ```

   **生成规则**

   编译器对lambda的生成规则如下：

   - lambda表达式中的**捕获列表**，对应lambda_xxxx类的**private 成员**
   - lambda表达式中的**形参列表**，对应lambda_xxxx类成员函数 **operator()的形参列表**
   - lambda表达式中的**mutable**，对应lambda_xxxx类成员函数 **operator() 的常属性 const**，即是否是**常成员函数**
   - lambda表达式中的**返回类型**，对应lambda_xxxx类成员函数 **operator() 的返回类型**
   - lambda表达式中的**函数体**，对应lambda_xxxx类成员函数 **operator() 的函数体**

4. 范围 `for` 语句

5. 右值引用

   *左值引用在汇编层面其实和普通的指针是一样的；*定义引用变量必须初始化，因为引用其实就是一个别名，需要告诉编译器定义的是谁的引用。

   在汇编层面右值引用做的事情和常引用是相同的，即产生临时量来存储常量。但是，唯一 一点的区别是，右值引用可以进行读写操作，而常引用只能进行读操作。

   右值引用可以减少深拷贝是创建新堆空间的消耗，而是直接使用拷贝的空间，拷贝完原指针置空

6. 标准库 `move()` 函数

7. 智能指针

8. `delete` 函数和 `default` 函数

   - `delete` 函数：`= delete` 表示该函数不能被调用。
   - `default` 函数：`= default` 表示编译器生成默认的函数，例如：生成默认的构造函数。

