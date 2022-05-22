// 链表项目.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
using namespace std;
/*链表题203. 移除链表元素*/
struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
    
};

class Solution {
public:
    /*链表题203. 移除链表元素*/
    //法一：递归
    ListNode* removeElements(ListNode* head, int val) {
        if (head == nullptr) {
            return head;
        }
        head->next = removeElements(head->next, val);
        return head->val == val ? head->next : head;    
    }
    //法二：迭代
    ListNode* removeElements2(ListNode* head, int val) {
        struct ListNode* dummyHead = new ListNode(0, head);
        struct ListNode* temp = dummyHead;
        while (temp->next != NULL) {
            if (temp->next->val == val) {
                temp->next = temp->next->next;
            }
            else {
                temp = temp->next;
            }
        }
        return dummyHead->next;
    }
    /*链表题206. 反转链表*/
    //法一：递归
    ListNode* reverseList(ListNode* head) {
        if (!head || !head->next) {
            return head;
        }
        ListNode* newhead = reverseList(head->next);
        head->next->next = head;
        head->next = NULL;
        return newhead;
    }
    //法二：迭代
    ListNode* reverseList2(ListNode* head) {
        ListNode* temp = head;
        ListNode* pre = NULL;
        while (temp) {
            ListNode* next = temp->next;
            temp->next = pre;
            pre = temp;
            temp = next;
        }
        return pre;
    }

        
};

int main()
{
    Solution S1;
    ListNode Node[5];
    Node[4].val = 1;
    Node[4].next = NULL;
    Node[3].val = 2;
    Node[3].next = &Node[4];
    Node[2].val = 3;
    Node[2].next = &Node[3];
    Node[1].val = 4;
    Node[1].next = &Node[2];
    Node[0].val = 5;
    Node[0].next = &Node[1];
    for (int i = 0; i < 5; i++)
        cout << "List[" << i << "] : " << Node[i].val << " " << Node[i].next << endl;
    ListNode* W = S1.removeElements(Node, 4);
    while (W->next != NULL) {
        cout << W->val <<" " << W->next << endl;
        W = W->next;
    }
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
