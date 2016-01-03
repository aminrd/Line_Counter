/*
 * File name:
 * line_cnt.cpp
 * -------------------------------------------------
 * Description:
 *
 * A program to count .C, .CPP, .H code lines
 * including White Lines, Comment Lines, Code lines
 * it is designed for C/C++ and any programming language 
 * which have similar comment rules.
 *
 * I appreciate your help to improve this code and 
 * develop it for other programming languages
 * -------------------------------------------------
 * Line Counter ( version 1.0 ) (Available for Linux and Mac OS)
 * by :
 *		Amin Aghaee
 *      http://ce.sharif.edu/~aghaee/
 * Sharif University of Technology
 * Computer engineering department
 * -------------------------------------------------
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>


using namespace std;
// -------------------------------------------------------------------

#ifndef FOR
#define FOR(i,x) for(int i=0;i<(x);i++)
#endif

char _CMT = 'c';
char _WHT = ' ';
char _NRM = 'n';

char accepted_list[] = ".cpp  .c  .h  .java"; // separate by space
vector<string> acc_list;
// -------------------------------------------------------------------

void scan_accpeted(){
    stringstream ss;
    ss<<accepted_list;
    string frm;
    
    while( ss>>frm )
        acc_list.push_back( frm );
    return;
}

string get_format( string fn ){
    int idx = -1;
    FOR(i, fn.size())
        if( fn[i] == '.' )
            idx = i;
    if( idx == -1 )
        return "";
    return fn.substr(idx, fn.size() - idx);
}

bool check(string filename){
    filename = get_format(filename);
    FOR(i, acc_list.size())
        if( acc_list[i] == filename )
            return true;
    return false;
}


struct LINE_CNT{
    int normal, comment, white;
    LINE_CNT(){
        normal = 0;
        comment = 0;
        white = 0;
    }
    void copy( LINE_CNT* from ){
        this->normal = from->normal;
        this->comment = from->comment;
        this->white = from->white;
        return;
    }
    void add( LINE_CNT* from ){
        this->normal += from->normal;
        this->comment += from->comment;
        this->white += from->white;
        return;
    }
    int total(){
        return normal + white + comment;
    }
    void print(){
        cout<<"\n-Summary: \n";
        cout<<"  -Comment:\t\t"<<comment<<endl;
        cout<<"  -White:\t\t"<<white<<endl;
        cout<<"  -Code:\t\t"<<normal<<"  +"<<endl;
        cout<<"----------------------------------\n";
        cout<<"  -Total lines:\t\t"<<this->total()<<endl<<endl;
        return;
    }
};

void lines_printer( vector<string> );
void lines_printer( vector<string> lines){
    FOR(i,lines.size())
        cout<<i+1<<": "<<lines[i]<<endl;
    return;
}

// MY line functions:
vector<string> CPP_transform( vector<string>);// Works out for .cpp, .c and .java files
vector<string> CPP_transform( vector<string> lines ){ // Transfrom all comments in CPP to 8'b11111111 (_CMT)
    
    int state = 0;
    string l;
    FOR(k, lines.size()){
        l = lines[k];
        FOR(i, l.size()){
            if( isspace( l[i] ) )
                l[i] = _WHT;
            else if( l[i] != '*' && l[i] != '/' )
                l[i] = _NRM;
            
        switch (state) {
            case 0:
                if( l[i] == '/' )           state = 1;
                else if( l[i] != _WHT )     l[i] = _NRM;
                continue;
            case 1:
                if( l[i] == '/' )           {state = 2; l[i] = l[i-1] = _CMT;}
                else if( l[i] == '*' )      {state = 3; l[i] = l[i-1] = _CMT;}
                else                        {state = 0; l[i-1] = _NRM;}
                continue;
            case 2:
                l[i] = _CMT;
                continue;
            case 3:
                if( l[i] == '*' )           state = 4;
                else                        l[i] = _CMT;
                continue;
            case 4:
                if( l[i] == '/' )           {state = 0; l[i] = l[i-1] = _CMT;}
                else if( l[i] == '*' )      {l[i-1] = _CMT;}
                else                        {state = 3; l[i] = l[i-1] = _CMT;}
                continue;
            default:
                continue;
        }
        }
        lines[k] = l;
        if( state == 2 )
            state = 0;
    }
    return lines;
}


bool does_contain( string, char );
bool does_contain( string line, char c){
    FOR( i, line.size() )
        if( line[i] == c )
            return true;
    return false;
}

void single_file_counter( string, LINE_CNT* );
void single_file_counter( string filename, LINE_CNT* output){
    string line;
    LINE_CNT tmp_cnt;
    ifstream fin(filename);
    if( !fin ){
        output->copy( &tmp_cnt );
        return;
    }
    
    vector<string> lines;
    while( getline(fin, line) )
        lines.push_back( line );
    fin.close();
    
    lines = CPP_transform (lines);
    FOR(i, lines.size())
        if( does_contain(lines[i], 'n') )
            tmp_cnt.normal++;
        else if( does_contain(lines[i], 'c') )
            tmp_cnt.comment++;
        else
            tmp_cnt.white++;
    
    output->copy( &tmp_cnt );
    return;
}

static void recursive_run (const char * dir_name, LINE_CNT* output){
    char my_file[PATH_MAX];
    DIR * d;
    
    d = opendir (dir_name);
    if (! d) {
        fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent * entry;
        const char * d_name;
        
        entry = readdir (d);
        if (! entry)    break;
        d_name = entry->d_name;
        sprintf (my_file, "%s/%s", dir_name, d_name);
        
        if( check( my_file )){
            LINE_CNT tmp;
            printf("  %s\n", my_file);
            single_file_counter(my_file, &tmp);
            output->add( &tmp );
        }
        
        
        if (entry->d_type & DT_DIR) {
            if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0) {
                int path_length;
                char path[PATH_MAX];
                
                path_length = snprintf (path, PATH_MAX, "%s/%s", dir_name, d_name);
                if (path_length >= PATH_MAX) {
                    fprintf (stderr, "Path length has got too long.\n");
                    exit (EXIT_FAILURE);
                }
                recursive_run (path, output);
            }
        }
    }
    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n", dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
}


void track_with_file(string filename, LINE_CNT* output){
    ifstream fin(filename.c_str());
    if( !fin ){
        cerr<<" No such file or directory!!\n";
        return;
    }
    
    cout<<"Tracked files: \n";
    LINE_CNT tmp;
    string p2f;
    while( fin>>p2f ){
        cout<<" "<<p2f<<endl;
        single_file_counter( p2f, &tmp );
        output->add(&tmp);
    }
    fin.close();
    return;
}

// -------------------------------------------------------------------------------------------------------------
LINE_CNT result, tmp;
int main(){
    scan_accpeted();
    
    
    
    int type;
    string fn;
    
    cout<<"\n Please Enter running mode:\n";
    cout<<"\t 0:  Manual\n";
    cout<<"\t 1:  Automatic(Run all ACCEPTED files which exist here/ in sub-folders)\n";
    cout<<"\t 2:  File(List all paths in a file)\n";
    cin>>type;
    if( type == 0 ){
        cout<<" Number of files: \n";
        cin>>type;
        cout<<" Please enter path to files one by one. (e.g ./line_cnt.cpp)\n";
        FOR(i, type){
            cin>>fn;
            single_file_counter(fn, &tmp);
            result.add( &tmp );
        }
    }else if( type == 1 ){
        cout<<" Please Enter root path: (e.g. . , .. , ./FOLDER )\n";
        cin>>fn;
        cout<<" List of Tracked files: \n";
        recursive_run(fn.c_str(), &result);
    }else if( type == 2 ){
        cout<<" Please Enter File name/path: (e.g. ./fld/filename.txt )\n";
        cin>>fn;
        track_with_file(fn, &result);
    }
    result.print();
}

