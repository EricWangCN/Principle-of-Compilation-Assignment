#include <bits/stdc++.h>

using namespace std;

ifstream fp;
struct vt
{
    int vn_seq;
    int vt_seq;
    vt(int x = 0, int y = 0) : vn_seq(x), vt_seq(y){};
};

int **OGmat;
int **firstvt;
int **lastvt;

typedef struct vn
{
    string Key;
    int Seq;
    int *Funcs;
    int *first; //以-1为结束标志
    int *last; //以-1为结束标志
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
VN vns[] = {{"E", 1}, {"T", 2}, {"F", 3}};
VN vts[] = {{"+", 4}, {"-", 5}, {"*", 6}, {"/", 7}, {"(", 8}, {")", 9}, {"i", 10}, {"#", 11}};

//每个导出式右部的字符串,以及对应的符号序号将在initial中链接， “0”为空导出式
FUNC func[] = {{"E+T"}, {"E-T"}, {"T"}, {"T*F"}, {"T/F"}, {"F"}, {"(E)"}, {"i"}, {"0"}};

//每个导出式右部的序号数组,以0为一个右部的结束标志,如TE' 为3，2，0, 里面的数字对应vns和vnt中的序号
int func_seq[][10] = {{1, 4, 2, 0}, {1, 5, 2, 0}, {2, 0}, {2, 6, 3, 0}, {2, 7, 3, 0}, {3, 0}, {8, 1, 9, 0}, {10, 0}, {11, 0}};

//每个非终结符号包含的所有导出式的序号， 里面的数字对应func_seq中的数组序号，如 1 对应func[0], 2 对应func[1]
int func_vn[][10] = {{1, 2, 3, 0}, {4, 5, 6, 0}, {7, 8, 0}};
void initial();
void find_firstvt();
void find_lastvt();
bool is_vt(int x);
bool is_vn(int x);

bool is_vt(int x)
{
    if (x >= 4 && x <= 10)
        return true;
    else
        return false;
}

bool is_vn(int x)
{
    if (x >= 1 && x <= 3)
        return true;
    else
        return false;
}

void initial()
{
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    int func_num = sizeof(func) / sizeof(func[0]);
    for (int i = 0; i < func_num; i++)
    {
        func[i].Seqs = func_seq[i];
    }
    for (int i = 0; i < vn_len; i++)
    {
        vns[i].Funcs = func_vn[i];
        vns[i].first = new int[vt_len];
        vns[i].last = new int[vt_len];
        for (int j = 0; j < vt_len; ++j) {
            vns[i].first[j] = -1;
            vns[i].last[j] = -1;
        }
    }
    glo->VNs = vns;
    glo->VTs = vts;

    firstvt = new int *[vn_len];
    lastvt = new int *[vn_len];
    for (int i = 0; i < vn_len; i++)
    {
        firstvt[i] = new int[vt_len];
        lastvt[i] = new int[vt_len];
    }
    for (int i = 0; i < vn_len; ++i)
        for (int j = 0; j < vt_len; ++j) {
            firstvt[i][j] = 0;
            lastvt[i][j] = 0;
        }
}

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
    cout << "firstvt:\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 10; ++j) {
            printf("%d, ", firstvt[i][j]);
        }
        cout << '\n';
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
    cout << "lastvt:\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 10; ++j) {
            printf("%d, ", lastvt[i][j]);
        }
        cout << '\n';
    }
}

void build_OGmat()
{
    //0为相等， 1为小于， 2为大于， -1为不存在关系
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    OGmat = new int *[vt_len];
    for (int i = 0; i < vt_len; i++)
    {
        OGmat[i] = new int[vt_len];
//        memset(OGmat[i], -1, sizeof(OGmat[i]) * vt_len);
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
    for (int i = 0; i < vt_len; ++i) {
        for (int j = 0; j < vt_len; ++j) {
            printf("%d, ", OGmat[i][j]);
        }
        cout << '\n';
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
        return 11;
    }

}
/*
int reduction(vector<int> red)
{
	vector<int> x = red;
	int vt_len = sizeof(vts) / sizeof(vts[0]);
	int vn_len = sizeof(vns) / sizeof(vns[0]);
	int len = x.size();
	int err = 1;
	while (1)
	{
		int flag1 = 0;
		int flag;

		for (int i = 0; i < vn_len; i++)
		{
			int func_num = 0;
			while (vns[i].Funcs[func_num] != 0)
				func_num++;
			for (int j = 0; j < func_num; j++)
			{
				int func_index = vns[i].Funcs[j] - 1;//对应具体产生式的索引
				int func_len = 0;
				while (func[func_index].Seqs[func_len] != 0)
					func_len++;
				if (func_len != len)
					continue; // 产生式长度和需要匹配的不等长要跳过
				flag = 1; //记录是否匹配出错
				for (int k = 0; k < func_len; k++)
					if (x[k] != func[func_index].Seqs[k])
					{
						flag = 0; //匹配出错
						break;
					}

				if (flag)//匹配成功，对x进行归约
				{
					flag1 = 1; //记录有能够成功匹配的产生式
					err = 0;
					x = { i + 1 };
					break;
				}

			}
			if (flag)// //匹配成功，跳出循环
				break;
		}
		if (err)
			return -1;

		if (flag1 == 0) //没有能继续匹配的了，返回
			return x[0];
	}
}
*/
//对给定符号序列进行归约
bool singal_reduction(int des, int red)
{
    int func_num = 0;
    while (vns[des - 1].Funcs[func_num] != 0)
        func_num++;
    for (int i = 0; i < func_num; i++)
    {
        int func_index = vns[des - 1].Funcs[i] - 1; //对应具体产生式的索引
        int func_len = 0;
        if (func[func_index].Seqs[1] != 0)
            continue;
        if (is_vn(func[func_index].Seqs[0]))
        {
            if (func[func_index].Seqs[0] == red)
                return true;
            else
            {
                return singal_reduction(func[func_index].Seqs[0], red);
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}
int reduction(vector<int> red)
{
    vector<int> x = red;
    int vt_len = sizeof(vts) / sizeof(vts[0]);
    int vn_len = sizeof(vns) / sizeof(vns[0]);
    int len = x.size();
    int err = 1;

    int flag1 = 0;
    int flag = 0;

    for (int i = 0; i < vn_len; i++)
    {
        int func_num = 0;
        while (vns[i].Funcs[func_num] != 0)
            func_num++;
        for (int j = 0; j < func_num; j++)
        {
            int func_index = vns[i].Funcs[j] - 1; //对应具体产生式的索引
            int func_len = 0;
            while (func[func_index].Seqs[func_len] != 0)
                func_len++;
            if (func_len != len)
                continue; // 产生式长度和需要匹配的不等长要跳过
            flag = 1;	  //记录是否匹配出错
            for (int k = 0; k < func_len; k++)
            {
                if (x[k] != func[func_index].Seqs[k])
                {
                    if (!is_vn(x[k])) //是终结符匹配错误肯定不行
                    {
                        flag = 0; //匹配出错
                        break;
                    }
                    else if (!singal_reduction(func[func_index].Seqs[k], x[k]))
                    {
                        flag = 0;
                        break;
                    }
                }
            }

            if (flag) //匹配成功，对x进行归约
            {
                flag1 = 1; //记录有能够成功匹配的产生式
                err = 0;
                return i + 1;
            }
        }
        if (flag) // //匹配成功，跳出循环
            break;
    }
    if (err)
        return -1;
    return -1;
}

void print(vector<int> x)
{
    int len = x.size();
    for (int i = 0; i < len; i++)
    {
        if (is_vn(x[i]))
            cout << vns[x[i] - 1].Key << " ";
        else
            cout << vts[x[i] - 4].Key << " ";
    }
    cout << endl;
}

void OG_analysis()
{
    vector<int> analysis;
    vector<int> scan_vt;   //保留已进栈的vt
    vector<int> before_vt; //保留小于后面终结符号的vt在ana里面的序号，如#<i, #就进去
    analysis.push_back(11);
    scan_vt.push_back(11);
    int ana_len = 0;
    int top_vt = scan_vt.back();
    int before;
    int flag = 1; //判断是否读取下一字符，如果刚刚归约完，是不读的.
    int current = read_next();
    while (1)
    {
        print(analysis);
        if (analysis.size() == 3)
        {
            if (analysis[0] == 11 && analysis[1] == 1 && analysis[2] == 11)
            {
                cout << "成功" << endl;
                return;
            }
        }
        if (is_vn(current))
        {
            analysis.push_back(current);
            ana_len++;
            flag = 1;
        }

        else
        {
            if (OGmat[top_vt - 4][current - 4] == 1) //小于
            {
                if (is_vn(analysis[ana_len]))
                    before_vt.push_back(ana_len - 1);
                else
                    before_vt.push_back(ana_len);
                analysis.push_back(current);
                scan_vt.push_back(current);
                top_vt = scan_vt.back();
                ana_len++;
                flag = 1;
            }
            else if (OGmat[top_vt - 4][current - 4] == 0) //等于
            {
                analysis.push_back(current);
                scan_vt.push_back(current);
                top_vt = scan_vt.back();
                ana_len++;
                flag = 1;
            }

            else if (OGmat[top_vt - 4][current - 4] == 2) //大于
            {
                //归约上一个非终结符号之后到下一个终结符号之前的产生式
                before = before_vt.back(); //上一个小于的终结符号
                before_vt.pop_back();	   //出现大于后，上一个小于的非终结符号就出栈.
                vector<int>::iterator it;
                vector<int> red;

                vector<int>::iterator del_start = analysis.begin() + before + 1;
                int red_seq, del_num = 0;
                for (it = analysis.begin() + before + 1; it != analysis.end(); it++)
                {
                    red.push_back(*it);
                    del_num++;
                }

                ana_len -= del_num; //更新ana_len;
                red_seq = reduction(red);
                if (red_seq == -1)
                {
                    print(red);
                    cout << "上述产生式匹配无效" << endl;
                    return;
                }

                analysis.erase(del_start, analysis.end()); //把待归约的出栈然后进栈归约的符号。
                analysis.push_back(red_seq);
                ana_len++;
                flag = 0;
                top_vt = analysis[before]; //更新最顶vt.
            }
            else if (OGmat[top_vt - 4][current - 4] == -1)
            {
                cout << vts[top_vt - 4].Key << " " << vts[current - 4].Key << "无优先关系" << endl;
                return;
            }
        }
        if (flag)
            current = read_next();
    }
}

int main()
{
    initial();
    find_firstvt();
    find_lastvt();
    build_OGmat();
    fp.open("test.txt");
    OG_analysis();

    return 0;
}