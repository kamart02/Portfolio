#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <unordered_set>
#include <cctype>
#include <queue>
#include <unordered_map>


enum InputType {
    ERROR, 
    NEW,
    TOP,
    DATA,
    EMPTY,
};

namespace def {
    constexpr uint32_t topSize = 7;

    using pair = std::pair<uint32_t, uint64_t>;
    using unord_map = std::unordered_map<uint32_t, uint64_t>;
    using unord_set = std::unordered_set<uint32_t>;
}

// Collection of used regexes
namespace reg {
    const std::regex topRegex("^\\s*TOP\\s*$");
    const std::regex newRegex("^\\s*NEW\\s+0*[1-9]\\d{0,7}\\s*$");
    const std::regex dataRegex("^\\s*(0*[1-9]\\d{0,7}\\s+)*(0*[1-9]\\d{0,7})\\s*$");
    const std::regex emptyRegex("^\\s*$");
}

namespace cmp {
    struct lessGreaterQueue {
        bool operator()(const def::pair& p1, const def::pair& p2) const {
            if (p1.second > p2.second) {
                return true;
            }
            else if (p1.second < p2.second) {
                return false;
            }
            else {
                if (p1.first < p2.first) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
    };
}

// Converts first found number in string to int
uint32_t strToSingleNumber(const std::string& str) {
    std::string::size_type lastPosition = 0;

    for (lastPosition = 0; lastPosition < str.size(); lastPosition++) {
        if (std::isdigit(str[lastPosition])) {
            break;
        }
    }
    
    return std::stoi(str.substr(lastPosition));
}

// Converts a string of numbers to a vector
std::vector<uint32_t> strToVector(std::string& str) {
    std::vector<uint32_t> values;
    std::string::size_type lastPosition = 0;
    uint32_t value = 0;
    std::string localStr = str.substr(0);
    

    while (lastPosition < localStr.size()) {
        try {
            localStr = localStr.substr(lastPosition);
            value = std::stoi(localStr, &lastPosition);
            lastPosition++;
            values.push_back(value);
        }
        catch (const std::invalid_argument& ex) {
            break;
        }
    }

    return values;
}

// Simple validation of input (checks for commands and numbers)
InputType simpleValidateAndMatch(const std::string& line) {
    if (std::regex_match(line, reg::topRegex)) {
        return TOP;
    }
    else if (std::regex_match(line, reg::newRegex)) {
        return NEW;
    }
    else if (std::regex_match(line, reg::dataRegex)) {
        return DATA;
    }
    else if (std::regex_match(line, reg::emptyRegex)) {
        return EMPTY;
    }
    else {
        return ERROR;
    }
}

// Returns true if duplicate is found in vector
bool checkForDuplicates(std::vector<uint32_t>& vec) {
    def::unord_set set;

    for (uint32_t element: vec) {
        if (set.count(element) > 0) {
            return true;
        }
        else {
            set.insert(element);
        }
    }

    return false;
}

// Returns true if dropped track found
bool checkForDropped(std::vector<uint32_t>& vec, def::unord_set& dropped) {
    for (uint32_t element: vec) {
        if (dropped.count(element) > 0) {
            return true;
        }
    }

    return false;
}

bool input(std::string& str, size_t& lineNumber) {
    bool endOfFile;
    std::getline(std::cin, str);
    endOfFile = std::cin.eof();
    if (!endOfFile) {
        lineNumber++;
    }

    return !endOfFile;
}

void printError(std::string& str, size_t lineNumber) {
    std::cerr << "Error in line " << lineNumber <<": " << str << "\n";
}

void addValueToMap(uint32_t key, uint64_t value, def::unord_map& map) {
    auto it = map.find(key);

    if (it == map.end()) {
        map.insert({key, value});
    }
    else {
        it->second += value;
    }
}

void vectorAddToMap(std::vector<uint32_t>& vec, def::unord_map& map) {
    for (uint32_t track: vec) {
        addValueToMap(track, 1, map);
    }
}

uint32_t vectorMax(const std::vector<uint32_t>& vec) {
    uint32_t value = 0;

    for (auto element: vec) {
        if (element > value) {
            value = element;
        }
    }

    return value;
}


std::vector<uint32_t> findTopValues(const def::unord_map& map) {
    std::priority_queue<def::pair, std::vector<def::pair>, cmp::lessGreaterQueue> queue;
    
    for (auto it : map) {
        if (queue.size() < def::topSize) {
            queue.push(it);
        }
        else {
            static constexpr auto compare = cmp::lessGreaterQueue();
            if (!compare(queue.top(), it)) {
                queue.pop();
                queue.push(it);
            }
        }
    }

    std::vector<uint32_t> values(queue.size());
    for (auto i = values.size(); i-- > 0;) {
        values[i] = queue.top().first;
        queue.pop();
    }

    return values;
}

bool vectorContains(std::vector<uint32_t>& vec, uint32_t value) {
    for(auto element : vec) {
        if(element == value) return true;
    }
    return false;
}

int positionChange(std::vector<uint32_t>& prev, std::vector<uint32_t>& current, u_int32_t value) {
    int pos1 = 0, pos2 = 0;
    for(std::vector<uint32_t>::size_type i = 0; i < prev.size(); i++) {
        if(prev[i] == value) pos1 = i;

    }
    for(std::vector<uint32_t>::size_type i = 0; i < current.size(); i++) {
        if(current[i] == value) pos2 = i;
    }
    return pos1 - pos2;
}

void updateDroppedTracks(def::unord_set& dropped, std::vector<uint32_t>& prevNot, std::vector<uint32_t>& currentNot) {
    for(std::vector<uint32_t>::size_type i = 0; i < prevNot.size(); i++) {
        bool isDropped = true;
        for(std::vector<uint32_t>::size_type j = 0; j < currentNot.size(); j++) {
            if(prevNot[i] == currentNot[j]) isDropped = false;
        }
        if(isDropped) {
            dropped.insert(prevNot[i]);
        }
    }
}

void updateTop(def::unord_map& currentNot, def::unord_map& allTimeTop) {
    std::vector<uint32_t> notif;
    notif = findTopValues(currentNot);
    for(std::vector<uint32_t>::size_type i = 0; i < notif.size(); i++) {
        addValueToMap(notif[i], def::topSize - i, allTimeTop);
    }

}
void printNot(def::unord_map& currentNot, def::unord_set& dropped, std::vector<uint32_t>& prevNot) {
    std::vector<uint32_t> notif;

    notif = findTopValues(currentNot);
    for(std::vector<uint32_t>::size_type i = 0; i < notif.size(); i++) {
        std::cout << notif[i] << " ";
        if(!vectorContains(prevNot, notif[i])) std::cout << "-\n";
        else std::cout << positionChange(prevNot, notif, notif[i]) << "\n";
    }
    updateDroppedTracks(dropped, prevNot, notif);
    prevNot = notif;
}

void topNot(def::unord_map& allTimeTop, std::vector<uint32_t>& prevTop) {
    std::vector<uint32_t> top = findTopValues(allTimeTop);
    for(std::vector<uint32_t>::size_type i = 0; i < top.size(); i++) {
        std::cout << top[i] << " ";
        if(!vectorContains(prevTop, top[i])) std::cout << "-\n";
        else std::cout << positionChange(prevTop, top, top[i]) << "\n";
    }
    prevTop = top;

}

void newNot(def::unord_map& currentNot) {
    currentNot.clear();
}

int main() {
    std::string lineOfInput;
    size_t lineNumber = 0;
    uint32_t maxTrackNumber = 0;
    bool eof = false;
    def::unord_map currentNot;
    def::unord_map allTimeTop;
    std::vector<uint32_t> prevNot;
    std::vector<uint32_t> prevTop;
    def::unord_set droppedTracks;

    while (true) {
        eof = !input(lineOfInput, lineNumber);
        InputType type = simpleValidateAndMatch(lineOfInput);
        
        if (type == InputType::ERROR) {
            printError(lineOfInput, lineNumber);
        }
        else if (type == InputType::DATA) {
            auto vec = strToVector(lineOfInput);
            if (checkForDuplicates(vec) || checkForDropped(vec, droppedTracks) || vectorMax(vec) > maxTrackNumber) {
                printError(lineOfInput, lineNumber);
            }
            else {
                vectorAddToMap(vec, currentNot);
            }
        }
        else if (type == InputType::NEW) {
            uint32_t newMax = strToSingleNumber(lineOfInput);
            if (newMax < maxTrackNumber) {
                printError(lineOfInput, lineNumber);
            }
            else {
                printNot(currentNot, droppedTracks, prevNot);
                updateTop(currentNot, allTimeTop);
                maxTrackNumber = newMax;
                newNot(currentNot);
            }
        }
        else if (type == InputType::TOP) {
            topNot(allTimeTop, prevTop);
        }

        if (eof) {
            break;
        }
    }

    return 0;
}