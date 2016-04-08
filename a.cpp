#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>
#include <cassert>
#include <algorithm>
using namespace std;
//#define DEBUG
#ifndef DEBUG
#define RELEASE
#endif

typedef pair<string,vector<string> > svs;
/*
 * grammer parsing
 */
struct Grammer { 
    multimap<string,vector<string> > g;
    multimap<vector<string>,string> bg;
    Grammer() {}
    void insert(svs curGrammer) {
        g.insert(curGrammer);
        bg.insert(pair<vector<string>, string>(curGrammer.second,curGrammer.first));
    }
    void printG() {
        for ( map<string,vector<string> >::iterator it=g.begin();it!=g.end();it++ ) {
            printf("%s -> ",it->first.c_str());
            for ( int i = 0 ; i < (int)it->second.size() ; i++ ) 
                printf("%s ",it->second[i].c_str());
            puts("");
        }
    }
    void printBg() {
        for ( multimap<vector<string>,string>::iterator it=bg.begin();it!=bg.end();it++ ) {
            for ( int i = 0 ; i < (int)it->first.size() ; i++ ) 
                printf("%s ",it->first[i].c_str());
            printf("-> ");
            printf("%s\n",it->second.c_str());
        }
    }
};
svs parseGrammer(char *s) {
    vector<string> vs;
    for ( char *p = strtok(s," \t") ; p ; p=strtok(NULL," \t") ) {
        if ( string(p) == "->" ) continue;
        vs.push_back(string(p));
    }
    svs ret;
    ret.first = vs[0];
    for ( int i = 1 ; i < (int)vs.size() ; i++ ) 
        ret.second.push_back(vs[i]);
    return ret;
}
Grammer initGrammer(string grammerFilename) {
    FILE *fp = fopen(grammerFilename.c_str(),"r");
    Grammer grammer;
    for ( char buf[1024] ; ~fscanf(fp,"%[^\n]\n",buf) ; ) {
        svs curParsedGrammer = parseGrammer(buf);
        grammer.insert(curParsedGrammer);
    }
    fclose(fp);
    return grammer;
}

/*
 * create cky table
 */
struct Cky {
    vector<vector<set<string> > > table;
    Cky(){}
    Cky(int n) {
        for ( int i = 0 ; i < n ; i++ ) {
            vector<set<string> > cur;
            for ( int j = 0 ; j < n ; j++ ) {
                set<string> curS;
                curS.clear();
                cur.push_back(curS);
            }
            table.push_back(cur);
        }
    }
    void print() {
        for ( int i = 0 ; i < (int)table.size() ; i++ ) {
            for ( int j = 0 ; j < (int)table[i].size() ; j++ ) {
                printf("[");
                for ( set<string>::iterator it=table[i][j].begin();it!=table[i][j].end();it++ ) 
                    printf("%s%s",it==table[i][j].begin()?"":",",(*it).c_str());
                printf("]");
            }
            puts("");
        }
    }
};
vector<string> parseString(char *s) {
    vector<string> ret;
    for ( char *p = strtok(s," \t") ; p ; p=strtok(NULL," \t" ) )
        ret.push_back(string(p));
    return ret;
}
vector<vector<string> > initInputString(string inputFilename) {
    FILE *fp = fopen(inputFilename.c_str(),"r");
    vector<vector<string> > ret;
    for ( char buf[1024] ; ~fscanf(fp,"%[^\n]\n",buf) ; ) {
        for ( int i = 0 ; buf[i] ; i++ ) 
            if ( buf[i] == '.' ) buf[i] = 0;
        vector<string> cur = parseString(buf);
        ret.push_back(cur);
    }
    fclose(fp);
    return ret;
}
void printParsedInputString(vector<string>& parsedInputString) {
    for ( int i = 0 ; i < (int)parsedInputString.size() ; i++ ) 
        printf("%s ",parsedInputString[i].c_str());
    puts("");
}
Cky calculateCky(Grammer& grammer,vector<string>& parsedInputString) {
    Cky cky((int)parsedInputString.size());    
    
    pair<multimap<vector<string>,string>::iterator,multimap<vector<string>,string>::iterator> foundGrammer;
    for ( int i = 0 ; i < (int)parsedInputString.size() ; i++ ) {
        vector<string> cur;
        cur.push_back(parsedInputString[i]);
        foundGrammer = grammer.bg.equal_range(cur);
        for ( multimap<vector<string>,string>::iterator it=foundGrammer.first;it!=foundGrammer.second;it++ ) {
            cky.table[0][i].insert(it->second);
        }
    }
    int n = (int)parsedInputString.size();
    for ( int l = 2 ; l <= n ; l++ ) {
        for ( int j = 0 ; j <= n-l ; j++ ) {
            for ( int i = l-2 ; i >= 0 ; i-- ) {
                /* self parsing */
                set<string> copy = cky.table[i][j];
                for ( set<string>::iterator it=copy.begin();it!=copy.end();it++ ) {
                    vector<string> cur;
                    cur.push_back(*it);
                    foundGrammer = grammer.bg.equal_range(cur);
                    for ( multimap<vector<string>,string>::iterator it=foundGrammer.first;it!=foundGrammer.second; it++ ) 
                        cky.table[i][j].insert(it->second);
                }
                copy.clear();
                copy = cky.table[l-i-2][j+i+1];
                for ( set<string>::iterator it=copy.begin();it!=copy.end();it++ ) {
                    vector<string> cur;
                    cur.push_back(*it);
                    foundGrammer = grammer.bg.equal_range(cur);
                    for ( multimap<vector<string>,string>::iterator it=foundGrammer.first;it!=foundGrammer.second; it++ ) 
                        cky.table[l-i-2][j+i+1].insert(it->second);
                }
                /* self parsing end */

                set<string> lhs = cky.table[i][j];
                set<string> rhs = cky.table[l-i-2][j+i+1];
                for ( set<string>::iterator it1=lhs.begin();it1!=lhs.end();it1++ ) {
                    for ( set<string>::iterator it2=rhs.begin();it2!=rhs.end();it2++ ) {
                        vector<string> cur;
                        cur.push_back(*it1);
                        cur.push_back(*it2);
                        foundGrammer = grammer.bg.equal_range(cur);
                        for ( multimap<vector<string>,string>::iterator it=foundGrammer.first;it!=foundGrammer.second; it++ ) 
                            cky.table[l-1][j].insert(it->second);
                    }
                }
            }
        }
    }
    return cky;
}
int main(int argc,char *argv[]) {
    string grammerFilename = "grammer.txt";
    string inputFilename = "input.txt";
    if ( argc == 3 ) {
        char buf[1024];
        sscanf(argv[1],"%s",buf);
        grammerFilename = buf;
        sscanf(argv[2],"%s",buf);
        inputFilename = buf;
    }
    Grammer grammer = initGrammer(grammerFilename);
    vector<vector<string> > parsedInputString = initInputString(inputFilename);
#ifdef DEBUG
    grammer.printG();
    grammer.printBg();
    printParsedInputString(parsedInputString[0]);
#endif
    Cky cky = calculateCky(grammer,parsedInputString[0]);
    cky.print();
    return 0;
}
