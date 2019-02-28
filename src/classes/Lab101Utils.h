 //
//  Lab101Utils.h
//  CollationShrink
//
//  Created by lab101 on 27/03/14.
//
//

#ifndef CollationShrink_Lab101Utils_h
#define CollationShrink_Lab101Utils_h

#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <regex>
#include "cinder/Filesystem.h"
#include "cinder/Vector.h"
#include "cinder/CinderMath.h"

#include "cinder/Utilities.h"


inline std::string getStringWithLeadingZero(int number,int minWidth){
   
    std::string txt = ci::toString(number);
    return std::string(minWidth - txt.length(), '0') + txt;

    //return "3";
}

inline int getUnixTime(){
    std::time_t t = std::time(0);  // t is an integer type
    return (int) t;
}


inline std::string getDateString(){
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S");
	return ss.str();
}

inline std::string getDayString(){
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
	return ss.str();
}

inline std::string getFormatedString(float value, int decimals){
	std::stringstream stream;
	stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
	stream.precision(decimals);
	stream << value;
	return stream.str();

}


// file helpers
inline bool isHidden(const ci::fs::path &p)
{
    std::string name = p.filename().string(); //p.filename();
    if (name[0] == '.')
    {
        return true;
    }
    
    return false;
}

inline bool exists(const std::string& path)
{
    return ci::fs::exists(path);
}

// used mostly for cleaning filenames of garbage
inline std::string getCleanText(std::string input){
    std::regex reg("([a-zA-Z_0-9-]*)");
    std::smatch match;
    std::string output = "";
    
    std::sregex_iterator next(input.begin(), input.end(), reg);
    std::sregex_iterator end;
    while (next != end) {
        std::smatch match = *next;
        output += match.str();
        next++;
    }
    
    return output;
}

inline void strReplace(std::string& source, const std::string& find,const std::string& replace){
	size_t start_pos = 0;
	while ((start_pos = source.find(find, start_pos)) != std::string::npos) {
		//source.replace(start_pos, find.length(), replace);
		//start_pos += replace.length(); // Handles case where 'to' is a substring of 'from'
	}
}


inline std::string strToUpper(std::string& input){
    std::transform(input.begin(), input.end(),input.begin(), ::toupper);
    return input;
}


//check for division by zero???
//--------------------------------------------------
inline float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {

	if (fabs(inputMin - inputMax) < FLT_EPSILON){
		//		ofLogWarning("ofMath") << "ofMap(): avoiding possible divide by zero, check inputMin and inputMax: " << inputMin << " " << inputMax;
		return outputMin;
	}
	else {
		float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

		if (clamp){
			if (outputMax < outputMin){
				if (outVal < outputMax)outVal = outputMax;
				else if (outVal > outputMin)outVal = outputMin;
			}
			else{
				if (outVal > outputMax)outVal = outputMax;
				else if (outVal < outputMin)outVal = outputMin;
			}
		}
		return outVal;
	}

}

inline int HexadecimalToDecimal(std::string hex) { //https://www.programmingalgorithms.com/algorithm/hexadecimal-to-rgb?lang=C%2B%2B
    int hexLength = hex.length();
    double dec = 0;
    std::string hexChars[6] =  {"A","B","C","D","E","F"};
    
    for (int i = 0; i < hexLength; ++i)
    {
        
        float b ;
        for(int j = 0;j<6;j++){
            if(std::string(1,hex[i]) == hexChars[j]){
                b = j+10;
            }else{
                b = hex[i] - 0.0f; // to int
            }
        };
        
        if (b >= 48 && b <= 57)
            b -= 48;
        else if (b >= 65 && b <= 70)
            b -= 55;
        dec += b * std::pow(16, ((hexLength - i) - 1));
    }
    return (int)dec;
}

inline ci::Color hexStringToColor(std::string hex){//https://www.programmingalgorithms.com/algorithm/hexadecimal-to-rgb?lang=C%2B%2B
    if (hex[0] == '#')
        hex = hex.erase(0, 1);
    
    unsigned char r = (unsigned char)HexadecimalToDecimal(hex.substr(0, 2));
    unsigned char g = (unsigned char)HexadecimalToDecimal(hex.substr(2, 2));
    unsigned char b = (unsigned char)HexadecimalToDecimal(hex.substr(4, 2));
    float R = r/255.0f;
    float G = g/255.0f;
    float B = b/255.0f;
    return ci::Color(R, G, B);
}


#endif
