# 编译原理实验报告
王子龙 18281218 wangzilong@bjtu.edu.cn
## 实验环境
类目 | 详情
:-: | ---
操作系统 | macOS Big Sur 11.4 
CPU | Intel Core i5-7260U@2.3Ghz x2
IDE | CLion 2021.1.2 Build #CL-211.7442.42
Compiler | Apple clang version 11.0.0 (clang-1100.0.33.8)
## 实验要求（实验功能描述）
### 实验项目
实现算符优先分析算法，完成以下描述算术表达式的算符优先文法的算符优先分析过程。
$$
\begin{equation*}
	\begin{aligned}
		G[S]: & E\rightarrow{E+T}|E-T|T\\
					& T\rightarrow{T*F}|T/F|F\\
					& F\rightarrow(E)|i		
  \end{aligned}
\end{equation*}
$$
### 设计说明
终结符号`i`为用户定义的简单变量，即标识符的定义。
### 设计要求
- 构造该算符优先文法的优先关系矩阵或优先函数；
- 输入串应是词法分析的输出二元式序列，即某算术表达式“专题1”的输出结果。输出为输入串是否为该文法定义的算术表达式的判断结果。
- 算符优先分析过程应能发现输入串出错。
- 设计两个测试用例（尽可能完备，正确和出错），并给出测试结果；
- 考虑编写程序根据算符优先文法构造算符优先关系矩阵，并添加到你的算符优先分析程序中。
### 任务分析
重点解决算符优先矩阵的构造方法和算符优先算法的实现。
## 主要数据结构描述
### 文法结构体`GLO`
```C++
typedef struct Glo
{
    VN *VNs;
    VN *VTs;
} GLO;
```
其中`VNs`为非终结符号数组，` VTs`为终结符号数组，定义`VN`类型如下:
### 符号数组
```C++
typedef struct vn
{
    string Key;
    int Seq;
    int *Funcs;
    int *first;	//以-1为结束标志
    int *follow; //以-1为结束标志
} VN;
```
每个`VN`类型变量包含一个`String`类型的`Key`，存储其具体名称，一个编号`Seq`，非终结符号还有对应的产生式指针`Func`，其中存储对应产生式的编号。以及`First`集和`Follow`集指针，其中存储的为终结符号的编号。

本实验中的`VNs`和`VTs`定义如下（其中“0”表示空产生式）：
```C++
VN vns[] = {{"E", 1}, {"T", 2}, {"F", 3}};
VN vts[] = {{"+", 4}, {"-", 5}, {"*", 6}, {"/", 7}, {"(", 8}, {")", 9}, {"i", 10}, {"#", 11}};
```
每个产生式右部的数据结构定义如下：
```C++
//每个导出式右部的字符串,以及对应的符号序号将在initial中链接， “0”为空导出式
FUNC func[] = {{"E+T"}, {"E-T"}, {"T"}, {"T*F"}, {"T/F"}, {"F"}, {"(E)"}, {"i"}, {"0"}};

//每个导出式右部的序号数组,以0为一个右部的结束标志,如TE' 为3，2，0, 里面的数字对应vns和vnt中的序号
int func_seq[][10] = {{1, 4, 2, 0}, {1, 5, 2, 0}, {2, 0}, {2, 6, 3, 0}, {2, 7, 3, 0}, {3, 0}, {8, 1, 9, 0}, {10, 0}, {11, 0}};
```
因为一个非终结符号可以导出多个产生式右部，故要对每个产生式右部编号，让`VN`结构体中的`Funcs`数组存储每个非终结结符号对应的所有右部编号。按照 E,E’...的顺序，分别对应的产生式右部编号为(0为结束标志):
```C++
//每个非终结符号包含的所有导出式的序号， 里面的数字对应func_seq中的数组序号，如 1 对应func[0], 2 对应func[1]
int func_vn[][10] = {{1, 2, 3, 0}, {4, 5, 6, 0}, {7, 8, 0}};
```
## 程序结构描述
### 文法分析
本实验的文法为：
$$
\begin{equation*}
	\begin{aligned}
		G[S]: & E\rightarrow{E+T}|E-T|T\\
					& T\rightarrow{T*F}|T/F|F\\
					& F\rightarrow(E)|i		
  \end{aligned}
\end{equation*}
$$
则$FirstVT$与$LastVT$两个集合为
|$FirstVT$|$LastVT$
:-:|:-:|:-:
$E$|$\{+,-\, *, /, (\}$|$\{+,-\,*,/,)\}$
$T$|$\{*,/,(,i\}$|$\{*,/,),i\}$
$F$|$\{(,i\}$|$\{),i\}$
进而可以求得算符优先矩阵为
|+|-|*|/|(|)|i
:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
+|>|>|<|<|<|>|<
-|>|>|<|<|<|>|<
*|>|>|>|>|<|>|<
/|>|>|>|>|<|>|<
(|<|<|<|<|<|=|<
)|>|>|>|>||>|
i|>|>|>|>||>|
### FirstVT与LastVTech自动求解
求解$FirstVT$与$LastVT$两个集合的方法基本相同，共分两个步骤，*e.g.*求解FirstVT时，应设置一个栈，定义一个结构体`VT`如下，$VT(1,3)$表示第一个非中介字符的FirstVT/LastVT集包含第三个终结符号。首先根据文法的产生式，将形如$U\rightarrow{b}...$或$U\rightarrow{Vb}...$的产生式中的$b$加入到$U$的*FirstVT*集中，即将$FirstVT(U,b)$压入栈中，此即步骤一初始化，接下来的步骤二中，依次出栈，加入出栈元素为$(V,b)$，则将寻找形如$U\rightarrow{V...}$的产生式，使$FirstVT(V)\in FirstVT(U)$，再将所有的新优先关系压入栈中，循环往复直至栈空。
```C++
struct vt
{
    int vn_seq;
    int vt_seq;
    vt(int x = 0, int y = 0) : vn_seq(x), vt_seq(y){};
};
```
使用两个全局变量存储文法的$FirstVT$和$LastVT$两个集合。
```C++
int **firstvt;
int **lastvt;
```
定义两个函数进行分布求解：
```C++
void find_firstvt()
{
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    //int func_len = sizeof(func) / sizeof(func[0]);
    vector<vt> stack;
    //步骤1
    for (int i = 0; i < vn_len; i++)
    {
        int func_num = 0;
        while (vns[i].Funcs[func_num] != 0)
        {
            func_num++;
        }
        for (int j = 0; j < func_num; j++)
        {
            int func_index = vns[i].Funcs[j] - 1; //对应具体产生式的索引
            int first_seq = func[func_index].Seqs[0];
            int second_seq = func[func_index].Seqs[1];
            if (first_seq >= 4 && first_seq <= 10)
            {
                firstvt[i][first_seq - 4] = 1;
                stack.push_back(vt(i, first_seq - 4));
            }
            else if (second_seq >= 4 && second_seq <= 10)
            {
                firstvt[i][second_seq - 4] = 1;
                stack.push_back(vt(i, second_seq - 4));
            }
        }
    }
    //步骤2
    while (!stack.empty())
    {
        vt top = stack.back();
        stack.pop_back();
        int vn_seq = top.vn_seq + 1; //恢复到1到3
        int vt_seq = top.vt_seq + 4; //恢复到4到10
        for (int i = 0; i < vn_len; i++)
        {
            if (i == vn_seq - 1)
                continue;
            int func_num = 0;
            while (vns[i].Funcs[func_num] != 0)
            {
                func_num++;
            }
            for (int j = 0; j < func_num; j++) //循环当前检索的非终结符号的产生式
            {
                int func_index = vns[i].Funcs[j] - 1; //对应具体产生式的索引
                int first_seq = func[func_index].Seqs[0];
                if (first_seq == vn_seq) //产生式首部符号是当前出栈的非终结符号
                {
                    if (firstvt[i][vt_seq - 4] != 1)
                    {
                        firstvt[i][vt_seq - 4] = 1;
                        stack.push_back(vt(i, vt_seq - 4));
                    }
                }
            }
        }
    }
}

void find_lastvt()
{
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    //int func_len = sizeof(func) / sizeof(func[0]);
    vector<vt> stack;
    //步骤1
    for (int i = 0; i < vn_len; i++)
    {
        int func_num = 0;
        while (vns[i].Funcs[func_num] != 0)
        {
            func_num++;
        }
        for (int j = 0; j < func_num; j++)
        {
            int func_index = vns[i].Funcs[j] - 1; //对应具体产生式的索引
            int func_len = 0;
            int seclast_seq;
            int last_seq;
            while (func[func_index].Seqs[func_len] != 0)
                func_len++;

            last_seq = func[func_index].Seqs[func_len - 1];
            if (func_len > 1)
                seclast_seq = func[func_index].Seqs[func_len - 2];
            else
                seclast_seq = -1;

            if (last_seq >= 4 && last_seq <= 10)
            {
                lastvt[i][last_seq - 4] = 1;
                stack.push_back(vt(i, last_seq - 4));
            }
            else if (seclast_seq >= 4 && seclast_seq <= 10)
            {
                lastvt[i][seclast_seq - 4] = 1;
                stack.push_back(vt(i, seclast_seq - 4));
            }
        }
    }
    //步骤2
    while (!stack.empty())
    {
        vt top = stack.back();
        stack.pop_back();
        int vn_seq = top.vn_seq + 1; //恢复到1到3
        int vt_seq = top.vt_seq + 4; //恢复到4到10
        for (int i = 0; i < vn_len; i++)
        {
            if (i == vn_seq - 1)
                continue;
            int func_num = 0;
            while (vns[i].Funcs[func_num] != 0)
            {
                func_num++;
            }
            for (int j = 0; j < func_num; j++) //循环当前检索的非终结符号的产生式
            {
                int func_index = vns[i].Funcs[j] - 1; //对应具体产生式的索引
                int func_len = 0;
                while (func[func_index].Seqs[func_len] != 0)
                    func_len++;

                int last_seq = func[func_index].Seqs[func_len - 1];

                if (last_seq == vn_seq) //产生式首部符号是当前出栈的非终结符号
                {
                    if (lastvt[i][vt_seq - 4] != 1)
                    {
                        lastvt[i][vt_seq - 4] = 1;
                        stack.push_back(vt(i, vt_seq - 4));
                    }
                }
            }
        }
    }
}
```
### 算符优先矩阵自动求解
定义全局变量存储算符优先矩阵，其中值为-1表示无优先关系，0 为=关系，1 为<，2为>。
```C++
int **OGmat;
```
根据刚刚求解的$FirstVT$和$LastVT$集，针对灭一个产生式规则，形如$...ab...$，$...aVb...$，置$a=b$，$...aU...$的置$a<FirstVT(U)$，$...Ub...$的置$LastVT(U)>b$。
随后使用函数$build_OGmat$对算符优先矩阵进行求解：
```C++
void build_OGmat()
{
    //0为相等， 1为小于， 2为大于， -1为不存在关系
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    OGmat = new int *[vt_len];
    for (int i = 0; i < vt_len; i++)
    {
        OGmat[i] = new int[vt_len];
        for (int j = 0; j < vt_len; ++j) OGmat[i][j] = -1;
    }
    for (int i = 0; i < vt_len; i++)
    {
        OGmat[i][vt_len - 1] = 2;
        OGmat[vt_len - 1][i] = 1;
    }
    OGmat[vt_len - 1][vt_len - 1] = 0;
    for (int i = 0; i < vn_len; i++)
    {
        int func_num = 0;
        while (vns[i].Funcs[func_num] != 0)
        {
            func_num++;
        }
        for (int j = 0; j < func_num; j++)
        {
            int func_index = vns[i].Funcs[j] - 1; //对应具体产生式的索引
            int func_len = 0;
            while (func[func_index].Seqs[func_len] != 0)
                func_len++;				//func_len包含0 ，即{1, 0}则len为2
            int f1, f2, f3, s1, s2, s3; //三个字符一起找
            if (func_len > 2)
            {
                f1 = 0;
                f2 = 1;
                f3 = 2;
                while (func[func_index].Seqs[f3] != 0)
                {
                    s1 = func[func_index].Seqs[f1];
                    s2 = func[func_index].Seqs[f2];
                    s3 = func[func_index].Seqs[f3];
                    if (is_vt(s1) && is_vt(s2))
                        OGmat[s1 - 4][s2 - 4] = 0;
                    if (is_vt(s2) && is_vt(s3))
                        OGmat[s2 - 4][s3 - 4] = 0;

                    if (is_vt(s1) && is_vt(s3))
                        OGmat[s1 - 4][s3 - 4] = 0;

                    if (is_vt(s1) && is_vn(s2))
                        for (int k = 0; k < vt_len; k++)
                            if (firstvt[s2 - 1][k] == 1)
                                OGmat[s1 - 4][k] = 1;

                    if (is_vn(s1) && is_vt(s2))
                        for (int k = 0; k < vt_len; k++)
                            if (lastvt[s1 - 1][k] == 1)
                                OGmat[k][s2 - 4] = 2;

                    if (is_vt(s2) && is_vn(s3))
                        for (int k = 0; k < vt_len; k++)
                            if (firstvt[s3 - 1][k] == 1)
                                OGmat[s2 - 4][k] = 1;

                    if (is_vn(s2) && is_vt(s3))
                        for (int k = 0; k < vt_len; k++)
                            if (lastvt[s2 - 1][k] == 1)
                                OGmat[k][s3 - 4] = 2;
                    f1++;
                    f2++;
                    f3++;
                }
            }

            if (func_len == 2)
            {

                s1 = func[func_index].Seqs[0];
                s2 = func[func_index].Seqs[1];

                if (is_vt(s1) && is_vt(s2))
                    OGmat[s1 - 4][s2 - 4] = 0;

                if (is_vt(s1) && is_vn(s2))
                    for (int k = 0; k < vt_len; k++)
                        if (firstvt[s2 - 1][k] == 1)
                            OGmat[s1 - 4][k] = 1;
                if (is_vn(s1) && is_vt(s2))
                    for (int k = 0; k < vt_len; k++)
                        if (lastvt[s1 - 1][k] == 1)
                            OGmat[k][s2 - 4] = 2;
            }
        }
    }
}
```
## 程序测试
### 测试样例1
$$
i+i*i\#
$$
**INPUT:**
$(10,"i")$
$(4,"+")$
$(10,"i")$
$(6,"*")$
$(10,"i")$
$(11,"\#")$
运行结果如下：

```
# 
# i 
# F 
# F + 
# F + i 
# F + F 
# F + F * 
# F + F * i 
# F + F * F 
# F + T 
# E 
# E # 
成功
```
### 测试样例2
$$
i+iii\#
$$
**INPUT:**
$(10,"i")$
$(4,"+")$
$(10,"i")$
$(10,"i")$
$(10,"i")$
$(11,"\#")$
运行结果如下：
```
# 
# i 
# F 
# F + 
# F + i 
i i无优先关系
```
### 测试样例3
$$
i+*i)\#
$$
**INPUT:**
$(10,"i")$
$(4,"+")$
$(6,"*")$
$(10,"i")$
$(9,")")$
$(11,"\#")$
运行结果如下：
```
# 
# i 
# F 
# F + 
# F + * 
# F + * i 
# F + * F 
* F 
上述产生式匹配无效
```



