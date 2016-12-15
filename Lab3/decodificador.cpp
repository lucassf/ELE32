#include <cstdio>
#include <queue>
#include <list>
using namespace std;

class Scrambler
{
public:
    Scrambler()
    {
    }
    void reset(){state = (1<<8)-1;}

    unsigned char getOutBit(unsigned char bit){
        return bit^=(state&1);
    }

    void scramble(){
        unsigned char sumbit = (state&1)^((state>>3)&1)^((state>>5)&1)^((state>>7)&1);
        int index=0;
        state >>= 1;
        state ^= ((1&sumbit)<<7);
    }

private:
    unsigned char state;
};

class ConvolutionEncoder{
private:
    unsigned char in;
    unsigned char memory;
public:
    ConvolutionEncoder(){
        reset();
    }
    void reset(){
        memory = in = 0;
    }
    void insert(unsigned char bit){
        memory >>= 1;
        memory |= (in << 5);
        in = bit;
    }
    void setState(unsigned char _in, unsigned char _memory){
        memory = _memory;
        in = _in;
    }
    unsigned char getOut1(){
        return ((memory >> 0) & 1)^((memory >> 3) & 1)^((memory >> 4) & 1)^((memory >> 5) & 1)^in;
    }
    unsigned char getOut2(){
        return ((memory >> 0) & 1)^((memory >> 1) & 1)^((memory >> 3) & 1)^((memory >> 4) & 1)^in;
    }
    unsigned char getMemory(){
        return memory;
    }
};

inline unsigned char hex2uchar(char carac){
    if (carac >= 'A' && carac <= 'F') carac -= 'A' - 10;
    else if (carac >= 'a' && carac <= 'f') carac -= 'a' - 10;
    else carac -= '0';
    return (unsigned char) carac;
}

queue<unsigned char> bitqueue;
Scrambler scrambler;
ConvolutionEncoder encoder;

bool addNext4Bits(FILE * in){
    char carac;
    if (feof(in)) return false;
    if(fscanf(in, "%c", &carac) == EOF) return false;
    unsigned char bit = hex2uchar(carac);
    bitqueue.push((((int)bit) >> 3) & 1);
    bitqueue.push((((int)bit) >> 2) & 1);
    bitqueue.push((((int)bit) >> 1) & 1);
    bitqueue.push((((int)bit) >> 0) & 1);
    return true;
}

unsigned char getBit(FILE * in){
    if (bitqueue.empty()){
        if (!addNext4Bits(in)) return 2;
    }
    unsigned char bit = bitqueue.front();
    bitqueue.pop();
    return bit;
}

bool findHeader(FILE * in){
    list<unsigned char> curbits;
    const char *headerstr = "11100101100001111111001011011101";
    //const char *headerstr = "00011010110011111111110000011101";
    int dist, bitsskipped = 0, i;
    unsigned char bit;
    while(true){
        while(curbits.size() < 32){
            bit = getBit(in);
            if ((int)bit > 1) return false;
            curbits.push_back(bit);
        }
        list<unsigned char>::iterator it;
        dist = 0;
        for(it = curbits.begin(), i=0; it!=curbits.end(); it++, i++){
            if ((*it) != (unsigned char)(headerstr[i] - '0')) dist++;
        }
        if (dist <= 4) break;
        bitsskipped++;
        curbits.pop_front();
    }
    //printf("header found after skipping %d bits, dist = %d\n", bitsskipped, dist);
    return true;
}

struct node{
    unsigned char state, fromc, fromi;
    unsigned char message1, message2;
    int cost, k;
} nodes[(1<<6)+9][1009];

void processNode(node & cur){
    int r;
    for(unsigned char c=0, out1, out2, newstate; c<=1; c++)
    {
        encoder.setState(c, cur.state);
        out1 = encoder.getOut1();
        out2 = encoder.getOut2();
        r = (out1!=cur.message1) + (out2!=cur.message2);
        encoder.insert(0);
        newstate = encoder.getMemory();
        if (nodes[newstate][cur.k+1].cost > cur.cost + r){
            nodes[newstate][cur.k+1].cost = cur.cost + r;
            nodes[newstate][cur.k+1].fromi = cur.state;
            nodes[newstate][cur.k+1].fromc = c;
        }
    }
}

bool processFile(const char * inpath, const char * outpath){
    FILE * in = fopen(inpath, "r");
    if (!in){
        printf("Nao pode ler %s\n", inpath);
        return false;
    }
    FILE * out = fopen(outpath, "w");
    if (!out){
        printf("Nao pode ler %s\n", outpath);
        return false;
    }

    while(!bitqueue.empty()) bitqueue.pop();

    unsigned char bit, message1, message2, mincoststate;
    list<unsigned char> ans;

    while(findHeader(in)){
        scrambler.reset();
        for(int i=0; i<32; i++)scrambler.scramble();
        for(int j=0; j<=1000; j++){
            if (j<1000){
                bit = getBit(in);
                if (bit > 1) return false;
                message1 = scrambler.getOutBit(bit);
                scrambler.scramble();
                bit = getBit(in);
                if (bit > 1) return false;
                message2 = scrambler.getOutBit(bit);
                scrambler.scramble();
            }
            else message1 = message2 = 0;
            //printf("%d%d", message1, message2);
            for(int i=0; i<(1<<6); i++){
                nodes[i][j].message1 = message1;
                nodes[i][j].message2 = message2;
                nodes[i][j].state = i;
                nodes[i][j].k = j;
                nodes[i][j].fromi = 2;
                nodes[i][j].fromc = 2;
                nodes[i][j].cost = (1<<30);
            }
            nodes[0][0].cost = 0;
            for(int i=0; i<(1<<6) && j>0; i++){
                processNode(nodes[i][j-1]);
            }
        }
        mincoststate = 0;
        for(int i=0; i<(1<<6); i++){
            if (nodes[i][1000].cost < nodes[mincoststate][1000].cost){
                mincoststate = i;
            }
        }
        //printf("min cost message = %d\n", nodes[mincoststate][1000].cost);
        ans.clear();
        for(int j=1000; j>0; j--){
            if (mincoststate < 0){
                printf("error, path not found.\n");
                return false;
            }
            ans.push_front(nodes[mincoststate][j].fromc);
            mincoststate = nodes[mincoststate][j].fromi;
        }
        //printf("printing sequence...\n");
        list<unsigned char>::iterator it;
        for(it = ans.begin(); it!=ans.end(); it++){
            fprintf(out, "%c", (*it) == 1 ? '1' : (*it) == 0 ? '0' : '?');
        }
        //printf("sequence printed\n");
    }

    fclose(in);
    fclose(out);

    return true;
}

int main(){
    if(!processFile("saida_100.txt", "decoded_100.txt")){
        printf("error!\n");
    }
}
