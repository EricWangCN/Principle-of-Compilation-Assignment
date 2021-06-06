#include<bits/stdc++.h>

using namespace std;

typedef struct vn
{
    string Key = " ";
    int Seq = 0;
    int Func_num = 0;
    int *Funcs = NULL;
    int *first = NULL;   //以-1为结束标志
    int *follow = NULL;  //以-1为结束标志
}VN;

typedef struct Glo
{
    VN* VNs;
    VN* VTs;

}GLO;

typedef struct func //string的Key， Seq数组， 长度， 编号从0开始
{

    string Keys;
    int *Seqs;
    int len;
    int num;
    func(string keys = " ", int *seqs = NULL, int l = 0, int n = 0) :Keys(keys), Seqs(seqs), len(l), num(n) {};
}FUNC;

GLO* glo = new GLO;

VN vns[] = { {"S'", 1}, {"S", 2}, {"V", 3}, {"E", 4}, {"T", 5}, {"F", 6} };
VN vts[] = { {"=", 7}, {"+", 8}, {"-", 9}, {"*", 10}, {"/", 11}, {"(", 12},
             {")", 13}, {"i", 14}, {"#", 15}, {"0", 16} };

//每个导出式右部的字符串,以及对应的符号序号将在initial中链接， “0”为空导出式, 只有1，2,3,5,6会产生四元式.
FUNC func[] = { {"S"}, {"V=E"}, {"E+T"}, {"E-T"}, {"T"}, {"T*F"}, {"T/F"}, {"F"}, {"(E)"}, {"i"}, {"0"} };

//每个导出式右部的序号数组,以0为一个右部的结束标志,如TE' 为3，2，0, 里面的数字对应vns和vnt中的序号
int func_seq[][10] = { {2, 0}, {3, 7, 4, 0}, {4, 8, 5, 0}, {4, 9, 5, 0}, {5, 0}, {5, 10, 6, 0}, {5, 11, 6, 0},
                       {6, 0}, {12, 4, 13, 0}, {14, 0}, {16, 0} };

//每个非终结符号包含的所有导出式的序号， 里面的数字对应func_seq中的数组序号，如 1 对应func[0], 2 对应func[1]
int func_vn[][10] = { {1, 0}, {2, 0}, {10, 0}, {3, 4, 5, 0}, {6, 7, 8, 0}, {9, 10, 0} };

typedef struct form //func, vn, tag, finish
{
    int vn; //产生式左部的非终结符号编号

    int tag;// 原点初始在最左边
    int finish;
    FUNC func;

    form(FUNC f, int v = 0, int x = 0, int y = 0) : func(f), vn(v), tag(x), finish(y) {};
}Form;

typedef struct i
{
    int number; //状态编号;
    vector<form> project; //项目族集
    i(int num = 0) :number(num) {};
}I;

typedef struct act
{

    char tag; //r归约， s转移
    int action;  //转移到哪个状态 或者 用哪条产生式归约
    int vn; //归约时产生式的左部
    act(char t = '0', int a = -1, int v = -1) : tag(t), action(a), vn(v) {};
}Act;

typedef struct Qua //四元式, op, arg1, arg2, res
{
    string op;
    string arg1;
    string arg2;
    string res;
    Qua(string o = "", string a1 = "", string a2 = "", string r = "") :op(o), arg1(a1), arg2(a2), res(r) {};
}Quaternion;

struct act_row
{
    vector<Act> row;
    act_row() :row(9) {};
};

struct goto_row
{
    vector<int> row;
    goto_row() :row(5) {};
};

struct binary
{
    int key;
    string val;
    binary(int x = 0, string s = "") :key(x), val(s) {};
};


int result_count = 0; //用于产生新的Temp编号
ifstream fp; //文件流变量
vector<I> DFA; //DFA
vector<act_row> Action;  //Action表
vector<goto_row> GoTo;  //GOTO表
vector<Quaternion> Q; //存储四元式

bool first_has_null(int *first);
void find_first(VN vn);
void find_follow();
void cat_follow(int* s, int* r);
void cat_follow(int* s, int  r);
bool head_is_final(FUNC func);
bool v_is_final(int seq);
void initial();
vector<form> closure(vector<form> pro);
bool operator == (FUNC a, FUNC b);
bool operator == (form a, form b);
bool vec_find(vector<int> vec, int tar);
bool vec_find(vector<form> vec, form tar);
bool operator == (vector<form> a, vector<form> b);
int already_hadI(vector<I> dfa, vector<form> tar);
bool SLR1();
void print_ana(vector<int> x);
void print_state(vector<int> x);
binary read_next();
bool had_qua(int x); //判断产生式是否可以生成四元式

binary read_next()
{
    string str;
    if (getline(fp, str))
    {
        string seq = "";
        string val = "";
        int i = 1;
        while (str[i] != ',')
        {
            seq = seq + str[i];
            i++;
        }
        int key = atoi(seq.c_str());
        //cout << key << endl;
        i += 2;//跳过第一个冒号
        while (str[i] != '"')
        {
            val = val + str[i];
            i++;
        }
        binary res(key, val);

        return res;
    }
    else
    {
        return binary(11, "");
    }

}

ostream & operator<<(ostream &out, Quaternion &A)
{
    out << "(" << A.op << "," << A.arg1 << "," << A.arg2 << "," << A.res << ")";
    return out;
}

bool operator == (FUNC a, FUNC b)
{
    if (a.Keys == b.Keys)
        return true;
    else
        return false;
}

bool operator == (form a, form b)
{
    if (a.vn == b.vn && a.tag == b.tag && a.func == b.func)
        return true;
    else
        return false;
}

bool operator == (vector<form> a, vector<form> b)
{
    if (a.size() != b.size())
        return false;
    for (int i = 0; i < a.size(); i++)
    {
        vector<form>::iterator it = find(b.begin(), b.end(), a[i]);
        if (it != b.end())
            continue;
        else
            return false;
    }
    return true;
}

void initial()
{
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    int func_len = sizeof(func) / sizeof(func[0]);
    for (int i = 0; i < func_len; i++)
    {
        func[i].Seqs = func_seq[i];
        func[i].num = i;
        int len = 0;
        while (func[i].Seqs[len] != 0)
            len++;
        func[i].len = len; //不包括0
    }
    for (int i = 0; i < vn_len; i++)
    {
        vns[i].Funcs = func_vn[i];
        int num = 0;
        while (vns[i].Funcs[num] != 0)
            num++;
        vns[i].Func_num = num; //不包括0;
        vns[i].first = new int[vt_len];
        vns[i].follow = new int[vt_len];
        for (int j = 0; j < vt_len; ++j) {
            vns[i].first[j] = -1;
            vns[i].follow[j] = -1;
        }
    }
    glo->VNs = vns;
    glo->VTs = vts;


}

bool first_has_null(int *first)
{
    int i = 0;
    while (first[i] != 0) //0为遍历一个非终结符号所有导出式结束的标志
    {
        if (first[i] == 16)  //11为空导出式的序号
            return true;
        i++;
    }
    return false;
}

bool v_is_final(int seq)
{
    if (seq >= 7 && seq <= 15) //vt符号的序号空间
        return true;
    else
        return false;
}

bool head_is_final(FUNC func)
{
    if (v_is_final(func.Seqs[0]))
        return true;
    else
        return false;
}

void find_first(VN vn)
{
    if (vn.first[0] > 0)
    {
        return;
    }
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    int vn_len = sizeof(vns) / sizeof(vns[0]);


    int first_index = 0; //填充first集
    int func_index = 0;//func_index遍历非终结符号的产生式
    while (vn.Funcs[func_index] != 0) //vn.Funcs[func_index]为对应的产生式右部序号
    {
        int key = vn.Funcs[func_index];

        if (key == 11)
        {
            vn.first[first_index] = 16; //填充空产生式编号
            first_index++;
        }

        else if (head_is_final(func[key - 1]))  //func[key - 1]为具体的一个产生式右部
        {
            vn.first[first_index] = func[key - 1].Seqs[0];  //func[key - 1].Seq[0]为产生式右部第一个符号的编号
            first_index++;
        }
        else
        {
            VN first_vn = vns[func[key - 1].Seqs[0] - 1];
            if (first_vn.Seq == vn.Seq)
            {
                func_index++;
                continue;
            }
            find_first(first_vn);
            int copy_index = 0;
            while (first_vn.first[copy_index] != -1)
            {
                vn.first[first_index] = first_vn.first[copy_index];
                first_index++;
                copy_index++;
            }
        }
        func_index++;
    }
    vn.first[first_index] = -1;
}

bool follow_has(int *s, int r)
{
    int s_index = 0;
    while (s[s_index] >= 0)
    {
        if (r == s[s_index])
            return true;
        s_index++;
    }
    return false;
}

void cat_follow(int* s, int* r)
{
    //s为要增添的follow集，r为源
    int s_index = 0, r_index = 0;
    while (s[s_index] >= 0) //初始化之后没有填充符号的follow内容为负数
    {
        s_index++;
    }
    while (r[r_index] > 0)
    {
        if (r[r_index] == 16)
            r_index++;
        else if (follow_has(s, r[r_index]))
            r_index++;
        else
        {
            s[s_index] = r[r_index];
            s_index++;
            r_index++;
        }
    }
    return;
}

void cat_follow(int* s, int r)
{
    //s为要增添的follow集，r为源
    int s_index = 0;
    while (s[s_index] >= 0) //初始化之后没有填充符号的follow内容为负数
    {
        s_index++;
    }
    if (follow_has(s, r))
        return;
    s[s_index] = r;
    return;
}

void find_follow()
{

    int vn_len = sizeof(vns) / sizeof(vns[0]);
    for (int i = 0; i < vn_len; i++)
    {
        vns[i].follow[0] = 15;
    }

    //第一次循环 实现A -> aBb， first(b) - 空 属于follow(B)
    for (int vn_index = 0; vn_index < vn_len; vn_index++)
    {
        int func_index = 0;//func_index遍历非终结符号的产生式
        while (vns[vn_index].Funcs[func_index] != 0) //(vn_index + 1)才是当前产生式左部的编号
        {
            int f = vns[vn_index].Funcs[func_index]; //对应func_seq元素的编号, 即一个具体的产生式右部的编号
            int func_len = 1;  //记录一个产生式右部的长度, 包括结束标志0
            while (func[f - 1].Seqs[func_len - 1] != 0)  //func[f - 1].Seqs是一个产生式右部符号的编号数组，
            {                                        //E -> TE'即 （vn_index + 1）-> { 3, 2, 0}
                func_len++;
            }
            for (int i = func_len - 2; i > 0; i--)  //一个产生式右部每相邻的两个非终结符号，follow（before）包含first（after）
            {
                int before = func[f - 1].Seqs[i - 1];
                int after = func[f - 1].Seqs[i];
                if (v_is_final(before) && v_is_final(after))
                {
                    continue;
                }
                else if (!v_is_final(before) && v_is_final(after))
                {
                    cat_follow(vns[before - 1].follow, after);
                }
                else if (!v_is_final(before) && !v_is_final(after))
                {
                    cat_follow(vns[before - 1].follow, vns[after - 1].first);
                }
                else
                {
                    continue;
                }

            }

            func_index++;
        }
    }

    //第二次循环 实现A -> aB 或 A -> aBb, 空属于first（b)， follow(A)属于follow(B)
    for (int vn_index = 0; vn_index < vn_len; vn_index++)
    {
        int func_index = 0;//func_index遍历非终结符号的产生式
        while (vns[vn_index].Funcs[func_index] != 0) //(vn_index + 1)才是当前产生式左部的编号， vn_index是数组索引
        {
            int f = vns[vn_index].Funcs[func_index]; //对应func_seq元素的编号, 即一个具体的产生式右部的编号
            int func_len = 1;  //记录一个产生式右部的长度, 包括结束标志0
            while (func[f - 1].Seqs[func_len - 1] != 0)  //func[f - 1].Seqs是一个产生式右部符号的编号数组，
            {                                        //E -> TE'即 （vn_index + 1）-> { 3, 2, 0}
                func_len++;
            }
            //A -> aB,follow(A)属于follow(B)
            int end = func[f - 1].Seqs[func_len - 2];
            if (v_is_final(end) == false)
            {
                if (end != 16)
                    cat_follow(vns[end - 1].follow, vns[vn_index].follow); // 如E -> TE' follow(E)属于follow(E')
            }
            for (int i = func_len - 2; i > 0; i--)
            {
                int before = func[f - 1].Seqs[i - 1];
                int after = func[f - 1].Seqs[i];
                if (!v_is_final(after) && !v_is_final(before))
                {
                    if (first_has_null(vns[after - 1].first))
                    {
                        //A -> aBb, 空属于first（b)， follow(A)属于follow(B)
                        cat_follow(vns[before - 1].follow, vns[vn_index].follow);
                    }
                }

            }
            func_index++;
        }
    }

}

bool vec_find(vector<int> vec, int tar)
{
    int len = vec.size();
    vector<int>::iterator it = find(vec.begin(), vec.end(), tar);
    if (it != vec.end())
        return true;
    else
        return false;
}

bool vec_find(vector<form> vec, form tar)
{
    int len = vec.size();
    vector<form>::iterator it = find(vec.begin(), vec.end(), tar);
    if (it != vec.end())
        return true;
    else
        return false;
}

int already_hadI(vector<I> dfa, vector<form> tar) //返回的是从0开始的编号
{
    int ilen = dfa.size();
    for (int i = 0; i < ilen; i++)
    {
        if (dfa[i].project != tar)
            continue;
        else
            return dfa[i].number;
    }
    return -1;
}

vector<form> closure(vector<form> pro)
{
    vector<int> visited;
    vector<int> nonvisit;
    int origin_len = pro.size();
    for (int i = 0; i < origin_len; i++) //遍历原项目族里的产生式
    {
        int tag = pro[i].tag; //tag 从0 开始, after_tag 从 1 开始
        int after_tag = pro[i].func.Seqs[tag];//tag = 0，即tag在第0个字符前面, pro[i].func.Seqs[tag]即tag位置后面的字符
        if (!v_is_final(after_tag) && !vec_find(nonvisit, after_tag)) //是VN且还没有入 待访问栈
            nonvisit.push_back(after_tag); //待访问的VN 进栈

    }
    while (!nonvisit.empty())
    {
        int cur = nonvisit.back(); //待访问栈栈顶，即非终结符号的编号，从1开始
        nonvisit.pop_back();
        visited.push_back(cur);

        for (int j = 0; j < vns[cur - 1].Func_num; j++) //遍历VN[cur - 1]的产生式, 将tag在最左边的产生式加入项目族
        {
            int func_index = vns[cur - 1].Funcs[j]; //产生式的编号,从1开始
            form new_form(func[func_index - 1], cur, 0, 0); //产生式FUNC，产生式左部VN编号， finished， tag的位置
            pro.push_back(new_form); //加入项目族
            int first_v = func[func_index - 1].Seqs[0]; //new_form 的第一个字符编号, 也就是它的tag  后面的字符编号
            if (!v_is_final(first_v) && !vec_find(visited, first_v)) //是VN且没有计算过闭包
                nonvisit.push_back(first_v);
        }
    }
    return pro;
}

vector<I> build_DFA()
{
    vector<I> D;
    int curi = 0; //当前正在检查的项目族编号
    vector<int> check; //当前检查的项目族可以接受的字符编号， 比如I0 可以接受 S， V ，i
    vector<form> reduction; //存储当前项目族中可以归约的产生式
    int pro_len;
    int had;
    //建立I0；
    I newi(0);
    form new_form(func[0], 1, 0, 0);
    vector<form> vec_form;
    vec_form.push_back(new_form);
    newi.project = closure(vec_form);
    //
    act_row new_actrow;
    goto_row new_gotorow;
    Action.push_back(new_actrow); //Action表新增一行
    GoTo.push_back(new_gotorow); //Goto表新增一行

    D.push_back(newi);// I0入DFA

    while (curi <= D.size() - 1) //当curi大于D.size()时，说明已经没有新的项目族集再产生了
    {
        pro_len = D[curi].project.size(); //DFA栈顶的项目族  产生式的数量
        for (int i = 0; i < pro_len; i++) //寻找对于当前I，能接受的字符集， 比如I0 可以接受 S， V ，i
        {
            int tag = D[curi].project[i].tag; //查看当前I每个产生式的tag后面的字符，tag从0开始
            FUNC f = D[curi].project[i].func; //项目族集中的产生式
            if (f.Seqs[tag] != 0 && !vec_find(check, f.Seqs[tag])) //tag后面的符号不是 0（产生式结束标志）， 也没有出现在check中
                check.push_back(f.Seqs[tag]); //可以接受的字符入check栈
            else if (f.Seqs[tag] == 0) //已经finished的产生式入归约栈
                reduction.push_back(D[curi].project[i]);

        }

        //检查当前项目族D[curi]导致归约的字符。
        while (!reduction.empty())
        {
            form top = reduction.back();
            reduction.pop_back();
            int vn = top.vn; //vn从1开始
            int vt_len = sizeof(vts) / sizeof(vts[0]);
            for (int i = 0; i < vt_len; i++)
            {
                if (follow_has(vns[vn - 1].follow, i + 7))  //可以归约的产生式左部的follow集 含有 当前vt
                {
                    Action[curi].row[i].tag = 'r';  //归约标志
                    Action[curi].row[i].vn = vn; //产生式左部编号
                    Action[curi].row[i].action = top.func.num; //使用的产生式的编号
                }
            }
        }



        //下面检查的都是当前项目族 D[curi] 移进的字符，不是可以归约的.
        while (!check.empty()) //依次检查check
        {
            int v = check.back();
            check.pop_back();
            vector<form> new_form;
            vector<form> closured;
            for (int i = 0; i < pro_len; i++) //遍历产生式, 将可以接受check里面字符的产生式， tag往后移一位之后加入new_form
            {
                int tag = D[curi].project[i].tag; //tag从0开始
                int vn = D[curi].project[i].vn; //产生式的左部
                FUNC f = D[curi].project[i].func; //项目族集中的产生式
                if (f.Seqs[tag] == v) //可以接受 v  的产生式
                {
                    form temp(f, vn, tag + 1);
                    if (f.Seqs[tag + 1] == 0)//可以归约了， finished设置为1
                        temp.finish = 1;
                    else
                        temp.finish = 0;
                    new_form.push_back(temp);
                }

            }

            closured = closure(new_form); //求一步closure
            had = already_hadI(D, closured); //检查是否之前出现过相同的项目族
            if (had == -1) //不存在相同的, 新建I，加入DFA
            {
                I newi(D.size());
                newi.project = closured;
                D.push_back(newi);
                //下面为更新Action和Goto表
                if (v_is_final(v)) //接受的VT， 更新Action表
                {
                    Action[curi].row[v - 7].tag = 's'; //s代表移进
                    Action[curi].row[v - 7].action = D.size() - 1; //转移到的状态编号
                }
                else  //接受的VN， 更新Goto表
                    GoTo[curi].row[v - 2] = D.size() - 1;  //转移到的状态编号， 要减2，因为Goto表中从S开始，不是S'


                //下面给新的I，在Action和Goto表中新增一行
                act_row new_actrow;
                goto_row new_gotorow;
                Action.push_back(new_actrow); //Action表新增一行
                GoTo.push_back(new_gotorow); //Goto表新增一行
            }
            else //存在相同的, 在Action和Goto中设置状态转移
            {
                if (v_is_final(v)) //接受的VT， 更新Action表
                {
                    Action[curi].row[v - 7].tag = 's'; //s代表移进
                    Action[curi].row[v - 7].action = had; //转移到的状态编号
                }
                else  //接受的VN， 更新Goto表
                    GoTo[curi].row[v - 2] = had;  //转移到的状态编号， 要减2，因为Goto表中从S开始，不是S'
            }

        }

        curi++; //检查下一个项目族集。

    }



    return D;


}

bool had_qua(int x) //判断产生式是否可以生成四元式
{
    if (x == 1 || x == 2 || x == 3 || x == 5 || x == 6)
        return true;
    else
        return false;
}

void print_ana(vector<int> x)
{
    int len = x.size();
    for (int i = 0; i < len; i++)
    {
        if (!v_is_final(x[i]))
            cout << vns[x[i] - 1].Key << " ";
        else
            cout << vts[x[i] - 7].Key << " ";
    }
    cout << endl;
}

void print_state(vector<int> x)
{
    int len = x.size();
    for (int i = 0; i < len; i++)
    {
        cout << x[i] << " ";
    }
    cout << endl;
}

void print_value(vector<string> x)
{
    int len = x.size();
    for (int i = 0; i < len; i++)
    {
        cout << x[i] << " ";
    }
    cout << endl;
}

void print_Q(vector<Quaternion> x)
{
    int len = x.size();
    for (int i = 0; i < len; i++)
    {
        cout << x[i] << endl;
    }
    cout << endl;
}

string newtemp()
{
    char c = 'A';
    c = c + result_count;
    result_count++;
    string newtemp = "";
    newtemp += c;

    return newtemp;
}

bool SLR1()
{
    vector<string> value; //存储符号的属性
    vector<int> analysis; //分析容器存储 字符编号
    vector<int> state;   //状态容器存储  状态编号
    analysis.push_back(15);
    state.push_back(0);
    value.push_back("#");
    //current为当前还未入栈，正在分析的字符，cur_state为当前状态编号,cur_act为对于分析字符的动作，r_vn为归约动作时的产生式左部符号，r_len为归约动作时产生式右部的长度
    int current, cur_state, cur_act, r_vn, r_len;
    binary next; //存储下一个二元组
    string cur_val; //如果当前是i，存储其val
    char tag;
    string a1, a2, r, o;
    bool is_r = false; //如果此轮分析出现了归约，那么current应该变成归约后的VN，也不应该再读新的输入字符串
    while (!(state.back() == 0 && analysis.back() == 1)) //还未Acc
    {
        print_ana(analysis);
        print_state(state);
        print_value(value);
        cout << endl;
        cur_state = state.back();
        if (!is_r)
        {
            next = read_next(); //读取下一个字符
            current = next.key;
            cur_val = next.val;
        }




        if (v_is_final(current))  //是VT，查Action表
        {
            cur_act = Action[cur_state].row[current - 7].action; //状态容器顶
            tag = Action[cur_state].row[current - 7].tag;  //是s还是r
            if (cur_act != -1 && tag == 's') //是s， 移进项目，action为状态转移编号
            {
                state.push_back(cur_act);
                analysis.push_back(current);
                value.push_back(cur_val); //属性值进容器
                is_r = false;
            }
            else if (cur_act != -1 && tag == 'r') //是r， 归约项目, action为产生式编号， func中只有编号1,2,3,5,6可以产生四元式，且只有1不用产生新的临时变量
            {
                r_vn = Action[cur_state].row[current - 7].vn;
                r_len = func[cur_act].len;

                //下面为生成四元式，更改value内容
                if (had_qua(cur_act))//可以产生四元式
                {
                    if (cur_act == 1) //S->V=E
                    {
                        a1 = value.back();
                        value.pop_back();
                        value.pop_back(); // E和=的val出栈
                        r = value.back();
                        value.pop_back();
                        value.push_back(a1); //将新的value入栈
                        Quaternion newq("=", a1, "", r);
                        Q.push_back(newq);
                    }
                    else  //E->E+T, E->E-T, T->T*F, T->T/F
                    {
                        a2 = value.back();
                        value.pop_back();
                        o = value.back();
                        value.pop_back();
                        a1 = value.back();
                        value.pop_back();
                        r = newtemp();
                        value.push_back(r); //将新的value入栈
                        Quaternion newq(o, a1, a2, r);
                        Q.push_back(newq);
                    }
                }
                else if(cur_act == 8)//不产生四元式,（E）
                {
                    value.pop_back();
                    a1 = value.back();
                    value.pop_back();
                    value.pop_back();
                    value.push_back(a1);

                }

                //下面为analysis和state的分析过程
                for (int i = 0; i < r_len; i++)  //将产生式长度的字符出容器
                {
                    analysis.pop_back();
                    state.pop_back();
                }
                if (r_vn == 1)  //归约出S来，对应的新状态是acc
                    analysis.push_back(r_vn);  //VN入栈
                else
                {
                    cur_state = state.back(); //获得出栈后的新最新状态
                    cur_state = GoTo[cur_state].row[r_vn - 2];  //加入归约后的VN之后的新状态
                    analysis.push_back(r_vn);  //VN入栈
                    state.push_back(cur_state);  //新状态入栈
                }

                is_r = true; //此轮发生了归约， 下一轮current就还是当前这一轮的，不再读新字符。


            }
            else if (cur_act == -1)  //Action表中无内容，报错
            {
                cout << "分析出错" << endl;
                return false;
            }
        }
        else  //是VN，查Goto表
        {
            cur_act = Action[cur_state].row[current - 2].action; //要减2，因为Goto表中从S开始，不是S'
            if (cur_act != -1) //Goto表只有移进
            {
                state.push_back(cur_act);
                analysis.push_back(current);
            }
            else  //Goto表中无内容，报错
            {
                cout << "分析出错" << endl;
                return false;
            }
            is_r = false;
        }
    }
    cout << "分析成功" << endl;
    return false;
}




int main()
{

    initial();
    initial();
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    for (int i = 0; i < vn_len; i++)
    {
        find_first(vns[i]);
    }
    find_follow();

    //打印first和follow集
    for (int i = 0; i < vn_len; i++)
    {
        cout << vns[i].Key << ".first: " << endl;
        for (int j = 0; ; j++)
        {
            if (vns[i].first[j] >= 0)
            {
                cout << vts[vns[i].first[j] - 7].Key << "  ";
            }
            else
            {
                break;
            }
        }
        cout << endl;
        cout << vns[i].Key << ".follow: " << endl;
        for (int j = 0; ; j++)
        {
            if (vns[i].follow[j] >= 0)
            {
                cout << vts[vns[i].follow[j] - 7].Key << "  ";
            }
            else
            {
                break;
            }
        }
        cout << endl;
    }
    fp.open("test.txt");
    DFA = build_DFA();
    SLR1();
    print_Q(Q);

    return 0;
}