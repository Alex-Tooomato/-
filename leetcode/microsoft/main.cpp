#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

int main() {
    vector<int> A={-3,-2,1,0,8,7,1};
    int M=3;
    if(A.size()==0)return 0;
    vector<vector<int>> ans;
    unordered_map<int,int> points;
    for(int i=0;i<A.size();++i){
        points[A[i]]=i;
    }
    sort(A.begin(),A.end());
    for (int & k:A){
        cout<<k<<endl;
    }
    for(int i=0;i<A.size();++i){
        for(int j=i+1;j<A.size();++j){
            if((A[j]-A[i])%M==0){
                ans.push_back({A[j],A[i]});
            }
        }
    }
    for(auto & p:ans){
        cout<<p[0]<<","<<p[1]<<endl;
    }
    return 0;
}
