#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <optional>
#include <sstream>
#include <type_traits>

template <typename T>
struct is_vector : std::false_type{};
template <typename T>
struct is_vector<std::vector <T>> : std::true_type{};
class ParserJSON {
    private :
    std::string filename;
    std::optional<std::string> optionalWriteFilename;
    std::optional<std::unordered_map<std::string, std::string>> rs;
    std::string toJsonFormat(const std::string & key, const std::string & val) const{
        std::string value = val;
        this->trim(value);
        if(value == "true" || value == "false") {
            return "\"" + key + "\" : " + value;
        }

        else if(!value.empty() && (value.front() == '[' || value.front() == '{')) {
            return "\"" + key + "\" : " + value;
        }
        try {
            size_t pos;
            std::stod(value, &pos);
            if(pos == value.size()) {
                return "\"" + key + "\" : " + value;
            }
            
        } catch(...) {
            try {
                size_t pos;
            std::stoi(value, &pos);
            if(pos == value.size()) {
                return "\"" + key + "\" : " + value;
            }
            } catch(...) {}
        }
        return "\"" + key + "\" : " + "\"" + value + "\""; 
    }

    std::string parseVectorOfStructures(std::istream &stream) const {
        std::stringstream ss ;
        std::string buffer;
        size_t count = 1;
        bool inQ = false;
        ss << "[\n";
        char c;
        while(stream.get(c)) {
            if(c == '"') inQ = !inQ;
            if(!inQ && isspace(c)) continue;
            if(!inQ) {
            if(c == '[') {
                count++;
            } 
           else if(c == ']') {
                count--;
            } 
            if(count == 0) break;
            ss << c;
            if(c == '{' || c == '}' || c == '[' || c == ']' || c == ',') ss << "\n ";

        }else ss << c;

        }

        ss << "\n]";
        return ss.str();
    }

    void parseArr(std::istream & file) {
        std::string buffer;
        size_t count = 0;
        bool objectStarted = false;
        std::string object;
        size_t index = 0;
        std::string checker;
        while(getline(file, buffer)) {
            this->trim(buffer);
            checker.clear();
            size_t targetIndex = buffer.find_last_of('"');

            if(targetIndex != std::string::npos) {
                checker = buffer.substr(targetIndex);
            }
            
            if(buffer.front() == '{') {
                objectStarted = true;
                count++;
            }
            else if(!this->isEmpty(checker)) {
                if(checker.find('{') != std::string::npos) {
                    count++;
                }
            }
            
            if(objectStarted) object += "\n" + buffer;
            if(buffer.front() == '}') {
            count--;
            if(count == 0) {
                objectStarted = false;

                this->trim(object);
                if(object.back() == ',') object.pop_back();
                rs.value()[std::to_string(index++)] = object;
                object.clear();

            }
           }
        }
        
    }
    void parseStream(std::istream & stream) {
        rs = std::unordered_map<std::string, std::string> ();

        std::string buffer;
        bool start = false;
        while(getline(stream, buffer)) {
            if(this->isEmpty(buffer)) continue;
            if((buffer.find("//") ==0) || (buffer.find("#") == 0)) continue;
            if(!start) {
                size_t openBrace = buffer.find("{");
                if(openBrace != std::string::npos) {
                    start = true;

                    std::string remainder = buffer.substr(openBrace + 1);

                    if(this->isEmpty(remainder)) continue;

                    buffer = remainder;
                } 

                
            } 
            if(buffer.find("}") != std::string::npos && buffer.find(":") == std::string::npos) {
                break;
            }
            auto x = this->add(buffer, stream);
            if(!x.has_value()) continue;
            rs.value().insert({x.value().first, x.value().second});

        }

    }

    std::pair<size_t, size_t> receiveIndexesOfObjectInVector(std::stringstream & ss){
        size_t first = 0;
        size_t last = 0;
        size_t count = 0;
        
        char c;
        bool foundFirst  = false;
        size_t current = 0;

        while(ss.get(c)) {
            if(c == '{') {
                if(!foundFirst) {
                    first = current;
                    foundFirst = true;
                }
                count++;
            } 
            else if(c == '}') {
                count--;
                if(count == 0 && foundFirst) {
                    last = current;
                    return {first, last};
                }
            }
            current++;

        }

        return {0,0};

    }
    public:

    void trim(std::string & str) const{
        size_t start = str.find_first_not_of(" \t\n\r");
        
        if(start == std::string::npos) {
            return;
        }

        size_t last = str.find_last_not_of(" \t\n\r");

         if(last == std::string::npos) {
            return;
        }

        str = str.substr(start, (last - start + 1));
    }
    template <typename T>

    std::optional<T> getValue(const std::string & key) const{
        if(!rs.has_value()) return std::nullopt;
        auto & map = rs.value();

        auto target = map.find(key);

        if(target == this->rs.value().end()) return std::nullopt;
 
        if constexpr (is_vector<T>::value) {
            using element = typename T::value_type;
            std::vector<element> res;

            T val;
            
            std::string item = target->second;
            this->vectorNormalize(item);
            size_t first = item.find_first_of('[');
            if(first == std::string::npos) return std::nullopt;
            size_t last = item.find_last_of(']');
            if(last == std::string::npos) return std::nullopt;
            item = item.substr(first + 1, (last - first));
            std::stringstream ss (item);
            while(getline(ss, item, ',')) {
                this->trim(item);
                this->normalize(item);

                std::stringstream converter (item);
                element value;

                if(converter >> value) {
                    res.push_back(value);
                }
            }

            return res;
            
        }

           else if constexpr (std::is_same_v<T, bool>) {
                if(target->second == "true") return true;
                else if(target->second == "false") return false;

                return std::nullopt;
            }
            else {
                std::string copy = target->second;
                if constexpr (std::is_same_v<T, std::string>) {
                    this->normalize(copy);

                    return copy;
                }
                else {
            std::stringstream ss (target->second);

            T value;

            if(ss >> value) {
                return value;
            }
        }
    }
        return std::nullopt;
    }
    void vectorNormalize(std::string & target) const{
        if(target.empty()) return;
        this->trim(target);
        if(target.back() == ',') {
            target.pop_back();
        }
    }
    void normalize(std::string & target) const{
        if(target.empty()) return;
        if(target.front() == '{') return;
        size_t first = target.find_first_of('"');
        size_t second = target.find_first_of('[');
        if(second != std::string::npos && second < first) {
            this->vectorNormalize(target);
            return;
        }

        else if(first == std::string::npos && second == std::string::npos) {
            size_t last = target.find_last_of(',');
            if(last == std::string::npos) return;
            target = target.substr(0, last);
        }

        size_t last = target.find_last_of('"');

        if(last == std::string::npos) {
            return;
        }

        target = target.substr(first + 1, (last - first - 1));
    }

    std::optional<std::pair<std::string, std::string>> add (std::string  &str, std::istream & file) {
        size_t del = str.find(":");
        if(del == std::string::npos) {
            return std::nullopt;
        }
        
        std::string key = str.substr(0, del);
        std::string value = str.substr(del + 1);
        
        this->trim(key);
        this->trim(value);
        this->normalize(key);
        

        if(!value.empty() && value.front() == '{') {
            
            int braces = 0;

            for(const char & c : value) {
                if(c == '{') braces++;
                else if(c == '}') braces--;
            }

            std::string buffer;

            while(braces > 0 && std::getline(file,buffer)) {
                trim(buffer);
                value += "\n" + buffer;

                for(const char & c : buffer) {
                if(c == '{') braces++;
                else if(c == '}') braces--;
            }
            }
            size_t lastBrace = value.find_last_of("}");
            if(lastBrace != std::string::npos) {
                value = value.substr(0, lastBrace + 1);
            }
        }   else if(!value.empty() && value.back() == '[') {
            
          value =  this->parseVectorOfStructures(file);  
        }

        else {
               this->normalize(value);
        }

        if(!value.empty() && value.back() == ',') value.pop_back();
        this->trim(value);
        if(!value.empty()) {
            if(value.front() == '{' || value.front() == '[') {
                size_t idx = (value.front() == '{') ? value.find_last_of("}") : value.find_last_of("]");
                if(idx != std::string::npos) {
                     value = value.substr(0, idx + 1);
                }
               
            }
        }
        return std::pair{key, value};
    }

    ParserJSON(std::string f) : filename(f) {}
    
    bool isEmpty(std::string_view rs) const{
        if(rs.empty() || rs.find_first_not_of(' ') == std::string::npos) return true;
        return false;
    }

    void parse (void) {
        std::ifstream file(filename);
                
        if(!file.is_open()) {
            std::cerr << "Invalid file name" << std::endl;
            rs = std::nullopt;
            return;
        }
        rs = std::unordered_map<std::string, std::string> ();
        std::string buffer;
        bool start = false;
        size_t count = 0;
        
        while(getline(file, buffer)) {
           
            if(this->isEmpty(buffer)) continue;
            if((buffer.find("//") ==0) || (buffer.find("#") == 0)) continue;
            this->trim(buffer);

            if(start) {
            if(buffer.back() == '{') count++;
            else if(buffer.back() == '}') count--;
            if(count == 0) break;
            }

            if(!start) {
                
                if(buffer.front() == '{') {
                    start = true;
                    count++;
                    continue;
                }
                else if(buffer.front() == '[') {
                    parseArr(file);
                    return;
                }
                continue;
            }
            
            
            auto x = this->add(buffer, file);
            if(!x.has_value()) continue;
            rs.value().insert({x.value().first, x.value().second});

        }

    }
    
    void print(void) {
        if(!rs.has_value()) return;
        auto & map= rs.value();
        for(const auto & x :map) {
            std::cout << x.first << ": " <<  x.second << std::endl;
        }
    }
    template <typename T>
    T get_or(const std::string & key, T def) const {
        return this->getValue<T>(key).value_or(def);
    }

    bool exists(const std::string & key) const {
        if(!rs.has_value()) return false;
        auto & map = rs.value();

        return map.find(key) != map.end();
    }

    std::optional<ParserJSON> getObject(const std::string & key) const{
        if(!this->exists(key)) return std::nullopt;

        ParserJSON obj ("");
        auto target = rs.value().find(key);
        std::stringstream ss (target->second);
        obj.parseStream(ss);
        
        return obj;

    }

    void stringify(void) {
        std::string currentFile = this->filename;
        if(this->optionalWriteFilename.has_value()) currentFile = this->optionalWriteFilename.value();
        std::ofstream file(currentFile);
        
        if(!file.is_open()) return;
        if(!rs.has_value()) return;
       file << recursiveStringify(*this);
    }
        std::string toString(void) {
            std::stringstream ss;
       ss << recursiveStringify(*this);
       return ss.str();
    }
    std::string recursiveStringify(const ParserJSON & ps, int level = 1) {
        std::string indent(level * 2, ' ');
        std::string result = "{\n";
        if(!ps.rs.has_value()) return "";
        auto & map = ps.rs.value();
        
        for(auto x = map.begin(); x != map.end(); x++) {
            result += indent + "\"" + x->first + "\" : ";

            if(!x->second.empty() && x->second.front() == '{') {
            ParserJSON inner ("");
            std::stringstream ss(x->second);
            inner.parseStream(ss);

            result += recursiveStringify(inner, level + 1);
            }

            else if(!x->second.empty() && x->second.front() == '[') {
                result += x->second;
            }

            else {
                result += "\"" + x->second + "\"";
            }

            if(std::next(x) != map.end()) result += ",";
            result += "\n";
        }

        result += std::string((level - 1) * 2, ' ') + "}";

        return result;
    }
    template <typename T>
    void insert(std::string key, T val) {
        if(!rs.has_value()) {
            rs = std::unordered_map<std::string, std::string> ();
        }

        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char *>) {
            rs.value()[key] = std::string(val);
        }
        else if constexpr (is_vector<T>::value) {
            using element = typename T::value_type;
            bool vectorStr = false;
            if constexpr (std::is_same_v<element, std::string>) vectorStr = true;
            std::stringstream ss;
            ss << "[";
            std::string buffer;
            for(size_t x = 0; x < val.size(); x++) {
                buffer = "";
                if(x + 1 < val.size()) {
                    buffer = ",";
                }
                if(vectorStr) {
                    ss << "\"" <<  val[x] << "\"" << buffer;
                }
                else {
                    ss << val[x] << buffer; 
                }
                
            }

            ss << "]";

            rs.value()[key] = ss.str();

        }
        else if constexpr(std::is_arithmetic_v<T>) {
            rs.value()[key] = std::to_string(val);
        }
        else {
            return;
        }


    }
    void assignWriteFile(std::string name) {
        this->optionalWriteFilename = name;
    }
    template <typename T>
    void updateValue(const std::string & key, T value) {
        if(!rs.has_value()) return;
        auto & map = rs.value();
        map[key] = value;
    }
    std::optional<std::vector<ParserJSON>> getVectorObjects(std::string  key) {
        if(!rs.has_value()) return std::nullopt;
        auto value = this->getValue<std::string>(key);
        this->vectorNormalize(value.value());
        if(!value.has_value()) return std::nullopt;
        std::vector<ParserJSON> ps;
        std::string val;
        for(;;) {
            std::stringstream search (value.value());
            auto res  = this->receiveIndexesOfObjectInVector(search);
                
            if(res.first == 0 && res.second == 0) break;
            val = value.value().substr(res.first, res.second - res.first + 1);
            value.value().erase(res.first, res.second - res.first + 1);
            std::stringstream ss (val);
            ParserJSON tmp ("");
            
            tmp.parseStream(ss);
            ps.push_back(tmp);
            

        }
        
        if(ps.empty()) return std::nullopt;
        return ps;
    }
    ParserJSON() {}
    void listKeys(void) const { 
        if(!rs.has_value()) return;
        for(const auto &x : rs.value()) {
            std::cout << x.first << std::endl;
        }
    }
};
