#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <set>
#include <cstring>
#include <fstream>
#include <iterator>
#include <algorithm>

using namespace std;

//vector <string> T = {"+", "*", "(", ")","i"};
vector <string> T = {"key", "id", "const", "limiter", "operator","@"};
vector <string> VT;

map <string, set<string> > first;
map <string, set<string> > follow;

vector<string > LEFT;
vector<vector<string> > RIGHT;

set<string> unIon(set<string> a, set<string> b){

    set<string> t;
    //set_intersection(a.begin(),a.end(),b.begin(),b.end(),inserter(t,t.end()));
    for(auto i : a)t.insert(i);
    for(auto i : b)t.insert(i);

    return t;
}

set<string> findFirst(string a, bool &isEp){
    if(a == "@"){
        isEp = true;
    }
    if(find(T.begin(), T.end(),a) != T.end()){
        return first[a];
    }
    set<string> ans;
    for(int i = 0;i < LEFT.size();i++){
        if(LEFT[i] == a){
            // cout<<"search:"<<RIGHT[i][0]<<endl;
            ans = unIon(ans, findFirst(RIGHT[i][0], isEp));
        }
    }
    return ans;
}

void initFirst(){
    for(string i : VT){
        set<string> t;
        follow[i] = t;
    }
    for(string i : T){
        set<string> f;
        f.insert(i);
        first[i] = f;
    }
    set<string> temp;
    temp.insert("@");
    first["@"] = temp;
    for(string i : VT){
        set<string> t;
        first[i] = t;
    }

}

void getFirst(){

    for(int i = 0;i < LEFT.size();i++){
        auto left = LEFT[i];
        auto right = RIGHT[i];
        cout<<left<<" -> ";
        for(string i : right){
            cout<<i;
        }
        cout<<endl;
        bool isEp = false;

        for(auto j = right.begin(); j <= right.end();j++){
            if(j == right.end()){
                first[left].insert("@");
                break;
            }
            first[left] = unIon(first[left], findFirst(*j, isEp));
            // cout<<left<<":";
            // for(auto i : first[left])cout<<i<<" ";
            // cout<<endl;
            if(isEp == false){
                break;
            }
        }
        
    }
}

void printFollow(){
    for(auto i : follow){
        cout<<"follow["<<i.first<<"]:{";
        for(auto j : i.second){
            cout<<j<<", ";
        }
        cout<<"}"<<endl;
    }
}

void getFollow(){
    
    follow[LEFT[0]].insert("#");
    for(int i = 0;i < LEFT.size(); i++){
        printFollow();
        auto left = LEFT[i];
        auto right = RIGHT[i];


        cout<<left<<" -> ";
        for(string i : right){
            cout<<i;
        }
        cout<<endl;

        for(auto j = right.begin();j != right.end(); j++){
            if(find(T.begin(), T.end(),*j) != T.end() || *j == "@"){
                continue;
            }
            bool isEp = false;
            for(auto k = j + 1; k <= right.end();k++){
                if(k == right.end()){
                    follow[*j] = unIon(follow[*j], follow[left]);
                    break;
                }else{
                    follow[*j] = unIon(follow[*j], findFirst(*k ,isEp));
                }
                if(isEp == false){
                    break;
                }
            }
        }
    }
}
void printTable();

map <string,map<string,int> >table;
void getTable(){
    for(int i = 0;i < LEFT.size();i++){
        cout<<i<<':'<<LEFT[i];
        auto left = LEFT[i];
        auto right = RIGHT[i];
        for(string xx:right){
            cout<<xx;
        }
        if(right[0] == "@"){
            auto flw = follow[left];
            for(auto j: flw){
                table[left][j] = i;
            }
        }
        auto temp = first[right[0]];
        for(auto j: temp){
            if(j != "@"){
                table[left][j] = i;
            }else{
                auto flw = follow[right[0]];
                cout<<right[0]<<", ";
                for(auto k: flw){
                    table[left][k] = i;
                }
            }
        }
        cout<<endl;
        printTable();
    }
}

vector <string> word;

bool run(){
    stack<string> s;
    s.push("#");
    s.push(LEFT[0]);
    int i = 0;
    while(!s.empty()){
        string t = s.top();
        s.pop();
        cout<<"pop:"<<t<<endl;
        cout<<"cur:"<<word[i]<<endl;
        if(t == word[i]){
            i++;
        }else if(t == "#"){
            return true;
        }else if(table.count(t) && table[t].count(word[i]) != 0 ){
            int k = table[t][word[i]];
            cout<<LEFT[k]<<"->"<<k;
            //for(auto xx: RIGHT[k])cout<<xx;
            cout<<endl;
            for(int temp = RIGHT[k].size()-1 ;temp >= 0;temp--){
                if(RIGHT[k][temp] != "@")
                s.push(RIGHT[k][temp]);
            }
        }else {
            cout<<"error:"<<t;
            while(!s.empty()){
                string t = s.top();
                cout<<t;
                s.pop();
            }
            return false;
        }
    }
    return true;
}

void printTable(){
    for(auto i:table){
        cout<<i.first<<'\t';
        for(auto j:i.second){
            cout<<j.first<<':'<<j.second<<'\t';
        }
        cout<<endl;
    }
}


int main(int argc,char **argv){
    if(argc != 3){
        cout<<"need input"<<endl;
        return -1;
    }

    cout<<"[*]read token list"<<endl;
    ifstream fToken;
    vector <string> token;
    fToken.open(argv[1], ios::in);
    while(fToken){
        string t;
        fToken>>t;
        if(t.find("(") != t.npos){
            t = t.replace(t.find("(") , 1, "");
            t = t.replace(t.find(","), 1, "");
            cout<<t<<endl;
            token.push_back(t);
        }
    }
    token.push_back("#");
    word = token;

    cout<<"[*]read grammar"<<endl;
    ifstream fGrammar;
    fGrammar.open(argv[2], ios::in);
    while(fGrammar.peek() != '\n'){
        string t;
        fGrammar>>t;
        cout<<t<<endl;
        VT.push_back(t);
    }

    bool isLeft = true;
    string l;
    vector <string> r;
    while(fGrammar){
        string t; 
        fGrammar>>t;
        // cout<<t<<endl;
        if(t == "->"){
            isLeft = false;
            continue;
        }
        if(isLeft){
            l = t;
        }else{
            r.push_back(t);
        }
        if(fGrammar.peek() == '\n'){
            isLeft = true;
            LEFT.push_back(l);
            RIGHT.push_back(r);
            l.clear();
            r.clear();
        }
    }

    for(int i = 0;i < LEFT.size(); i++){
        cout<<"left:"<<LEFT[i]<<endl;
        for(string i : RIGHT[i]){
            cout<<i<<endl;
        }
    }
    
    initFirst();

    getFirst();


    auto pre = follow;
    while(1){
        getFollow();
        if(pre == follow){
            break;
        }else{
            pre = follow;
        }
    }
    for(auto i : first){
        cout<<"first["<<i.first<<"]:{";
        for(auto j : i.second){
            cout<<j<<", ";
        }
        cout<<"}"<<endl;
    }

    for(auto i : follow){
        cout<<"follow["<<i.first<<"]:{";
        for(auto j : i.second){
            cout<<j<<", ";
        }
        cout<<"}"<<endl;
    }
    getTable();
    // while(cin.peek() != '\n'){
    //     string t;
    //     cin>>t;
    //     word.push_back(t);
    // }
    // word.push_back("#");
    cout<<run();
    return 0;
}

