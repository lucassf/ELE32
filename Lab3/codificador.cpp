#include <cstdio>

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
    unsigned char getOut1(){
        return ((memory >> 0) & 1)^((memory >> 3) & 1)^((memory >> 4) & 1)^((memory >> 5) & 1)^in;
    }
    unsigned char getOut2(){
        return ((memory >> 0) & 1)^((memory >> 1) & 1)^((memory >> 3) & 1)^((memory >> 4) & 1)^in;
    }
};

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

ConvolutionEncoder encoder;
Scrambler scrambler;

inline char uchar2hex(unsigned char outbit){
    if (outbit >= 10) outbit += 'A' - 10;
    else outbit += '0';
    return (char) outbit;
}

inline unsigned char hex2uchar(char carac){
    if (carac >= 'A' && carac <= 'F') carac -= 'A' - 10;
    else carac -= '0';
    return (unsigned char) carac;
}

bool printOutbit(unsigned char outbit, FILE * out){
    char carac = outbit;
    outbit = 0;
    outbit |= (scrambler.getOutBit((carac >> 3) & 1) << 3);
    scrambler.scramble();
    outbit |= (scrambler.getOutBit((carac >> 2) & 1) << 2);
    scrambler.scramble();
    outbit |= (scrambler.getOutBit((carac >> 1) & 1) << 1);
    scrambler.scramble();
    outbit |= (scrambler.getOutBit((carac >> 0) & 1) << 0);
    scrambler.scramble();
    carac = uchar2hex(outbit);
    fprintf(out, "%c", carac);
    return true;
}

bool printCarac(char carac, FILE * out){
    printOutbit(hex2uchar(carac), out);
    return true;
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

    encoder.reset();
    scrambler.reset();

    int counter = 0, nbits = 0;
    unsigned char bit, outbit = 0, p = 8;
    char carac;
    while(true){
        if (!feof(in)){
            if(fscanf(in, "%c", &carac) != EOF){
                if (carac != '1' && carac != '0') continue;
                bit = (carac == '1' ? 1 : 0);
            }
            else if (counter == 0){
                break;
            }
            else bit = 0;
        }
        else if(counter == 0) break;
        else bit = 0;

        nbits++;

        if (counter == 0){
            encoder.reset();
            scrambler.reset();
            printCarac('1', out);
            printCarac('A', out);
            printCarac('C', out);
            printCarac('F', out);
            printCarac('F', out);
            printCarac('C', out);
            printCarac('1', out);
            printCarac('D', out);
        }

        encoder.insert(bit);
        outbit |= (encoder.getOut1()*p);
        p >>= 1;
        outbit |= (encoder.getOut2()*p);
        p >>= 1;
        if (p == 0){
            p = 8;
            carac = uchar2hex(outbit);
            printCarac(carac, out);
            outbit = 0;
        }

        counter++;
        if (counter == 1000){
            counter = 0;
        }
    }

    fclose(in);
    fclose(out);

    printf("nbits = %d\n", nbits);

    return true;
}

int main(){
    processFile("code.txt", "encoded.txt");
    return 0;
}
