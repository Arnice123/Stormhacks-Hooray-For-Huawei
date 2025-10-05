#include "parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

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

ParseResult parse_stream(std::istream& is)
{
    ParseResult result;
    std::string line;
    bool header_consumed = false;

    while (std::getline(is, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

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
                    continue;
                } else if (is_int(tok0)) {
                    result.max_memory = std::stoll(tok0);
                    header_consumed = true;
                    continue;
                } else {
                    header_consumed = true;
                }
            }
        }

        std::istringstream ss(line);
        int id = -1;
        std::string name;
        int arity = 0;
        if (!(ss >> id >> name >> arity)) {
            throw std::runtime_error("Malformed node line (expected: id name arity ...): " + line);
        }
        if (id < 0 || arity < 0) {
            throw std::runtime_error("Negative id/arity not allowed: " + line);
        }

        std::vector<int> input_ids;
        input_ids.reserve(static_cast<std::size_t>(arity));
        for (int i = 0; i < arity; ++i) {
            int in_id = -1;
            if (!(ss >> in_id) || in_id < 0 || static_cast<std::size_t>(in_id) >= result.nodes.size()) {
                throw std::runtime_error("Input id not yet defined or invalid on line: " + line);
            }
            input_ids.push_back(in_id);
        }

        long long run_mem = 0, output_mem = 0, time_cost = 0;
        if (!(ss >> run_mem)) {
            throw std::runtime_error("Missing run_mem on line: " + line);
        }
        long long tmp = 0;
        if (ss >> tmp) {
            output_mem = tmp;
            if (ss >> tmp) {
                time_cost = tmp;
            }
        }

        result.nodes.emplace_back(id, name, std::move(input_ids), run_mem, output_mem, time_cost);

        if (name == "Return" && !result.nodes.back().getInputIds().empty()) {
            int src_idx = result.nodes.back().getInputIds().front();
            if (static_cast<std::size_t>(src_idx) < result.nodes.size()) {
                result.output_name = result.nodes[src_idx].getName();
            }
        }
    }

    return result;
}

ParseResult parse_file(const std::string& path)
{
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return parse_stream(ifs);
}
