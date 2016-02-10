#ifndef TOOLKIT_FOUNDATION_STRING_UTILS_H
#define TOOLKIT_FOUNDATION_STRING_UTILS_H

#include <string>
#include <sstream>
#include <cctype>

using namespace std;

namespace Foundation {
    class StringUtils {
    public:
        static string StringToUpper(string strToConvert);
        static string StringToLower(string strToConvert);
        static void RightTrim(string &source, string t);
        static void LeftTrim(string &source, string t);
        static string RightPad(string source, int strLength, string padChar = " ");
        static string LeftPad(string source, int strLength, string padChar = " ");
    };

    string StringUtils::StringToUpper(string strToConvert) {
       for(unsigned int i=0;i<strToConvert.length();i++)   {
          strToConvert[i] = (char)toupper(strToConvert[i]);
       }
       return strToConvert;
    }

    string StringUtils::StringToLower(string strToConvert)
    {
       for(unsigned int i=0;i<strToConvert.length();i++)  {
          strToConvert[i] = (char)tolower(strToConvert[i]);
       }
       return strToConvert;
    }

    void StringUtils::RightTrim(string &source, string t) {
        source.erase(source.find_last_not_of(t)+1);
    }

    void StringUtils::LeftTrim(string &source, string t) {
        source.erase(0, source.find_first_not_of(t));
    }

    string StringUtils::RightPad(string source, int strLength, string padChar) {
        string temp = "";
        int i;
        for(i = 0; i < min((int)source.size(), strLength); i++) {
            temp.push_back(source[i]);
        }
        for(int j = i; j < strLength; j++) {
            temp = temp + padChar;
        }
        return temp;
    }

    string StringUtils::LeftPad(string source, int strLength, string padChar) {
        string temp = "";
        int i;
        for(int j = 0; j < strLength - (int)source.size(); j++) {
            temp = padChar + temp;
        }

        for(i = 0; i < min((int)source.size(), strLength); i++) {
            temp.push_back(source[i]);
        }
        return temp;
    }
}

#endif
