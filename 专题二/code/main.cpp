#include <bits/stdc++.h>
using namespace std;
ifstream fp;
int current = 0;
typedef struct vn
{
    string Key;
    int Seq;
    int *Funcs;
    int *first;	//以-1为结束标志
    int *follow; //以-1为结束标志
} VN;

typedef struct func
{
    string Keys;
    int *Seqs;
} FUNC;

typedef struct Glo
{
    VN *VNs;
    VN *VTs;
} GLO;
GLO *glo = new GLO;
VN vns[] = {{"E", 1}, {"E'", 2}, {"T", 3}, {"T'", 4}, {"F", 5}, {"A", 6}, {"M", 7}};
VN vts[] = {{"(", 8}, {")", 9}, {"/", 10}, {"i", 11}, {"*", 12}, {"-", 13}, {"#", 14}, {"+", 15}, {"0", 16}};

//每个导出式右部的字符串,以及对应的符号序号将在initial中链接， “0”为空导出式
FUNC func[] = {{"TE'"}, {"ATE'"}, {"FT'"}, {"MFT'"}, {"(E)"}, {"+"}, {"-"}, {"i"}, {"*"}, {"/"}, {"0"}};

//每个导出式右部的序号数组,以0为一个右部的结束标志,如TE' 为3，2，0, 里面的数字对应vns和vnt中的序号
int func_seq[][10] = {{3, 2, 0}, {6, 3, 2, 0}, {5, 4, 0}, {7, 5, 4, 0}, {8, 1, 9, 0}, {15, 0}, {13, 0}, {11, 0}, {12, 0}, {10, 0}, {16, 0}};

//每个非终结符号包含的所有导出式的序号， 里面的数字对应func_seq中的数组序号，如 1 对应func[0], 2 对应func[1]
int func_vn[][10] = {{1, 0}, {2, 11, 0}, {3, 0}, {4, 11, 0}, {5, 8, 0}, {6, 7, 0}, {9, 10, 0}};

//每个非终结符号包含的所有导出式字符串，“0”为空导出式
string func_name[][10] = {{"TE'", "#"}, {"ATE'", "0", "#"}, {"FT'", "#"}, {"MFT'", "0", "#"}, {"(E)", "i", "#"}, {"+", "-", "#"}, {"*", "/", "#"}};

bool v_is_final(int seq);			  //判断一个符号是否为VT
bool head_is_final(FUNC func);		  //判断一个产生式的首符号是否为VT
bool funcs_has_null(int *funcs);	  //判断非终结符号的产生式中是否含有空产生式
bool first_has_null(int *first);	  //判断非终结符号的first集中是否含有空产生式
bool in_first(int *first, int seq);	  //判断是否在first集中
bool in_follow(int *follow, int seq); //判断是否在follow集中
void initial();
void find_first(VN vn);
void find_follow();
void cat_follow(int *s, int *r);
void cat_follow(int *s, int r);

bool call_E();
bool call_E1();
bool call_T();
bool call_T1();
bool call_F();
bool call_A();
bool call_M();
bool recursive_descent();

void initial()
{
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    int func_len = sizeof(func) / sizeof(func[0]);
    for (int i = 0; i < func_len; i++)
    {
        func[i].Seqs = func_seq[i];
    }
    for (int i = 0; i < vn_len; i++)
    {
        vns[i].Funcs = func_vn[i];
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

bool v_is_final(int seq)
{
    if (seq >= 8 && seq <= 15) //vt符号的序号空间
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

bool funcs_has_null(int *funcs)
{
    int i = 0;
    while (funcs[i] != -1) //-1为遍历一个first集结束的标志
    {
        if (funcs[i] == 11) //11为空导出式的序号
            return true;
        i++;
    }
    return false;
}

bool first_has_null(int *first)
{
    int i = 0;
    while (first[i] != 0) //0为遍历一个非终结符号所有导出式结束的标志
    {
        if (first[i] == 16) //11为空导出式的序号
            return true;
        i++;
    }
    return false;
}

bool in_first(int *first, int seq)
{
    int index = 0;
    while (first[index] >= 0)
    {
        if (seq == first[index])
            return true;
        index++;
    }
    return false;
}

bool in_follow(int *follow, int seq)
{
    int index = 0;
    while (follow[index] >= 0)
    {
        if (seq == follow[index])
            return true;
        index++;
    }
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

    int first_index = 0;			  //填充first集
    int func_index = 0;				  //func_index遍历非终结符号的产生式
    while (vn.Funcs[func_index] != 0) //vn.Funcs[func_index]为对应的产生式右部序号
    {
        int key = vn.Funcs[func_index];

        if (key == 11)
        {
            vn.first[first_index] = 16; //填充空产生式编号
            first_index++;
        }

        else if (head_is_final(func[key - 1])) //func[key - 1]为具体的一个产生式右部
        {
            vn.first[first_index] = func[key - 1].Seqs[0]; //func[key - 1].Seq[0]为产生式右部第一个符号的编号
            first_index++;
        }
        else
        {
            VN first_vn = vns[func[key - 1].Seqs[0] - 1];
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

void cat_follow(int *s, int *r)
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

void cat_follow(int *s, int r)
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
        vns[i].follow[0] = 14;
    }

    //第一次循环 实现A -> aBb， first(b) - 空 属于follow(B)
    for (int vn_index = 0; vn_index < vn_len; vn_index++)
    {
        int func_index = 0;							 //func_index遍历非终结符号的产生式
        while (vns[vn_index].Funcs[func_index] != 0) //(vn_index + 1)才是当前产生式左部的编号
        {
            int f = vns[vn_index].Funcs[func_index];	//对应func_seq元素的编号, 即一个具体的产生式右部的编号
            int func_len = 1;							//记录一个产生式右部的长度, 包括结束标志0
            while (func[f - 1].Seqs[func_len - 1] != 0) //func[f - 1].Seqs是一个产生式右部符号的编号数组，
            {											//E -> TE'即 （vn_index + 1）-> { 3, 2, 0}
                func_len++;
            }
            for (int i = func_len - 2; i > 0; i--) //一个产生式右部每相邻的两个非终结符号，follow（before）包含first（after）
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
        int func_index = 0;							 //func_index遍历非终结符号的产生式
        while (vns[vn_index].Funcs[func_index] != 0) //(vn_index + 1)才是当前产生式左部的编号， vn_index是数组索引
        {
            int f = vns[vn_index].Funcs[func_index];	//对应func_seq元素的编号, 即一个具体的产生式右部的编号
            int func_len = 1;							//记录一个产生式右部的长度, 包括结束标志0
            while (func[f - 1].Seqs[func_len - 1] != 0) //func[f - 1].Seqs是一个产生式右部符号的编号数组，
            {											//E -> TE'即 （vn_index + 1）-> { 3, 2, 0}
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

int read_next()
{
    string str;
    if (getline(fp, str))
    {
        string seq = "";
        int i = 1;
        while (str[i] != ',')
        {
            seq = seq + str[i];
            i++;
        }
        int key = atoi(seq.c_str());
        //cout << key << endl;
        return key;
    }
    else
    {
        return 14;
    }
}

bool recursive_descent()
{
    fp.open("test.txt");
    current = read_next();
    if (call_E())
        cout << "成功" << endl;
    else
        cout << "失败" << endl;
    return true;
}

bool call_E()
{
    if (in_first(vns[0].first, current))
        if (call_T())
            if (call_E1())
            {
                cout << "E->T E1" << endl;
                return true;
            }
    return false;
}

bool call_E1()
{

    if (in_first(vns[1].first, current))
    {
        if (call_A())
        {
            if (call_T())
                if (call_E1())
                {
                    cout << "E1 -> A T E1" << endl;
                    return true;
                }
                else
                    return false;
        }
        else
            return false;
    }
    else
    {
        if (in_follow(vns[1].follow, current))
        {
            cout << "E1 -> 空" << endl;
            return true;
        }

        else
            return false;
    }
    return false;
}

bool call_T()
{

    if (in_first(vns[2].first, current))
    {
        if (call_F())
        {
            if (call_T1())
            {
                cout << "T -> F T1" << endl;
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

bool call_T1()
{

    if (in_first(vns[3].first, current))
    {
        if (call_M())
        {
            if (call_F())
                if (call_T1())
                {
                    cout << "T1 -> M F T1" << endl;
                    return true;
                }
                else
                    return false;
        }
        else
            return false;
    }
    else
    {
        if (in_follow(vns[3].follow, current))
        {
            cout << "T1 -> 空" << endl;
            return true;
        }

        else
            return false;
    }
    return false;
}

bool call_M()
{

    if (current == 12)
    {
        current = read_next();
        cout << "M -> *" << endl;
        return true;
    }
    else if (current == 10)
    {
        current = read_next();
        cout << "M -> /" << endl;
        return true;
    }
    else
    {
        return false;
    }
}

bool call_A()
{

    if (current == 15)
    {
        current = read_next();
        cout << "A -> +" << endl;
        return true;
    }
    else if (current == 13)
    {
        current = read_next();
        cout << "A -> -" << endl;
        return true;
    }
    else
    {
        return false;
    }
}

bool call_F()
{

    if (current == 8)
    {
        current = read_next();
        if (call_E())
        {
            if (current == 9)
            {
                current = read_next();
                cout << "F -> ( E )" << endl;
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
    {
        if (current == 11)
        {
            current = read_next();
            cout << "F -> i" << endl;
            return true;
        }
        else
            return false;
    }
}

int main()
{

    initial();
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    for (int i = 0; i < vn_len; i++)
    {
        find_first(vns[i]);
    }
    find_follow();

    for (int i = 0; i < vn_len; i++)
    {
        cout << vns[i].Key << ".first: " << endl;
        for (int j = 0;; j++)
        {
            if (vns[i].first[j] >= 0)
            {
                cout << vts[vns[i].first[j] - 8].Key << "  ";
            }
            else
            {
                break;
            }
        }
        cout << endl;
        cout << vns[i].Key << ".follow: " << endl;
        for (int j = 0;; j++)
        {
            if (vns[i].follow[j] >= 0)
            {
                cout << vts[vns[i].follow[j] - 8].Key << "  ";
            }
            else
            {
                break;
            }
        }
        cout << endl;
    }

    recursive_descent();

    return 0;
}