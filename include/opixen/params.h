////// :: These function are About String parameter separations
#pragma once
#include <iostream>
#include <opixen/strings.h>

std::string getparam(std::string metadata, std::string param){
    auto v = findAllBeginEnd(metadata, param+"=", " ");
    for(auto it = v.rbegin(); it != v.rend(); ++it){
        if(it->start>0 && metadata[it->start-1]!=' '){
            v.erase(std::next(it).base());
        }
    }    
    if(v.size()>0){
        return getsubstr(metadata,v.back().start+param.size()+1,v.back().end-1);
    }else{ return ""; }//// :: Returns Param Value
}
std::string removeparam(std::string metadata, std::string param){
    auto v = findAllBeginEnd(metadata, param+"=", " ");
    if(v.size()>0){
        std::string result = metadata;
        for(auto it = v.rbegin(); it != v.rend(); ++it){
            if(it->start>0 && result[it->start-1]!=' '){continue;}
            result.erase(it->start, it->end - it->start);
        }
        return result;
    }else{ return metadata; }
}
void setparam(std::string& metadata, std::string param, std::string value){
    metadata = removeparam(metadata, param) + " " + param + "=" + value + " ";
}
