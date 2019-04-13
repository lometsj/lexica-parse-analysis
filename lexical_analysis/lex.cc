#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include "lex.h"
//Lometsj
using namespace std;


const int E = 100000;//max edge 
const int V = 100000;//max vertex
map<char, int> PRIORITY;
char ALAPHABET[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 
'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ',', ';', '+', '-', '[', ']'};

struct Edge{
    int to; 
    char w; //weight:[a-z][0-9]
    int next;   //point to next edge
};
Edge edge[E];
int head[V];

bool isOp(char op){
    return op=='|' || op=='&' || op=='*';
}

bool isSym(char t){
    return isOp(t) || t=='(' || t==')';
}


bool opCmp(char a, char b){
    // cout<<PRIORITY[a]<<' '<<PRIORITY[b]<<endl;
    return PRIORITY[a] < PRIORITY[b];
}

bool isAlapha(char a){
    return (a>=48 && a<=57) || (a>=65&&a<=122);
}

string Infix2Postix(string infixExpr){
    stack <char> symStack;
    string postfixExpr;
    for(char i: infixExpr){
        // cout<<"now:"<<i<<endl;
        if(isSym(i)){
            if(symStack.empty() || symStack.top() == '(' || i == '(')
                symStack.push(i);
            else{
                if(i == ')'){
                    // cout<<"in"<<endl;
                    // cout<<symStack.top()<<endl;
                    while(symStack.top() != '('){
                        // cout<<"pop:" <<symStack.top()<<endl;
                        // cout<<"size:"<<symStack.size()<<endl;
                        postfixExpr.push_back(symStack.top());
                        symStack.pop();
                    }
                    symStack.pop();
                    // cout<<"out"<<endl;
                }else{
                    bool flag = false;
                    while(!symStack.empty()){
                        //cout<<"compare:"<<symStack.top()<<' '<<i<<endl;
                        if(opCmp(symStack.top(),i)){
                            symStack.push(i);
                            flag = true;
                            break;
                        }
                        else{
                            if(symStack.top() == '(')
                                break;
                            postfixExpr.push_back(symStack.top());
                            symStack.pop();
                        }
                    }
                    if(!flag){
                        symStack.push(i);
                    }
                }
            }
        }else{
            postfixExpr.push_back(i);
        }
    }
    while(!symStack.empty()){
        postfixExpr.push_back(symStack.top());
        symStack.pop();
    }
    return postfixExpr;
}

vector <string> addConcat(vector<string> expr){
    vector <string> ret = expr;
    for(int i = 0;i < ret.size(); i++){
        string t = expr[i];
        for(int j = 0; j < t.length() - 1;j++){
            if(t.length() < 2)break;
            char next = t[j+1];
            char now = t[j];
            if(now!='&' && now!='(' && now!='|' && next!='|' && next!=')' && next!='*'){
                t.insert(j+1,1,'&');
                j++;
            }
        }
        ret[i] = t;
    }
    return ret;
}


int idxE = 0; 
int idxV = 1;
int start = 0;

void addEdge(int u, int v, char w){
    cout<<"("<<u<<", "<<v<<", "<<w<<')'<<endl;
    edge[idxE].to = v;
    edge[idxE].w = w;
    edge[idxE].next = head[u];
    head[u] = idxE;
    idxE++;
}

struct Graph{
    int u;
    int v;
    Graph(int u,int v){
        this->u = u;
        this->v = v;
    }
};

Graph buildNfa(string expr){
    cout<<"nfa"<<endl;
    stack<Graph> gStack;
    string t = expr;
    for(char i : expr){
        if(isOp(i)){
            if(i == '&'){
                Graph right  = gStack.top();
                gStack.pop();
                Graph left = gStack.top();
                gStack.pop();

                int from = left.u;
                int to = right.v;

                addEdge(left.v, right.u, ' ');
                gStack.push(Graph(from, to));
            }else if(i == '|'){
                Graph left  = gStack.top();
                gStack.pop();
                Graph right = gStack.top();
                gStack.pop();

                int from = idxV++;
                int to = idxV++;

                addEdge(from, right.u, ' ');
                addEdge(from, left.u, ' ');
                addEdge(right.v, to, ' ');
                addEdge(left.v, to, ' ');
                gStack.push(Graph(from, to));
            }else if(i == '*'){
                Graph t = gStack.top();
                gStack.pop();

                int from = idxV++;
                int to = idxV++;

                addEdge(t.v, t.u, ' ');
                addEdge(from, t.u, ' ');
                addEdge(t.v, to, ' ');
                addEdge(from, to, ' ');
                gStack.push(Graph(from, to));
            }
        }else{
            int from = idxV++;
            int to = idxV++;
            addEdge(from, to, i);
            Graph graph(from, to);
            gStack.push(graph);
        }
    }
    return gStack.top();
}

Edge edgeDfa[100000];
int idxDfaV = 1;
int idxDfaE = 0;
int headDfa[100000];

set<int> EpsilonClosure(int x){
    set<int> res;
    queue<int> q;
    q.push(x);
    bool vis[idxV];
    memset(vis, false, sizeof(vis));
    while(!q.empty()){
        int i = q.front();
        q.pop();
        res.insert(i);
        // cout<<"extend :"<<i<<endl;
        for(int j = head[i]; j != -1; j = edge[j].next){
            // cout<<edge[j].to<<endl;
            // cout<<vis[edge[j].to]<<endl;
            if(edge[j].w == ' ' && vis[edge[j].to] == false && res.count(edge[j].to)==0){
                res.insert(edge[j].to);
                // for(int tt :res)cout<<tt<<',';
                // cout<<endl;
                // cout<<vis[edge[j].to]<<endl;
                // cout<<i<<','<<edge[j].to<<endl;
                q.push(edge[j].to);
                vis[edge[j].to] = true;
                // cout<<vis[edge[j].to]<<endl;
            }
        }
    }
    return res;
}

void addDfaEdge(int u, int v, char w){
    cout<<"("<<u<<", "<<v<<", "<<w<<')'<<endl;
    edgeDfa[idxDfaE].to = v;
    edgeDfa[idxDfaE].w = w;
    edgeDfa[idxDfaE].next = headDfa[u];
    headDfa[u] = idxDfaE;
    idxDfaE++;
}

int findIndex(vector<set<int> >a, set<int> b){
    return distance(a.begin(), find(a.begin(), a.end(), b));
}

vector< set<int> > nfa2dfa(){
    vector< set<int> > states;
    states.push_back(EpsilonClosure(start));
    queue< set<int> > q;
    q.push(states[0]);
    int from = 0;
    while(!q.empty()){
        set<int> i = q.front();
        q.pop();
        for(char j : ALAPHABET){
            set<int> k;
            for(int l: i){
                for(int temp = head[l]; temp != -1; temp = edge[temp].next){
                    if(edge[temp].w == j)
                        k.insert(edge[temp].to);
                }
            }
            if(k.size() != 0){
                for(int temp: k){
                    // cout<<"extend epsil:"<<temp<<endl;
                    set<int> ec = EpsilonClosure(temp);
                    k.insert(ec.begin(), ec.end());
                    // for(int x : k)cout<<x<<", ";
                    // cout<<endl;
                }
            }else{
                continue;
            }
            bool flag = true;
            for(auto temp: states){
                if(temp == k){
                    flag = false;
                    break;
                }
            }
            if(flag){
                // int to = states.size();
                states.push_back(k);
                // int to = idxDfaV;
                // idxDfaV++;
                // addDfaEdge(from, to, j);
                q.push(k);
            }
            int to = findIndex(states, k);
            addDfaEdge(from, to, j);
        }
        from++;

    }
    return states;
}

string runDfa(string token, map<int, string> terminalState){
    //cout<<token<<endl;
    int now = 0;
    for(char i : token){
        for(int j = headDfa[now]; j!=-1;j=edgeDfa[j].next){
            if(edgeDfa[j].w == i){
                //cout<<"to:"<<edgeDfa[j].to<<endl;
                now = edgeDfa[j].to;
            }
        }
    }
    if(terminalState.count(now)==1){
        return string("("+terminalState[now]+", "+token+")");
    }
    else{
        return string("(error,"+token+")");
    }
}

void init(){
    PRIORITY['|'] = 1;
    PRIORITY['&'] = 2;
    PRIORITY['*'] = 3;
    PRIORITY['('] = 4;
    memset(head, -1, sizeof(head));
    memset(headDfa, -1, sizeof(headDfa));

}

int main(int argc, char **argv) {
    //preInit :read re_expr and add '&' 
    init();
	if(argc < 2) {
		puts("need input.");
		return -1;
	}
    vector <string> name;
    vector <string> expr;
	// get input
	ifstream fRe;
    fRe.open(argv[1], ios::in);
    while(fRe){
        string a, b;
        fRe>>a>>b;
        name.push_back(a);
        expr.push_back(b);
    }

    expr = addConcat(expr);
    for(string i: expr){
        // cout<<i<<endl;
        cout<<Infix2Postix(i)<<endl;
    }
    cout<<"[*]preInit succeed."<<endl;

    //infix2postfix
    vector<string> post;
    for(string i: expr){
        post.push_back(Infix2Postix(i));
    }
    cout<<"[*]convert infix_expr to postfix"<<endl;

    cout<<"[*]build nfa"<<endl;

    map<int, string> token;
    int j = 0;
    for(string i :post){
        if(i.length() < 2)break;
        Graph dfa = buildNfa(i);
        cout<<"graph:"<<'('<<dfa.u<<", "<<dfa.v<<")"<<endl;
        addEdge(start, dfa.u, ' ');
        token[dfa.v] = name[j++];
    }
    for(auto &i : token){
        cout<<i.first<<endl;
        cout<<i.second<<endl;
    }

    map<int, string> terminalState;
    vector< set<int> > states = nfa2dfa();
    cout<<"[*]nfa2dfa complete."<<endl;

    for(int i = 0;i<states.size();i++){
        for(int j :states[i]){
            if(token.count(j)==1){
                terminalState[i] = token[j];
            }
        }
    }
    for(auto &i : terminalState){
        cout<<i.first<<" "<<i.second<<endl;
    }
    ifstream fSrc;
    fSrc.open(argv[2],ios::in);
    while(fSrc){
        string t;
        fSrc>>t;
        // cout<<t<<endl;
        if(t== "")continue;
        cout<<runDfa(t, terminalState)<<endl;
    }
    return 0;
}