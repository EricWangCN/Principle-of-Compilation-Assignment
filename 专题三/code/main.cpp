#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;
vector<int> analysis;
ifstream fp;
int ** table;
typedef struct vn
{
	string Key;
	int Seq;
	int *Funcs = NULL;
	int *first = NULL;   //以-1为结束标志
	int *follow = NULL;  //以-1为结束标志
}VN;

typedef struct func
{
	string Keys;
	int *Seqs;
	int *first = NULL; //LL1分析法中的first集是对应到每个产生式的
}FUNC;

typedef struct Glo
{
	VN* VNs;
	VN* VTs;

}GLO;
GLO* glo = new GLO;
VN vns[] = { {"E", 1}, {"E'", 2}, {"T", 3}, {"T'", 4}, {"F", 5}, {"A", 6},
			 {"M", 7} };
VN vts[] = { {"(", 8}, {")", 9}, {"/", 10}, {"i", 11}, {"*", 12}, {"-", 13},
			 {"#", 14}, {"+", 15}, {"0", 16} };

//每个导出式右部的字符串,以及对应的符号序号将在initial中链接， “0”为空导出式
FUNC func[] = { {"TE'"}, {"ATE'"}, {"FT'"}, {"MFT'"}, {"(E)"}, {"+"}, {"-"}, {"i"}, {"*"}, {"/"}, {"0"} };

//每个导出式右部的序号数组,以0为一个右部的结束标志,如TE' 为3，2，0, 里面的数字对应vns和vnt中的序号
int func_seq[][10] = { {3, 2, 0}, {6, 3, 2, 0}, {5, 4, 0}, {7, 5, 4, 0}, {8, 1, 9, 0}, {15, 0}, {13, 0},
					 {11, 0}, {12, 0}, {10, 0}, {16, 0} };

//每个非终结符号包含的所有导出式的序号， 里面的数字对应func_seq中的数组序号，如 1 对应func[0], 2 对应func[1]
int func_vn[][10] = { {1, 0}, {2, 11, 0}, {3, 0}, {4, 11, 0}, {5, 8, 0}, {6, 7, 0}, {9, 10, 0} };

//每个非终结符号包含的所有导出式字符串，“0”为空导出式
string func_name[][10] = { {"TE'", "#"}, {"ATE'","0", "#"}, {"FT'", "#"},
					  {"MFT'", "0", "#"}, {"(E)", "i", "#"}, {"+", "-", "#"},
					  {"*", "/", "#"} };




bool v_is_final(int seq); //判断一个符号是否为VT
bool head_is_final(FUNC func); //判断一个产生式的首符号是否为VT
bool funcs_has_null(int *funcs); //判断非终结符号的产生式中是否含有空产生式
bool first_has_null(int *first); //判断非终结符号的first集中是否含有空产生式
bool in_first(int *first, int seq); //判断是否在first集中
bool in_follow(int *follow, int seq); //判断是否在follow集中
void initial();
void find_first(VN vn);
void find_follow();
void cat_follow(int* s, int* r);
void cat_follow(int* s, int  r);
void table_initial();
void LL1();
int read_next();

void initial()
{
	int vt_len = sizeof(vts) / sizeof(vts[0]);
	int vn_len = sizeof(vns) / sizeof(vns[0]);
	int func_len = sizeof(func) / sizeof(func[0]);
	for (int i = 0; i < func_len; i++)
	{
		func[i].Seqs = func_seq[i];
		func[i].first = new int[vt_len];
		for (int j = 0; j < vt_len; ++j) func[i].first[j] = -1;
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
	while (funcs[i] != 0) //0为遍历一个非终结符号所有导出式结束的标志
	{
		if (funcs[i] == 11)  //11为空导出式的序号
			return true;
		i++;
	}
	return false;
}

bool first_has_null(int *first)
{
	int i = 0;
	while (first[i] >= 0) //-1为遍历一个first集结束的标志
	{
		if (first[i] == 16)  //11为空导出式的序号
			return true;
		i++;
	}
	return false;
}

bool in_first(int * first, int seq)
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

bool in_follow(int * follow, int seq)
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
	func[10].first[0] = 16;

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
			func[key - 1].first[0] = func[key - 1].Seqs[0]; //func[key - 1]能够推导出的first集
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
				func[key - 1].first[copy_index] = first_vn.first[copy_index];
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
		vns[i].follow[0] = 14;
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



void table_initial()
{
	int vn_len = sizeof(vns) / sizeof(vns[0]);
	int vt_len = sizeof(vts) / sizeof(vts[0]);
	table = new int*[vn_len];
	for (int i = 0; i < vn_len; i++)
	{
		table[i] = new int[vt_len];
		for (int j = 0; j < vt_len; ++j) table[i][j] = -1;
	}
//    for (int i = 0; i < vn_len; ++i) {
//        for (int j = 0; j < vt_len; ++j) {
//            printf("%d ", table[i][j]);
//        }
//        cout << '\n';
//    }
	for (int i = 0; i < vn_len; i++)
	{
		for (int j = 0; j < vt_len; j++)
		{
			int func_index = 0;//func_index遍历非终结符号的产生式
			bool flag = funcs_has_null(vns[i].Funcs); //当前非终结符号产生式中是否有空产生式
			while (vns[i].Funcs[func_index] != 0)
			{
				int func_seq = vns[i].Funcs[func_index] - 1;
				if (in_first(func[func_seq].first, j + 8))
				{
					table[i][j] = func_seq;
					break;
				}
				else if (flag)
				{
					if (in_follow(vns[i].follow, j + 8))
					{
						table[i][j] = 10;
						break;
					}
					else
					{
						table[i][j] = -1;
					}
				}
				else
				{
					table[i][j] = -1;
				}
				func_index++;
			}
		}
	}
//	for (int i = 0; i < vn_len; ++i) {
//        for (int j = 0; j < vt_len; ++j) {
//            printf("%d ", table[i][j]);
//        }
//        cout << '\n';
//	}
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

void print()
{
	int size = analysis.size();
	string V;
	for (int i = 0; i < size; i++)
	{

		if (v_is_final(analysis[i]))
		{
			V = vts[analysis[i] - 8].Key;
			cout << V << " ";
		}
		else
		{
			V = vns[analysis[i] - 1].Key;
			cout << V << " ";
		}
	}
	cout << endl;
}

void LL1()
{
	analysis.push_back(14);
	analysis.push_back(1);
	int current = read_next();
	int top = analysis.back();
	print();
	while (top != 14)
	{

		if (v_is_final(top))
		{
			if (top == current)
			{

				analysis.pop_back();
				top = analysis.back();
				current = read_next();
			}
			else
			{
				cout << "匹配出错" << endl;
				return;
			}
		}
		else
		{
			int func_seq = table[top - 1][current - 8];
			if (func_seq >= 0) //table[VN][VT]有值
			{
				if (func_seq == 10) //LL1表项为空产生式
				{
					analysis.pop_back();
					top = analysis.back();
				}
				else   //LL1表项不为空产生式
				{
					int func_len = 1;  //记录一个产生式右部的长度, 包括结束标志0
					while (func[func_seq].Seqs[func_len - 1] != 0)  //func[f - 1].Seqs是一个产生式右部符号的编号数组，
						func_len++;                                //E -> TE'即 （vn_index + 1）-> { 3, 2, 0}

					analysis.pop_back(); //把产生式左部出栈
					for (int i = func_len - 2; i >= 0; i--)
						analysis.push_back(func[func_seq].Seqs[i]);  //产生式右部倒序入栈

					top = analysis.back();
				}
			}
			else //table[VN][VT]出错
			{
				cout << "匹配错误" << endl;
				return;
			}
		}
		print();
	}

	cout << "匹配成功" << endl;
}

int main()
{

	fp.open("test.txt");
	initial();
	int vn_len = sizeof(vns) / sizeof(vns[0]);
	for (int i = 0; i < vn_len; i++)
	{
		find_first(vns[i]);
	}
	find_follow();
	table_initial();
	LL1();

	return 0;

}