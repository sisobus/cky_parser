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
//#define UNIX_ENVIRONMENT
#define SHOW_SCREEN
#define SHOW_GRAMMER_USED

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
            fprintf(stdout,"%s -> ",it->first.c_str());
            for ( int i = 0 ; i < (int)it->second.size() ; i++ ) {
                fprintf(stdout,"%s ",it->second[i].c_str());
            }
            fprintf(stdout,"\n");
        }
    }
    void printBg() {
        for ( multimap<vector<string>,string>::iterator it=bg.begin();it!=bg.end();it++ ) {
            for ( int i = 0 ; i < (int)it->first.size() ; i++ ) 
                fprintf(stdout,"%s ",it->first[i].c_str());
            fprintf(stdout,"-> ");
            fprintf(stdout,"%s\n",it->second.c_str());
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
    vector<vector<set<pair<string,string> > > > table;
    Cky(){}
    Cky(int n) {
        for ( int i = 0 ; i < n ; i++ ) {
            vector<set<pair<string,string> > > cur;
            for ( int j = 0 ; j < n ; j++ ) {
                set<pair<string,string> > curS;
                cur.push_back(curS);
            }
            table.push_back(cur);
        }
    }
    void print() {
        for ( int i = 0 ; i < (int)table.size() ; i++ ) {
            for ( int j = 0 ; j < (int)table[i].size() ; j++ ) {
                fprintf(stdout,"[");
                for ( set<pair<string,string> >::iterator it=table[i][j].begin();it!=table[i][j].end();it++ ) 
                    fprintf(stdout,"%s%s",it==table[i][j].begin()?"":",",(*it).first.c_str());
                fprintf(stdout,"]");
            }
            fprintf(stdout,"\n");
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
void printParsedInputString(FILE* fp,vector<string>& parsedInputString) {
    for ( int i = 0 ; i < (int)parsedInputString.size() ; i++ ) 
        fprintf(fp,"%s ",parsedInputString[i].c_str());
    fprintf(fp,"\n");
}
void selfParsing(FILE* fp,Grammer& grammer,Cky& cky,int i,int j) {
    pair<multimap<vector<string>,string>::iterator,multimap<vector<string>,string>::iterator> foundGrammer;
    set<pair<string,string> > copy = cky.table[i][j];
    for ( set<pair<string,string> >::iterator it=copy.begin();it!=copy.end();it++ ) {
        vector<string> cur;
        cur.push_back(it->first);
        foundGrammer = grammer.bg.equal_range(cur);
        for ( multimap<vector<string>,string>::iterator it1=foundGrammer.first;it1!=foundGrammer.second; it1++ ) {
#ifdef SHOW_GRAMMER_USED
            fprintf(fp,"%s -> %s\n",it1->second.c_str(),it->first.c_str());
#ifdef SHOW_SCREEN
            fprintf(stdout,"%s -> %s\n",it1->second.c_str(),it->first.c_str());
#endif
#endif
            string addedClass = "("+it1->second+" "+it->second+")";
            cky.table[i][j].insert(pair<string,string>(it1->second,addedClass));
        }
    }
}
Cky calculateCky(FILE *fp,Grammer& grammer,vector<string>& parsedInputString) {
    Cky cky((int)parsedInputString.size());    
    
    pair<multimap<vector<string>,string>::iterator,multimap<vector<string>,string>::iterator> foundGrammer;
    for ( int i = 0 ; i < (int)parsedInputString.size() ; i++ ) {
        vector<string> cur;
        cur.push_back(parsedInputString[i]);
        foundGrammer = grammer.bg.equal_range(cur);
        for ( multimap<vector<string>,string>::iterator it=foundGrammer.first;it!=foundGrammer.second;it++ ) {
#ifdef SHOW_GRAMMER_USED
            fprintf(fp,"%s -> %s\n",it->second.c_str(),parsedInputString[i].c_str());
#ifdef SHOW_SCREEN
            fprintf(stdout,"%s -> %s\n",it->second.c_str(),parsedInputString[i].c_str());
#endif
#endif
            string addedClass = "("+it->second+" "+parsedInputString[i]+")";
            cky.table[0][i].insert(pair<string,string>(it->second,addedClass));
        }
    }
    int n = (int)parsedInputString.size();
    for ( int l = 2 ; l <= n ; l++ ) {
        for ( int j = 0 ; j <= n-l ; j++ ) {
            for ( int i = l-2 ; i >= 0 ; i-- ) {
                /* self parsing */
                selfParsing(fp,grammer,cky,i,j);
                selfParsing(fp,grammer,cky,l-i-2,j+i+1);
                /* self parsing end */

                set<pair<string,string> > lhs = cky.table[i][j];
                set<pair<string,string> > rhs = cky.table[l-i-2][j+i+1];
                for ( set<pair<string,string> >::iterator it1=lhs.begin();it1!=lhs.end();it1++ ) {
                    for ( set<pair<string,string> >::iterator it2=rhs.begin();it2!=rhs.end();it2++ ) {
                        vector<string> cur;
                        cur.push_back(it1->first);
                        cur.push_back(it2->first);
                        foundGrammer = grammer.bg.equal_range(cur);
                        for ( multimap<vector<string>,string>::iterator it=foundGrammer.first;it!=foundGrammer.second; it++ ) {
#ifdef SHOW_GRAMMER_USED
                            fprintf(fp,"%s -> %s %s\n",it->second.c_str(),it1->first.c_str(),it2->first.c_str());
#ifdef SHOW_SCREEN
                            fprintf(stdout,"%s -> %s %s\n",it->second.c_str(),it1->first.c_str(),it2->first.c_str());
#endif
#endif
                            string addedClass = "("+it->second+" "+it1->second+it2->second+")";
                            cky.table[l-1][j].insert(pair<string,string>(it->second,addedClass));
                        }
                    }
                }
            }
        }
    }
    return cky;
}
void printParseTree(FILE* fp,Cky& cky,vector<string>& parsedInputString) {
    int n = (int)parsedInputString.size();
    for ( int i = 0 ; i < n ; i++ ) {
        if ( cky.table[n-1][i].empty() ) continue;
        set<pair<string,string> > ans = cky.table[n-1][i];
        for ( set<pair<string,string> >::iterator it=ans.begin();it!=ans.end();it++ ) {
            if ( it->first != "S" ) continue;
            fprintf(fp,"%s\n",it->second.c_str());
        }
    }
}
int main(int argc,char *argv[]) {
    /*
     *  default filename
     */
    string grammerFilename = "grammer.txt";
    string inputFilename = "input.txt";
    string outputFilename = "output.txt";
    /*
     *  you can change default filename to argument 
     *  argv[1] : grammer filename
     *  argv[2] : input filename
     */
#ifdef UNIX_ENVIRONMENT
    if ( argc == 3 ) {
        char buf[1024];
        sscanf(argv[1],"%s",buf);
        grammerFilename = buf;
        sscanf(argv[2],"%s",buf);
        inputFilename = buf;
    }
#endif
    FILE* fp = fopen(outputFilename.c_str(),"w");

    /*
     *  grammer initializing
     *  input string parsing
     */
    Grammer grammer = initGrammer(grammerFilename);
    vector<vector<string> > parsedInputString = initInputString(inputFilename);
#ifdef DEBUG
    grammer.printG();
    grammer.printBg();
    printParsedInputString(parsedInputString[0]);
#endif
    for ( int i = 0 ; i < (int)parsedInputString.size() ; i++ ) {
        fprintf(fp,"/*\n * cky parser start\n * input string : ");
        printParsedInputString(fp,parsedInputString[i]);
        fprintf(fp," */\n");
#ifdef SHOW_SCREEN
        fprintf(stdout,"/*\n * cky parser start\n * input string : ");
        printParsedInputString(stdout,parsedInputString[i]);
        fprintf(stdout," */\n");
#endif
        Cky cky = calculateCky(fp,grammer,parsedInputString[i]);
#ifdef SHOW_SCREEN
#endif
#ifdef DEBUG
        cky.print(fp);
#endif
        printParseTree(fp,cky,parsedInputString[i]);
#ifdef SHOW_SCREEN
        printParseTree(stdout,cky,parsedInputString[i]);
#endif
    }
    fclose(fp);
    return 0;
}
