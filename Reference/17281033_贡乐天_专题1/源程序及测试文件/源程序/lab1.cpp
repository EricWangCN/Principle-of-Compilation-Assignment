/*************************************************
Author:贡乐天
Date:2020-04-03
Description:词法分析程序
**************************************************/

#include<iostream> 
#include<ctype.h> 
#include<string.h> 
#include<stdlib.h> 
#include <fstream>
#include <cassert>
#include <sstream>//使用istringstream所需要的头文件
using namespace std;

/*属性类，包括分界符标签、单词符号种别码以及单词符号字符串*/
typedef  struct restype {
	int tag;		/*分界符标签*/
	string num;		/*种别码*/
	string word;	/*单词符号*/
};


/*************************************************
Function:       // isDelimiter
Description:    // 判断单个字符为单分界符
Calls:          // 无
Input:          // 待判断字符
Output:         // 无输出参数
Return:         // 返回 restype类型变量
Others:         // tag为0表示不是分界符，为1表示是分界符
*************************************************/
restype isDelimiter(char word) {
	restype type;
	type.tag =0;//初始化为不分界符
	type.num = "0";
	char c1 = word;
	type.word = c1;
	
	if (c1 == '+') { type.tag = 1; type.num = "14"; type.word = c1; }
	else if (c1 == '-') { type.tag = 1; type.num = "15"; type.word = c1; }
	else if (c1 == '*') { type.tag = 1; type.num = "16"; type.word = c1; }
	else if (c1 == ';') { type.tag = 1; type.num = "17"; type.word = c1; }
	else if (c1 == '(') { type.tag = 1; type.num = "18"; type.word = c1; }
	else if (c1 == ')') { type.tag = 1; type.num = "19"; type.word = c1; }
	else if (c1 == '=') { type.tag = 1; type.num = "20"; type.word = c1; }
	else if (c1 == '>') { type.tag = 1; type.num = "21"; type.word = c1; }
	else if (c1 == '<') { type.tag = 1; type.num = "23"; type.word = c1; }
	else if (c1 == ':') { type.tag = 1; type.num = "26"; type.word = c1; }
	else if (c1 == '/') { type.tag = 1; type.num = "28"; type.word = c1; }
	else if (c1 == '!') { type.tag = 1; type.num = "30"; type.word = c1; }
	else if (c1 == '{') { type.tag = 1; type.num = "31"; type.word = c1; }
	else if (c1 == '}') { type.tag = 1; type.num = "32"; type.word = c1; }
	else if (c1 == '&') { type.tag = 1; type.num = "33"; type.word = c1; }
	else if (c1 == '|') { type.tag = 1; type.num = "34"; type.word = c1; }
	else if (c1 == '\n') { type.tag = 1; type.num = "35"; type.word = c1; }
	
	return type;
}

/*************************************************
Function:       // isWord
Description:    // 判断单词符号的种别码
Calls:          // 无
Input:          // 待判断的单词符号
Output:         // 无输出参数
Return:         // 返回 restype类型变量
Others:         // tag为0表示不是分界符，为1表示是分界符
*************************************************/
restype isWord(string word) {
	restype type;
	type.tag = 0;//确定不是分界符
	//全部转化为小写
	for (int i = 0; i < word.length(); ++i) {
		if (word[i] >= 'A' && word[i] <= 'Z') {
			word[i] = word[i] + 32;
		}
	}
	//保留字
	if (word == "void") { type.num = "1"; type.word = word; }
	else if (word == "int") { type.num = "2"; type.word = word; }
	else if (word == "float") { type.num = "3"; type.word = word; }
	else if (word == "double") { type.num = "4"; type.word = word; }
	else if (word == "if") { type.num = "5"; type.word = word; }
	else if (word == "else") { type.num = "6"; type.word = word; }
	else if (word == "for") { type.num = "7"; type.word = word; }
	else if (word == "do") { type.num = "8"; type.word = word; }
	else if (word == "while") { type.num = "9"; type.word = word; }
	else if (word == "return") { type.num = "10"; type.word = word; }
	else if (word == "break") { type.num = "11"; type.word = word; }

	else {
		int state = 1;
		for (int j = 0; j < word.length() && state == 1; j++) {
			if (word[j] == '0' || word[j] == '1' || word[j] == '2' || word[j] == '3' || word[j] == '4' || word[j] == '5' || word[j] == '6' ||
				word[j] == '7' || word[j] == '8' || word[j] == '9');
			else { state = 0; }
		}
		//数字
		if (state == 1) {
			type.num = "13"; type.word = word;
		}
		//未标识
		else {
			type.num = "12"; type.word = word;
		}
	}
	return type;
}

/*************************************************
Function:       // analyse
Description:    // 词法分析主体函数
Calls:          // isDelimiter；isWord；
Input:          // 从文件中读取的原字符串
Output:         // 无输出参数
Return:         // 返回得到二元式序列组成的结果字符串
Others:         // 完成词法分析，与注释未关闭导致的错误的检测
*************************************************/
string analyse(string my_str) {
	string result_str = "";//记录分析结果
	string word = "";
	int cnt = 0;
	//获取词
	my_str = my_str + ' ';
	int length = my_str.length();
	for (int i = 0; i < length; i++) {
		if (my_str[i] != ' ' && my_str[i] != '\r\t' && my_str[i] != '\0' && isDelimiter(my_str[i]).tag == 0 && (i + 1) != length ) {
			word = word + my_str[i];
			cnt++;
			
		}
		
		else {//开始分析
			if (cnt >= 32) {
				result_str = result_str + "\n用户定义的标识符最长不超过32个字符!\n";
				continue;
			}
			if (word != "") {
				restype judge1 = isWord(word);
				result_str = result_str + "(" + judge1.num + "," + judge1.word + ")";
				word = "";//对word进行清空,以便于记录下一个word
				cnt = 0;
			}
			//对当前分界符进行分析
			restype judge2 = isDelimiter(my_str[i]);
			if (judge2.tag == 1) {
				if (judge2.num == "21" && my_str[i + 1] == '=') {
					result_str = result_str + "(41, >= )";
					i++;//跳过双分界符的第二个字符
				}
				else if (judge2.num == "23" && my_str[i + 1] == '=') {
					result_str = result_str + "(42, <= )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "23" && my_str[i + 1] == '>') {
					result_str = result_str + "(43, <> )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "26" && my_str[i + 1] == '=') {
					result_str = result_str + "(44, := )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "14" && my_str[i + 1] == '=') {
					result_str = result_str + "(45, += )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "14" && my_str[i + 1] == '+') {
					result_str = result_str + "(46, ++ )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "15" && my_str[i + 1] == '=') {
					result_str = result_str + "(47, -= )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "15" && my_str[i + 1] == '-') {
					result_str = result_str + "(48, -- )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "16" && my_str[i + 1] == '=') {
					result_str = result_str + "(49, *= )";
					i++;//跳过双分界符的第二个字符				
				}
				else if (judge2.num == "28" && my_str[i + 1] == '=') {
					result_str = result_str + "(50, /= )";
					i++;//跳过双分界符的第二个字符		
				}
				else if (judge2.num == "33" && my_str[i + 1] == '&') {
					result_str = result_str + "(51, && )";
					i++;//跳过双分界符的第二个字符		
				}
				else if (judge2.num == "34" && my_str[i + 1] == '|') {
					result_str = result_str + "(52, || )";
					i++;//跳过双分界符的第二个字符		
				}
				else if (judge2.num == "21" && my_str[i + 1] == '>') {
					result_str = result_str + "(53, >> )";
					i++;//跳过双分界符的第二个字符		
				}
				else if (judge2.num == "23" && my_str[i + 1] == '<') {
					result_str = result_str + "(54, << )";
					i++;//跳过双分界符的第二个字符		
				}
				else if (judge2.num == "30" && my_str[i + 1] == '=') {
					result_str = result_str + "(55, != )";
					i++;//跳过双分界符的第二个字符		
				}
				//******************对换行符进行处理********************
				else if (judge2.num == "35" ) {
					result_str = result_str + "\n";
						
				}
				//******************对注释进行处理**********************
				else if (judge2.num == "28" && my_str[i + 1] == '*') {
					result_str = result_str + "(注释开始, /* )";
					i++;//跳过双分界符的第二个字符
					int state = 0;
					int j = i + 1;
					for (; j < length && state == 0; j++) {
						if (my_str[j] == '*' && my_str[j + 1] == '/') {
							state = 1;
						}
					}
					if (state == 1) {//注释有结尾
						result_str = result_str + "(注释关闭, */ )";
						i = j;
					}
					else {
						result_str = result_str + "(出现错误，注释未关闭!)";
						i = length;
					}
				}//开始分析	
				else {
					result_str = result_str + "(" + judge2.num + ", " + judge2.word + " )";
				}
			}//获取词
		}
	}
	return result_str;
}
/*************************************************
Function:       // writeResult
Description:    // 将分析结果写入文件
Calls:          // 无
Input:          // 结果字符串
Output:         // 结果字符串写入结果文件
*************************************************/
void writeResult(string result_str) {
	ofstream out("output2.txt");
	if (out.is_open())
	{
		out << result_str << endl;
		out.close();
	}
}

/*************************************************
Function:       // readTxt
Description:    // 读取文件中的字符串
*************************************************/
string readTxt(string file)
{
	ifstream infile;
	infile.open(file.data());   //将文件流对象与文件连接起来 
	assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 

	if (!infile) {
		cerr << "读取文件出错了！" << endl;
		exit(1);
	}

	char ch;
	string my_str = "";
	infile >> noskipws;
	while (infile.get(ch))//从文件读取字符进来  
	{
		my_str += ch;
	}
	infile.close();             //关闭文件输入流 
	return my_str;
}

int main() {
	string my_str = readTxt("input2.txt");
	string result_str = analyse(my_str);
	writeResult(result_str);
	return 0;
}