#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

using namespace std;

struct TimeSlot {
    enum class Weekday { SAT, SUN, MON, TUE,WED, THU, FRI };
    enum class Time {
        T071, T072, T081, T082, T091, T092, T101, T102, T111, T112, T121, T122, T131, T132,
        T141, T142, T151, T152, T161, T162, T171, T172, T181, T182, T191, T192, T201, T202
    };
    Weekday day;
    Time start_time, end_time;
};

typedef unsigned int CourseCode;
typedef unsigned short GroupCode;

struct Offering {
    CourseCode course_code;
    GroupCode group_code;
    vector<TimeSlot> time_slots;
};

map<string, TimeSlot::Weekday> get_weekday_dict() {
    static map<string, TimeSlot::Weekday> weekday_dict;
    if (!weekday_dict.size()) {
        weekday_dict["SAT"] = TimeSlot::Weekday::SAT;
        weekday_dict["SUN"] = TimeSlot::Weekday::SUN;
        weekday_dict["MON"] = TimeSlot::Weekday::MON;
        weekday_dict["TUE"] = TimeSlot::Weekday::TUE;
        weekday_dict["WED"] = TimeSlot::Weekday::WED;
        weekday_dict["THU"] = TimeSlot::Weekday::THU;
        weekday_dict["FRI"] = TimeSlot::Weekday::FRI;
    }
    return weekday_dict;
}

map<string, TimeSlot::Time> get_time_dict() {
    static map<string, TimeSlot::Time> time_dict;
    if (!time_dict.size()) {
        time_dict["07:00"] = TimeSlot::Time::T071;
        time_dict["07:30"] = TimeSlot::Time::T072;
        time_dict["08:00"] = TimeSlot::Time::T081;
        time_dict["08:30"] = TimeSlot::Time::T082;
        time_dict["09:00"] = TimeSlot::Time::T091;
        time_dict["09:30"] = TimeSlot::Time::T092;
        time_dict["10:00"] = TimeSlot::Time::T101;
        time_dict["10:30"] = TimeSlot::Time::T102;
        time_dict["11:00"] = TimeSlot::Time::T111;
        time_dict["11:30"] = TimeSlot::Time::T112;
        time_dict["12:00"] = TimeSlot::Time::T121;
        time_dict["12:30"] = TimeSlot::Time::T122;
        time_dict["13:00"] = TimeSlot::Time::T131;
        time_dict["13:30"] = TimeSlot::Time::T132;
        time_dict["14:00"] = TimeSlot::Time::T141;
        time_dict["14:30"] = TimeSlot::Time::T142;
        time_dict["15:00"] = TimeSlot::Time::T151;
        time_dict["15:30"] = TimeSlot::Time::T152;
        time_dict["16:00"] = TimeSlot::Time::T161;
        time_dict["16:30"] = TimeSlot::Time::T162;
        time_dict["17:00"] = TimeSlot::Time::T171;
        time_dict["17:30"] = TimeSlot::Time::T172;
        time_dict["18:00"] = TimeSlot::Time::T181;
        time_dict["18:30"] = TimeSlot::Time::T182;
        time_dict["19:00"] = TimeSlot::Time::T191;
        time_dict["19:30"] = TimeSlot::Time::T192;
    }
    return time_dict;
}

CourseCode to_course_code(const string str) {
    return atoi(str.c_str());
}

GroupCode to_group_code(const string str) {
    return atoi(str.c_str());
}

vector<string> tokenize(const string str) {
    stringstream sstr(str);
    vector<string> tokens;
    string token;
    while(sstr >> token)
        tokens.push_back(token);
    return tokens;
}

vector<string> tokenize(const string str, const char delim) {
    stringstream sstr(str);
    vector<string> tokens;
    string token;
    while(getline(sstr, token, delim))
        tokens.push_back(token);
    return tokens;
}

map<CourseCode, string> get_course_names(string dict_addr) {
    map<CourseCode, string> course_names;

    fstream dict_stream(dict_addr);
    if (dict_stream.fail()) {
        cerr << "Invlaid <CourseNameDict.csv> address: " << dict_addr << endl;
        exit(EXIT_FAILURE);
    }
    {
        string dummy_str;
        dict_stream >> dummy_str >> dummy_str;
    }
    while (!dict_stream.eof() && !dict_stream.fail()) {
        CourseCode code;
        string name;
        char dummy_char;
        dict_stream >> code >> dummy_char >> name;
        course_names[code] = name;
    }
    if (dict_stream.fail()) {
        cerr << "Invlaid <CourseNameDict.csv> structure: " << dict_addr << endl;
        exit(EXIT_FAILURE);
    }
    dict_stream.close();

    return course_names;
}

TimeSlot parse_time_slot(string weekday, string full_time) {
    const char time_delim = '-';
    static const map<string, TimeSlot::Weekday> weekday_dict = get_weekday_dict();
    static const map<string, TimeSlot::Time> time_dict = get_time_dict();

    TimeSlot time_slot;
    vector<string> times = tokenize(full_time, time_delim);
    if (times.size() != 2)
        throw invalid_argument("Invalid number of times per time slot");
    try {
        time_slot.day = weekday_dict.at(weekday);
        time_slot.start_time = time_dict.at(times[0]);
        time_slot.end_time = time_dict.at(times[1]);
    } catch(out_of_range) {
        throw invalid_argument("Invalid time slot arg");
    }
    return time_slot;
}

Offering parse_offering(string str) {
    const char code_delim = '-';

    Offering offering;
    vector<string> tokens = tokenize(str);
    if (!tokens.size() || !(tokens.size() % 2))
        throw invalid_argument("Invalid number of offering args");
    vector<string> codes = tokenize(tokens[0], code_delim);
    if (codes.size() != 2)
        throw invalid_argument("Invalid offering code");
    offering.course_code = to_course_code(codes[0]);
    offering.group_code = to_group_code(codes[1]);
    for (size_t i = 1; i < tokens.size() - 1; i += 2)
        offering.time_slots.push_back(parse_time_slot(tokens[i], tokens[i + 1]));
    return offering;
}

vector<Offering> get_offerings() {
    vector<Offering> offerings;
    string offering;
    while (getline(cin, offering)) {
        try {
            offerings.push_back(parse_offering(offering));
        } catch(const invalid_argument& exc) {
            cerr << exc.what() << ". Try again..." << endl;
        }
    }
    return offerings;
}

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <CourseNameDict.csv>" << endl;
        return EXIT_FAILURE;
    }

    static const map<CourseCode, string> course_names = get_course_names(argv[1]);
    vector<Offering> offerings = get_offerings();
    
    return EXIT_SUCCESS;
}
