#ifndef OPIXEN_ERROR_H
#define OPIXEN_ERROR_H
#include <vector>
#include <string>
#include <sstream>
#include <iostream>


namespace OPIXEN{
    class ERRORS{
        public:
            enum DANGERLEVEL{NORMAL=0,WARNING=1,FATAL=2};
            void report(int code, std::vector<std::string> text, DANGERLEVEL hazard);
            void printErrors();
            DANGERLEVEL ErrorState(){return dangerlevel;}
            bool hasError(){return error_list.size()>0? true:false;}
        private:
            std::vector<std::pair<int,std::string>> error_list;
            std::stringstream ss;
            DANGERLEVEL dangerlevel=NORMAL;
            

    } static errors;
    
    void ERRORS::report(int code, std::vector<std::string> text, DANGERLEVEL hazard){
        for(int i=0;i<text.size();i++){
            ss << text[i];
        }
        error_list.push_back({code,ss.str()});
        if(hazard>dangerlevel){dangerlevel=hazard;}
    }

    void ERRORS::printErrors(){
        for(int i=0;i<error_list.size();i++){
            std::cout << "error " << error_list[i].first << ": " << error_list[i].second << std::endl;
        }
    }
};

#endif