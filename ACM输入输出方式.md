# C++输入输出基础知识：

- C++语言未定义任何输入输出语句，而是使用标准库（standard library）来提供IO机制。

- ```c++
  #include<iostream> 
  cin >> 
  cout << 
  ```

- 标准输入输出对象

  - cin 标准输入(istream)
  - cout 标准输出（ostream对象）
  - cerr 输出警告和错误信息
  - clog 输出程序运行时的一般性信息

- **C++输入的每个字符串 由 空白（空格符 /换行符/制表符间）隔开**，**文件结束标识符为crtl+Z(windows)，control+D(linux)**
  https://blog.csdn.net/Eddie_burning/article/details/51493523

- 两种常用的输入模式：

  ![img](https://img-blog.csdnimg.cn/eb68f3c6c56c462088e87cc69994a732.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA6aOO6L-Y5aW95Ya3,size_20,color_FFFFFF,t_70,g_se,x_16)

- cin>> 语句读取用户输入的数据时，它会在遇到换行符时停止。换行字符未被读取，而是仍保留在键盘缓冲区中。从键盘读取数据的输入语句只在键盘缓冲区为空时等待用户输入值，但现在不为空。
- cin 遇空格或换行，会停止识别，如果你打算输入的字符串中带1个或多个空格，则采用getline把停止识别的符号设置为‘\n’（即换行符），就能正确输入输出了。
- cin.get 函数执行时，它开始从先前输入操作停止的键盘缓冲区读取，并发现了换行符，所以它无须等待用户输入另一个值。
- 读取string对象时，string对象会自动忽略开头处的空白（即空格符、换行符、制表符等），并从第一个真正的字符开始读起，直到遇见下一处空白为止，认为为一个字符串。

# [ACM](https://so.csdn.net/so/search?q=ACM&spm=1001.2101.3001.7020)模式输入输出参考程序

```c++
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<algorithm>
#include<limits.h>  //INT_MIN 和 INT_MAX的头文件  

using namespace std;

struct stu {
	string name;
	int num;
};


// 1. 直接输入一个数
int main() {
	int n = 0;
	while (cin >> n) { 
		cout << n << endl;
	}
	return -1;
}

// 2. 直接输入一个字符串
int main() {
	string str;
	while (cin >> str) {
		cout << str << endl;
	}
	return -1;
}

// 3. 只读取一个字符 
int main() {
	char ch;
	//方式1
	while (cin >> ch) {
		cout << ch << endl;
	}
	//方式2： cin.get(ch) 或者 ch = cin.get() 或者 cin.get()
	while (cin.get(ch)) {   
		cout << ch << endl;
	}
	//方式3 ：ch = getchar()  
	while (getchar()) {
		cout << ch << endl;
	}
	return -1;
}


// 3.1给定一个数，表示有多少组数（可能是数字和字符串的组合），然后读取
int main() {
	int n = 0; 
	while (cin >> n) {   //每次读取1 + n 个数，即一个样例有n+1个数 
		vector<int> nums(n);
		for (int i = 0; i < n; i++) {
			cin >> nums[i];
		}
		//处理这组数/字符串
		for (int i = 0; i < n; i++) {
			cout << nums[i] << endl;
		}
	}
	return -1;
}

//3.2 首先给一个数字，表示需读取n个字符，然后顺序读取n个字符
int main() {
	int n = 0;
	while (cin >> n) {  //输入数量
		vector<string> strs;
		for (int i = 0; i < n; i++) {
			string temp;
			cin >> temp;
			strs.push_back(temp);
		}
		//处理这组字符串
		sort(strs.begin(), strs.end());
		for (auto& str : strs) {
			cout << str << ' ';
		}
	}
	return 0;
}


//4.未给定数据个数，但是每一行代表一组数据，每个数据之间用空格隔开
//4.1使用getchar() 或者 cin.get() 读取判断是否是换行符，若是的话，则表示该组数（样例）结束了，需进行处理
int main() {
	int ele;
	while (cin >> ele) {
		int sum = ele;
		// getchar()   //读取单个字符
		/*while (cin.get() != '\n') {*/   //判断换行符号
		while (getchar() != '\n') {  //如果不是换行符号的话，读到的是数字后面的空格或者table
			int num;
			cin >> num;
			sum += num;
		}
		cout << sum << endl;
	}
	return 0;
}

//4.2 给定一行字符串，每个字符串用空格间隔，一个样例为一行
int main() {
	string str;
	vector<string> strs;
	while (cin >> str) {
		strs.push_back(str);
		if (getchar() == '\n') {  //控制测试样例
			sort(strs.begin(), strs.end());
			for (auto& str : strs) {
				cout << str << " ";
			}
			cout << endl;
			strs.clear();
		}
	}
	return 0;
}


//4.3 使用getline 读取一整行数字到字符串input中，然后使用字符串流stringstream，读取单个数字或者字符。
int main() {
	string input;
	while (getline(cin, input)) {  //读取一行
		stringstream data(input);  //使用字符串流
		int num = 0, sum = 0;
		while (data >> num) {
			sum += num;
		}
		cout << sum << endl;
	}
	return 0;
}


//4.4 使用getline 读取一整行字符串到字符串input中，然后使用字符串流stringstream，读取单个数字或者字符。
int main() {
	string words;
	while (getline(cin, words)) {
		stringstream data(words);
		vector<string> strs;
		string str;
		while (data >> str) {
			strs.push_back(str);
		}
		sort(strs.begin(), strs.end());
		for (auto& str : strs) {
			cout << str << " ";
		}
	}
}

//4.5 使用getline 读取一整行字符串到字符串input中，然后使用字符串流stringstream，读取单个数字或者字符。每个字符中间用','间隔
int main() {
	string line;
	
	//while (cin >> line) {  //因为加了“，”所以可以看出一个字符串读取
	while(getline(cin, line)){
		vector<string> strs;
		stringstream ss(line);
		string str;
		while (getline(ss, str, ',')) {
			strs.push_back(str);
		}
		//
		sort(strs.begin(), strs.end());
		for (auto& str : strs) {
			cout << str << " ";
		}
		cout << endl;
	}
	return 0;
}



int main() {
	string str;

	
	//C语言读取字符、数字
	int a;
	char c;
	string s;

	scanf_s("%d", &a);
	scanf("%c", &c);
	scanf("%s", &s);
	printf("%d", a);


	//读取字符
	char ch;
	cin >> ch;
	ch = getchar();
	while (cin.get(ch)) { //获得单个字符
		;
	}
	
	//读取字符串
	cin >> str;  //遇到空白停止
	getline(cin, str);  //读入一行字符串

}

```

