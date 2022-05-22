#include <unordered_map>
#include <vector>
#include <iostream>
using namespace std;
template <typename T>
class BST{
    struct Node{
        T data;
        Node * left;
        Node * right;
    };

    Node * root;

    Node * makeEmpty(Node * t){
        if(t==NULL) return NULL;
        makeEmpty(t->left);
        makeEmpty(t->right);
        return t;
    }

    Node * insert(Node * t,T x){
        if(t==NULL) {
            t = new Node;
            t->val = x;
            t->left = NULL;
            t->right = NULL;
        }else if(x<t->data){
            t->left= insert(t->left,x);
        }else if(x>t->data){
            t->right= insert(t->right,x);
        }
        return t;
    }
    Node * find(Node * t,T x){
        if(t==NULL) return NULL;
        if(x<t->data) return find(t->left,x);
        if(x>t->data) return find(t->right,x);
        return t;
    }

    Node * findMin(Node * t){
        if(t==NULL||t->left==NULL) return t;
        return findMin(t->left);
    }

    Node * findMax(Node * t){
        if(t==NULL||t->left==NULL) return t;
        return findMax(t->right);
    }

    Node * remove(Node * t, T x){
        if(t==NULL) return NULL;
        else if (x < t->data) t->left = remove(t->left,x);
        else if (x > t->data) t->right = remove(t->right,x);
        else if (t->left&&t->right){

        }else{
            Node * temp = t;
            if(t->left==NULL) t=t->right;
            else if (t->right==NULL) t=t->left;
            delete temp;
        }
        return t;
    }


public:
    BST():root(NULL){}

    ~BST(){
        root=makeEmpty(root);
    }

    void insert(T x){
        insert(root,x);
    }
    void remove(T x){
        remove(root,x);
    }
};
int main()
{

}