#include <string>
#include <iostream>
#include <fstream>

#include "Compilation_engine.h"

using namespace std;

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        string in_filename(argv[i]);
        string file_no_extension = in_filename.substr(0, in_filename.find_last_of('.'));
        Compilation_engine engine{in_filename,file_no_extension+"_analysis.xml"};
        try{
            engine.run();
            cout<<in_filename<<" compiled successfully\n";
        } catch(invalid_argument e){
            cout<<in_filename<<" compilation failed\n";
            cerr<<e.what()<<endl;
        }

    }
}