#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <queue>
#include <fstream>
#include <bitset>
using namespace std;

struct node {
	node* nextnode0;
	node* nextnode1;
	int value;
	char car;
};

struct node_comparison
{
	bool operator()(const node* a, const node* b) const
	{
		return a->value > b->value;
	}
};

typedef node* nodepointer;
typedef pair<int, nodepointer> ici;

void recurs(nodepointer n, string text, map<char, string> &coded,map<string,char> &revcoded) {
	if (n==NULL)return;
	if (n->nextnode1 == NULL) {
		coded[n->car] = text;
		revcoded[text] = n->car;
		return;
	}
	recurs(n->nextnode0, text + "0", coded,revcoded);
	recurs(n->nextnode1, text + "1", coded,revcoded);
}

string convertStringtoBit(string s) {
	int val = 0;
	int acum = 0;
	string ans = "";
	ans.push_back(s.size()%8);
	for (int i = 0; i<s.size(); i++) {
		acum = (s[i] - '0') + 2 * acum, val++;
		if (val == 8)ans.push_back(acum), acum = 0, val = 0;
	}for (int i=s.size()-val;i<s.size();i++)ans.push_back(s[i]);
	return ans;
}

string convertBitToString(string s) {
	int val,i;
	string ans = "";
	bitset<8>* bs;
	val = s[0];
	for (i = 1; i<s.size()-val; i++) {
		bs = new bitset<8>(s[i]);
		ans += bs->to_string();
	}while(i<s.size())ans.push_back(s[i]),i++;
	return ans;
}

string decodificate(string s,map<string,char> revcoded) {
	string t="",ans="";
	for (int i = 0; i < s.size(); i++) {
		t.push_back(s[i]);
		if (revcoded.count(t))ans.push_back(revcoded[t]),t="";
	}
	return ans;
}

void codificate(string input, string output) {
	//Variables declaration
	map<char, int> symbols;
	map<char, string> coded;
	map<string, char> revcoded;
	int cnt = 0;
	nodepointer u, root, v1, v2;
	priority_queue<nodepointer, vector<nodepointer>, node_comparison > textqueue;
	ifstream infile("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 1\\Livros\\" + input);
	string text = "", s;

	//Get text ans symbols
	while (getline(infile, s))
	{
		text += s;
		text.push_back('\n');
	}
	if (!s.empty())text.pop_back();
	symbols.clear();
	for (int i = 0; i<(int)text.size(); i++) {
		symbols[text[i]]++;
	}
	//Set priority queue with frst elements
	map<char, int>::iterator it;
	for (it = symbols.begin(); it != symbols.end(); it++) {

		if (it->second != 0) {
			u = (nodepointer)malloc(sizeof(node));
			u->nextnode0 = NULL;
			u->nextnode1 = NULL;
			u->car = it->first;
			u->value = it->second;
			textqueue.push(u);
		}
	}

	//Add symbols
	v1 = NULL;
	while (!textqueue.empty()) {
		v1 = textqueue.top();
		textqueue.pop();
		if (textqueue.empty())break;
		v2 = textqueue.top();
		textqueue.pop();
		u = (nodepointer)malloc(sizeof(node));
		u->nextnode0 = v1;
		u->nextnode1 = v2;
		u->car = 0;
		u->value = v1->value + v2->value;
		textqueue.push(u);
	}
	root = v1;

	//Recursion to obtain the code symbols
	recurs(root, "", coded,revcoded);

	//Print coded symbols on screen/file
	string ans = "";
	ofstream outfile(output);
	for (int i = 0; i < (int)text.size(); i++) {
		ans += coded[text[i]];
	}
	outfile<<convertStringtoBit(ans);
}

int main() {
	string data[] = { "English.txt","English2.txt","English3.txt","Frances.txt",
		"Frances2.txt","Frances3.txt","German.txt","German2.txt","German3.txt",
		"Portugues.txt","Portugues2.txt","Portugues3.txt" };
	string output[] = { "EnglishO.txt","English2O.txt","English3O.txt","FrancesO.txt",
		"Frances2O.txt","Frances3O.txt","GermanO.txt","German2O.txt","German3O.txt",
		"PortuguesO.txt","Portugues2O.txt","Portugues3O.txt" };
	for (int i = 0; i<12; i++)codificate(data[i], output[i]);
}
