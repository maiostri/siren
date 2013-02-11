#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <artemis/image/jpg/JpgLib.hpp>
#include "MetricTreeManager.h"
#include "DataDictionaryAccess.h"
#include "Parser.h"

using namespace std;
using namespace Connections;

int main() {
    
    // Necessarios para a instancia de DataDictionary
        vector<string> commandlist;
        const string username = "user1";
        const string password = "caxias";
        
//        string input = "create metric texture for stillimage (TEXTUREEXT (Texture AS T))";
        
        
        MetricTreeManager *mtm = new MetricTreeManager();
//        Parser *p = new Parser(input,username,password,mtm);
 //       DataDictionaryAccess *da = new DataDictionaryAccess(username,password,mtm,p);
        
        eConnections eConnectionsEnum = ORACLE;

        
        ifstream myReadFile;
        myReadFile.open("script.txt");
        string command;
        while (!myReadFile.eof()) {
            getline(myReadFile,command);
            cout << "Comando a ser executado: " + command << endl;
            Parser *parser = new Parser(eConnectionsEnum, command,username,password,mtm);
            parser->Parse();
            delete parser;            
        }
        myReadFile.close();
        delete mtm;
          
    
}
