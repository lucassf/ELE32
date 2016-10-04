#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <bitset>
#include <ctime>
using namespace std;
typedef pair<int, string> ii;

string add_zeros(string val, int bits)
{
	string ret = "";
	bits -= val.size();
	for (int i = 0; i<bits; i++)ret += "0";
	return ret + val;
}

string int_to_bin(int val)
{
	string s = "", ret = "";
	while (val>0)
	{
		if (val & 1 == 1)s += "1";
		else s += "0";
		val /= 2;
	}
	for (int i = s.size() - 1; i >= 0; i--)ret.push_back(s[i]);
	return ret;
}

int bin_to_int(string val) {
	int ret = 0;
	for (int i = 0; i < val.size(); i++) {
		ret = ret * 2 + val[i] - '0';
	}
	return ret;
}

string convertStringtoByte(string s) {
	int val = 0;
	int acum = 0;
	string ans = "";
	ans.push_back(s.size() % 8);
	for (int i = 0; i<s.size(); i++) {
		acum = (s[i] - '0') + 2 * acum, val++;
		if (val == 8)ans.push_back(acum), acum = 0, val = 0;
	}for (int i = s.size() - val; i<s.size(); i++)ans.push_back(s[i]);
	return ans;
}

string convertByteToString(string s) {
	int val, i;
	string ans = "";
	bitset<8>* bs;
	val = s[0];
	for (i = 1; i<s.size() - val; i++) {
		bs = new bitset<8>(s[i]);
		ans += bs->to_string();
	}while (i<s.size())ans.push_back(s[i]), i++;
	return ans;
}

bool aNumber(char val) {
	return val >= '0'&&val <= '9';
}

//Compression function
void compress(int maxmapsize, string filename)
{
	string s, v, text, sprev, compact, ans;
	stringstream sstr;
	vector<char> symbols;
	int mapsize;

	map<string, ii> dic;
	int maxindex = 1, i, j, b;
	ii aux;

	ofstream outfile;
	ifstream infile;

	infile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 1\\Livros\\" + filename,ios::binary);
	outfile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 2\\LempelZivCompression\\" + filename,ios::binary);

	ans = "";
	text = "";

	sstr << (infile.rdbuf());
	text = sstr.str();

    //Save symbols
	for (i = 0; i<text.size(); i++)
	{
		v = "";
		v.push_back(text[i]);
		if (dic.count(v) <= 0)
		{
			symbols.push_back(text[i]);
			dic[v] = ii(maxindex, int_to_bin(maxindex));
			maxindex++;
			ans.push_back(text[i]);
		}
	}

	ans += "..";
	string trp = "";
	mapsize = symbols.size();
	ans += to_string(maxmapsize);
	ans += "..";

    //Compress file size using Lempel Ziv algorithm.
	for (i = 0; i<text.size();)
	{
		if (mapsize == maxmapsize) {
			dic.clear();
			maxindex = 1;
			for (int index = 0; index<symbols.size(); index++) {
				v = "";
				v.push_back(symbols[index]);
				dic[v] = ii(maxindex, int_to_bin(maxindex));
				maxindex++;
			}
			mapsize = symbols.size();
		}

		sprev = "";
		v = "";
		v.push_back(text[i]);

		while (i<text.size() && dic.count(v)) {
			sprev = v;
			i++;
			if (i<text.size())v.push_back(text[i]);
		}
		if (i == text.size())
		{
			break;
		}


		b = ceil((double)log2(maxindex));
		j = dic[sprev].first;
		compact = add_zeros(int_to_bin(j), b);
		dic[v] = ii(maxindex++, compact);
		trp += compact;
		mapsize++;
	}

	ans += convertStringtoByte(trp);
	ans.push_back(26);

	ans += v;
	outfile.write(ans.c_str(),ans.size());

	infile.close();
	outfile.close();
}


//Decompression function
void decompress(string filename) {

	ofstream outfile;
	ifstream infile;

	infile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 2\\LempelZivCompression\\" + filename,ios::binary);
	outfile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 2\\LempelZivDecompression\\" + filename,ios::binary);

	string text,s, sprev,ans;
	stringstream sstr;
	vector<char> symbols;
	vector<string> dic;
	int maxmapsize, i,mapsize,previndex;
	int lastindex, b;


	sstr << (infile.rdbuf());
	text = sstr.str();

	//Read Symbols
	i = 0;
	while (i<text.size()-1&&(text[i] != '.' || text[i + 1] != '.'||!aNumber(text[i+2])))symbols.push_back(text[i]), i++;
	i += 2;
	//Read maximum size of map
	s = "";
	while (i<text.size() && (text[i] != '.' || text[i + 1] != '.'))s.push_back(text[i]), i++;
	maxmapsize = stoi(s);
	s = "";
	i += 2;

	for (int j = text.size() - 1; j > 0; j--) {
		if (text[j] == 26) {
			lastindex = j;
			break;
		}
	}
	//Read compressed bytes
	while (i<lastindex)s.push_back(text[i]), i++;
	if (i == text.size())return;
	//Convert bytes to 0s and 1s
	sprev = convertByteToString(s);
	i ++;
	ans = "";

	//Decompress file
	mapsize = maxmapsize;
	previndex = -1;

	for (int j = 0; j < sprev.size();) {
		if (mapsize == maxmapsize) {
			dic.clear();
			dic.push_back("0");//Place holder
			for (int index = 0; index<symbols.size(); index++) {
				s = "";
				s.push_back(symbols[index]);
				dic.push_back(s);
			}
			mapsize = symbols.size();
		}
		int jlimit = j + ceil((double)log2(mapsize+1));
		s = "";
		while (j < jlimit)s.push_back(sprev[j]), j++;

		if (previndex != -1) {
			dic[previndex] += dic[bin_to_int(s)][0];
		}

		dic.push_back(dic[bin_to_int(s)]);
		ans+=dic[mapsize + 1];

		previndex = mapsize+1;
		mapsize++;
	}

	//Add final characters
	while (i<text.size())ans.push_back(text[i]), i++;
	outfile << ans;

	outfile.close();
	infile.close();
}

//Convert all text letters into bytes
void challenge_compress(int maxmapsize,string filename){

    ofstream outfile;
	ifstream infile;

    infile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 1\\Livros\\" + filename,ios::binary);
	outfile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 1\\Livros\\bits" + filename,ios::binary);

    string text="";
	stringstream sstr;

	sstr << (infile.rdbuf());
	text.push_back(0);
	text+=sstr.str();
	outfile<<convertByteToString(text);

	compress(maxmapsize,"bits"+filename);

}

//Decompress a file that was consisted of only bit values
void challenge_decompress(string filename){
    ofstream outfile;
	ifstream infile;

    infile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 2\\LempelZivDecompression\\bits" + filename,ios::binary);
	outfile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 2\\LempelZivDecompression\\" + filename,ios::binary);

	decompress("bits"+filename);

    string text;
	stringstream sstr;

	sstr << (infile.rdbuf());
	text=sstr.str();

	text = convertStringtoByte(text);
	text.erase(0,1);
	outfile<<text;
}

int main() {

    //Time Data file
    ofstream outfile;
    clock_t start;
	outfile.open("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 2\\timedata.txt");

	string data[] = { "English.txt","English2.txt","English3.txt","Frances.txt",
		"Frances2.txt","Frances3.txt","German.txt","German2.txt","German3.txt",
		"Portugues.txt","Portugues2.txt","Portugues3.txt" };

	int maxmapsize;
	cin>>maxmapsize;

	for (int i = 0; i<12; i++) {
        outfile<<data[i]<<endl;
        start = clock();
        compress(maxmapsize, data[i]);
		outfile<<"Compress: "<<( std::clock() - start ) / (double) CLOCKS_PER_SEC<<endl;
        start = clock();
		decompress(data[i]);
		outfile<<"Decompress: "<<( std::clock() - start ) / (double) CLOCKS_PER_SEC<<endl;
		start = clock();
		challenge_compress(maxmapsize, data[i]);
		outfile<<"Challenge Compress: "<<( std::clock() - start ) / (double) CLOCKS_PER_SEC<<endl;
		start = clock();
		challenge_decompress(data[i]);
		outfile<<"Challenge Decompress: "<<( std::clock() - start ) / (double) CLOCKS_PER_SEC<<endl<<endl;
	}
}
