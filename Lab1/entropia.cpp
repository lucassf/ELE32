#include <cstdio>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

typedef pair<char,char> cc;

void getData(string input,FILE* output){
    ifstream infile("C:\\Users\\asus\\Documents\\ITA\\3º Ano - 2º Bim\\ELE-32\\Lab 1\\Livros\\"+input);

    string s;
    char caract,caract1,prev;
    int quantity=0,cqtt,cont=0,val;
    map<char,int> occur;
    map<cc,int> corr;
    double entropy,proby,probxy,condentropy;

    while (getline(infile,s))
    {
        prev=0;
        for (int i=0; i<s.length(); i++)
        {
            occur[s[i]]++;
            if (prev!=0)
            {
                corr[cc(prev,s[i])]++;
            }
            cont++;
            prev = s[i];
        }
    }
    map<char,int>::iterator it,jt;
    entropy = 0;
    condentropy = 0;
    for (it=occur.begin(); it!=occur.end(); it++)
    {
        caract = it->first;
        cqtt = it->second;
        proby = (double)cqtt/cont;
        entropy -= proby*log2(proby);
        cqtt=0;
        for (jt=occur.begin(); jt!=occur.end(); jt++)
        {
            caract1 = jt->first;
            cqtt+=corr[cc(caract,caract1)];
        }
        for (jt=occur.begin(); jt!=occur.end(); jt++){
            caract1 = jt->first;
            val = corr[cc(caract,caract1)];
            probxy = (double)val/cqtt;
            if (val!=0)condentropy-=proby*probxy*log2(probxy);
            if (caract=='q')printf("%c %c %lf\n",caract,caract1,probxy);
        }
    }
    fprintf(output,"%s\n",input.c_str());
    fprintf(output,"Entropia: %lf\nEntropia Conjunta:%lf\nIformacao mutua: %lf\n\n",entropy,condentropy,entropy-condentropy);
}

int main()
{
    string data[]={"English.txt","English2.txt","English3.txt","Frances.txt",
    "Frances2.txt","Frances3.txt","German.txt","German2.txt","German3.txt",
    "Portugues.txt","Portugues2.txt","Portugues3.txt"};
    FILE* output = fopen("output.txt","w");
    for (int i=0;i<12;i++)getData(data[i],output);
    int n;
    cin>>n;
}

