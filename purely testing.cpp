#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include<string.h>
#include<fstream>
#include<dirent.h>


using namespace std;
void listFile();

int main(){
    DIR *p;
    cout << p << " " << p << " " << p->dd_handle << " " << p->dd_name << " " << p->dd_stat << endl;
    p = opendir("./Dataset-1");
    cout << p << " " << p << " " << p->dd_handle << " " << p->dd_name << " " << p->dd_stat << endl;
    return 0;
}

void listFile(){
    DIR *pDIR;
    int county = 0;
    struct dirent *entry;
    if( pDIR=opendir("./Dataset-1") ){
        while(entry = readdir(pDIR)){
            if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
            cout << entry->d_name << "\n";
            county++;
            }
        }
        closedir(pDIR);
    }
    cout << endl << county << endl;
}
