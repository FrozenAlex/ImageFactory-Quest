#include "Utils/StringUtils.hpp" 


#include <vector>
#include <sstream>
namespace StringUtils {

    std::string removeTrailingZeros(int i) {
        std::string s = std::to_string(i);

        /*
            for some reason it just doesn't add zeros in this? idk man
            ¯\_(ツ)_/¯
        */

        return s;
    }

    // Il2cppString Split no workey lmao
    std::vector<std::string> split(std::string s, char c) {
        std::vector<std::string> tokens;
        std::string token;
        std::stringstream ss(s);
        while (getline(ss, token, c)){
            tokens.push_back(token);
        }
    
        return tokens;
    }

}