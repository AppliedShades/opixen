#pragma once
#include <vector>
#include <algorithm>
#include <string>
using strtype = const std::string&;
struct pos2_ulong{
    size_t start,end;
};
pos2_ulong findfirstof(std::string needle, std::string haystack){
    size_t start = haystack.find(needle);
    if(start == std::string::npos){
        return {0,0};
    }
    return {start, start + needle.size()};
}
std::vector<pos2_ulong> findAll(strtype haystack, strtype needle) {
    std::vector<pos2_ulong> positions;
    size_t pos = 0;
    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        positions.push_back({pos, pos + needle.length()});
        pos += needle.length();
    }
    return positions;
}
std::vector<pos2_ulong> findAllBeginEnd(strtype haystack, strtype begin, strtype end) {
    std::vector<pos2_ulong> positions;
    size_t pos = 0;
    while ((pos = haystack.find(begin, pos)) != std::string::npos) {
        size_t endPos = haystack.find(end, pos + begin.length());
        if (endPos != std::string::npos) {
            positions.push_back({pos, endPos + end.length()});
            pos = endPos + end.length(); // update pos to continue searching after the end
        } else {
            // if end is not found, break the loop to avoid infinite loop
            break;
        }
    }
    return positions;
}
std::vector<std::string> splitAllAnyMatch(strtype haystack, std::vector<std::string> needles) {
    std::vector<std::string> result;
    size_t prev = 0;
    std::vector<pos2_ulong> allPositions;
    std::sort(needles.begin(), needles.end(), [](const std::string& a, const std::string& b) {
        return a.length() > b.length();
    });
    for (const auto& needle : needles) {
        auto positions = findAll(haystack, needle);
        for (auto& pos : positions) {
            bool found = false;
            for (const auto& already : allPositions) {
                if (already.start <= pos.start && already.end >= pos.end) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                allPositions.push_back(pos);
            }
        }
    }
    std::sort(allPositions.begin(), allPositions.end(), [](const pos2_ulong& a, const pos2_ulong& b) {
        return a.start < b.start;
    });
    for (const auto& pos : allPositions) {
        if (pos.start > prev) {
            result.push_back(haystack.substr(prev, pos.start - prev));
        }
        result.push_back(haystack.substr(pos.start, pos.end - pos.start));
        prev = pos.end;
    }
    if (prev < haystack.size()) {
        result.push_back(haystack.substr(prev));
    }
    return result;
}
std::vector<std::string> splitAllBeginEndAnyMatch(strtype haystack, std::vector<std::pair<std::string, std::string>> beginendlist) {
    std::vector<std::string> splitterlist;
    for(auto i:beginendlist){splitterlist.push_back(i.first);splitterlist.push_back(i.second);}
    std::vector<std::string> splittedAll = splitAllAnyMatch(haystack, splitterlist);
    std::vector<std::string> result;
    // if a splitter begin matches current splittedall[i], then search for the splitter end exact match in splittedAll
    result.push_back("");
    for (size_t i=0; i< splittedAll.size(); i++) {
        bool matchedS = false, matchedE = false;
        for(size_t splitter=0; splitter<beginendlist.size(); splitter++){
            if(splittedAll[i]==beginendlist[splitter].first){
                //result.push_back("matchesS");
                std::string joiner = splittedAll[i];
                for(size_t j=i+1; j<splittedAll.size(); j++){
                    joiner+=splittedAll[j];
                    if(splittedAll[j]==beginendlist[splitter].second){
                        matchedE=true;
                        i=j;
                        break;
                    }
                }
                result.push_back(joiner);
                result.push_back("");
                if(!matchedE){i=splittedAll.size();}
                matchedS=true;
                break;
            }    
        }
        if(!matchedS){
            result[result.size()-1] += splittedAll[i];
        }
    }
    result.erase(std::remove_if(result.begin(), result.end(),
        [](const std::string& s) { return s.empty(); }), result.end());
    return result;
}
std::vector<std::string> getAllBeginEnd_substr(strtype haystack, strtype begin, strtype end) {
    auto positions = findAllBeginEnd(haystack, begin, end);
    std::vector<std::string> result;
    for(const auto& pos : positions){result.push_back(haystack.substr(pos.start, pos.end - pos.start));}
    return result;
}
std::string joinAll(const std::vector<std::string>& pieces, strtype glue){
    std::string result;
    for (const auto& piece : pieces) {
        result += piece + glue;
    }
    result.pop_back();
    return result;
}
std::string replaceAll(strtype haystack, strtype needle, strtype replacement) {
    std::string result = haystack;
    size_t pos = 0;
    while ((pos = result.find(needle, pos)) != std::string::npos) {
        result.replace(pos, needle.length(), replacement);
        pos += replacement.length();
    }
    return result;
}
std::string replaceAllBeginEnd(strtype haystack, strtype begin, strtype end, strtype replacebegin, strtype replaceend) {
    auto found = findAllBeginEnd(haystack, begin, end);
    std::string result = "";
    size_t prev = 0;
    for (const auto& pos : found) {
        result += haystack.substr(prev, pos.start - prev);
        result += replacebegin;
        result += haystack.substr(pos.start + begin.size(), pos.end - pos.start - begin.size() - end.size());
        result += replaceend;
        prev = pos.end;
    }
    result += haystack.substr(prev);

    return result;
}
bool isin(std::string needle, std::string haystack){
    return haystack.find(needle) != std::string::npos;
}
bool isnotin(std::string needle, std::string haystack){
    return haystack.find(needle) >= std::string::npos;
}
bool startswith(std::string needle, std::string haystack){
    return haystack.find(needle) == 0;
}
bool endswith(std::string needle, std::string haystack){
    return haystack.find(needle) == haystack.size() - needle.size();
}
std::string getsubstr(std::string haystack, size_t start, size_t end=-1){
    //if start or end are negative, they are relative to the end of the string
    size_t parsedstart =start % haystack.size();
    if(start<0){parsedstart=haystack.size()+start;}
    size_t parsedend =end % haystack.size();
    if(end<0){parsedend=haystack.size()+end;}
    return haystack.substr(parsedstart, parsedend - parsedstart);
}
std::string tolower(std::string haystack){
    std::transform(haystack.begin(), haystack.end(), haystack.begin(), [](char c) { return std::tolower(c); });
    return haystack;
}
std::string toupper(std::string haystack){
    std::transform(haystack.begin(), haystack.end(), haystack.begin(), [](char c) { return std::toupper(c); });
    return haystack;
}
unsigned int touint(std::string value){return std::stoul(value);}
int toint(std::string value){return std::stoi(value);}
float tofloat(std::string value){return std::stof(value);}
double todouble(std::string value){return std::stod(value);}
std::string tostr(int value){return std::to_string(value);}
std::string tostr(unsigned int value){return std::to_string(value);}
std::string tostr(float value){return std::to_string(value);}
std::string tostr(double value){return std::to_string(value);}
std::string tostr(glm::vec2 value, std::string delim = ",", std::string pref="", std::string post=""){return pref + std::to_string(value.x) + delim + std::to_string(value.y) + post;}