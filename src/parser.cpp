#include "parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <vector>

static inline void ltrim(std::string& s){ size_t i=0; while(i<s.size()&&std::isspace((unsigned char)s[i])) ++i; s.erase(0,i); }
static inline void rtrim(std::string& s){ size_t i=s.size(); while(i>0&&std::isspace((unsigned char)s[i-1])) --i; s.erase(i); }
static inline void trim(std::string& s){ ltrim(s); rtrim(s); }

static inline bool is_int(const std::string& s){
    if (s.empty()) return false;
    size_t i = (s[0]=='+' || s[0]=='-') ? 1 : 0;
    if (i >= s.size()) return false;
    for (; i < s.size(); ++i) if (!std::isdigit((unsigned char)s[i])) return false;
    return true;
}

ParseResult parse_stream(std::istream& is) {
    ParseResult result;
    std::string line;
    bool header_consumed = false;

    while (std::getline(is, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Consume the header once: "Return <MAX_MEMORY>" OR just "<MAX_MEMORY>"
        if (!header_consumed) {
            std::istringstream hs(line);
            std::string tok0, tok1;
            if (hs >> tok0) {
                if (tok0 == "Return") {
                    if (!(hs >> tok1) || !is_int(tok1)) {
                        throw std::runtime_error("Header line must be: Return <int>");
                    }
                    result.max_memory = std::stoll(tok1);
                    header_consumed = true;
                    continue; // move on to node lines
                } else if (is_int(tok0)) {
                    result.max_memory = std::stoll(tok0);
                    header_consumed = true;
                    continue;
                } else {
                    // Not a memory header; fall through and parse as a node line
                    header_consumed = true;
                }
            }
        }

        // Node line format (required): id name arity <input_ids...> run_mem output_mem time_cost
        std::istringstream ss(line);
        int id = -1, arity = 0;
        long long run_mem = 0, output_mem = 0, time_cost = 0;
        std::string name;

        if (!(ss >> id >> name >> arity)) {
            throw std::runtime_error("Malformed node line (expected: id name arity ...): " + line);
        }
        if (id < 0 || arity < 0) {
            throw std::runtime_error("Negative id/arity not allowed: " + line);
        }
        std::vector<Node> inputs;
        inputs.reserve((size_t)arity);
        for (int i = 0; i < arity; ++i) {
            int in_id = -1;
            if (!(ss >> in_id) || in_id < 0 || (size_t)in_id >= result.nodes.size()) {
                throw std::runtime_error("Input id not yet defined or invalid on line: " + line);
            }
            inputs.push_back(result.nodes[(size_t)in_id]); // copy (simple model)
        }

        std::vector<long long> tail;
        tail.reserve(3);
        long long value = 0;
        while (ss >> value) {
            tail.push_back(value);
        }
        if (tail.empty()) {
            throw std::runtime_error("Missing tail ints (run_mem output_mem time_cost) on line: " + line);
        }
        if (tail.size() > 3) {
            throw std::runtime_error("Too many tail ints on line: " + line);
        }

        run_mem = tail[0];
        output_mem = (tail.size() >= 2) ? tail[1] : 0;
        time_cost = (tail.size() >= 3) ? tail[2] : 0;

        result.nodes.emplace_back(id, name, inputs, run_mem, output_mem, time_cost);

        if (name == "Return" && !inputs.empty()) {
            result.output_name = inputs.front().getName();
        }
    }

    return result;
}

ParseResult parse_file(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return parse_stream(ifs);
}
