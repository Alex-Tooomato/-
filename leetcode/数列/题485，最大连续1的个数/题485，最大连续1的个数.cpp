// 题485，最大连续1的个数.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int findMaxConsecutiveOnes(vector<int>& nums) {
        int maxnum = 0, cnt = 0;
        for (int i = 0; i < nums.size(); i++) {
            if (nums[i] == 1)
            {
                cnt++;
            }
            else
            {
                maxnum = max(maxnum,cnt);
                cnt = 0;
            }
                
        }
        maxnum = max(maxnum, cnt);
        return maxnum;
       
    }

    int findMax(vector<int>& nums) {
        if (nums.size() == 0)
            return 0;
        int result = nums[0];
        for (int i = 1; i < nums.size(); i++) {
            if (nums[i] == 1)
                nums[i] = nums[i - 1] + 1;
            else
                nums[i] = 0;
            result = max(result, nums[i]);
        }
        return result;
    }
};

int main()
{   
    Solution solution1;
    vector<int> num { 1,1,0,1,1,1 };
    int count= solution1.findMaxConsecutiveOnes(num);
    cout << "count = " << count << endl;
    int max = solution1.findMax(num);
    cout << "max = " << max << endl;
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
