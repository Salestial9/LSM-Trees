
// lsm_wasm.cpp
#include <string>
#include <fstream>
#include <cstring>
#include <iostream>
#include <vector>
#include <emscripten/emscripten.h>

extern "C" {

std::string mem_keys[100];
std::string mem_vals[100];
int mem_size = 0;

std::string sstable_files[100];
int sstable_count = 0;

int memtable_threshold = 4;

// --- Bloom Filter Additions ---
const int BLOOM_SIZE = 1024;
bool bloom_filters[100][BLOOM_SIZE];

unsigned int hash1(const std::string& str) {
    unsigned int hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % BLOOM_SIZE;
}

unsigned int hash2(const std::string& str) {
    unsigned int hash = 0;
    for (char c : str) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % BLOOM_SIZE;
}
// ------------------------------

int findInMemtable(const std::string &key) {
    for (int i = 0; i < mem_size; i++)
        if (mem_keys[i] == key)
            return i;
    return -1;
}

void WriteSSTable(const std::string &filename) {
    std::ofstream out(filename);
    for (int i = 0; i < mem_size; i++)
        out << mem_keys[i] << "=" << mem_vals[i] << "\n";
}

void Flush() {
    std::string filename = "sstable_" + std::to_string(sstable_count) + ".txt";
    WriteSSTable(filename);
    
    // Populate bloom filter for the new SSTable
    for (int i = 0; i < BLOOM_SIZE; i++) bloom_filters[sstable_count][i] = false;
    for (int i = 0; i < mem_size; i++) {
        bloom_filters[sstable_count][hash1(mem_keys[i])] = true;
        bloom_filters[sstable_count][hash2(mem_keys[i])] = true;
    }

    sstable_files[sstable_count++] = filename;
    mem_size = 0;
}

bool SearchSSTable(int sstable_idx, const std::string &filename, const std::string &key, std::string &value) {
    // Check Bloom filter before reading file
    if (!bloom_filters[sstable_idx][hash1(key)] || !bloom_filters[sstable_idx][hash2(key)]) {
        return false; // Definitely not in this SSTable
    }

    std::ifstream in(filename);
    std::string line;

    while (std::getline(in, line)) {
        int pos = line.find('=');
        if (pos == -1) continue;

        std::string k = line.substr(0, pos);
        std::string v = line.substr(pos + 1);

        if (k == key) {
            value = v;
            return true;
        }
    }
    return false;
}

void Compact() {
    std::string merged_file = "sstable_merged.txt";
    std::ofstream out(merged_file);

    std::vector<std::string> seen_keys;
    std::vector<std::string> seen_vals;

    for (int i = 0; i < sstable_count; i++) {
        std::ifstream in(sstable_files[i]);
        std::string line;

        while (std::getline(in, line)) {
            int pos = line.find('=');
            if (pos == -1) continue;

            std::string k = line.substr(0, pos);
            std::string v = line.substr(pos + 1);

            bool found = false;
            for (size_t j = 0; j < seen_keys.size(); j++) {
                if (seen_keys[j] == k) {
                    seen_vals[j] = v;
                    found = true;
                    break;
                }
            }

            if (!found) {
                seen_keys.push_back(k);
                seen_vals.push_back(v);
            }
        }
    }

    // Populate bloom filter for the compacted SSTable (index 0)
    for (int i = 0; i < BLOOM_SIZE; i++) bloom_filters[0][i] = false;
    
    for (size_t i = 0; i < seen_keys.size(); i++) {
        out << seen_keys[i] << "=" << seen_vals[i] << "\n";
        bloom_filters[0][hash1(seen_keys[i])] = true;
        bloom_filters[0][hash2(seen_keys[i])] = true;
    }

    sstable_files[0] = merged_file;
    sstable_count = 1;
}

EMSCRIPTEN_KEEPALIVE
void put(const char* key, const char* value) {
    int idx = findInMemtable(key);
    if (idx != -1) mem_vals[idx] = value;
    else {
        mem_keys[mem_size] = key;
        mem_vals[mem_size] = value;
        mem_size++;
    }
    if (mem_size >= memtable_threshold) Flush();
}

EMSCRIPTEN_KEEPALIVE
char* get(const char* key) {
    static std::string result;
    std::string key_str = key;

    for (int i = 0; i < mem_size; i++)
        if (mem_keys[i] == key_str) {
            result = mem_vals[i];
            return (char*) result.c_str();
        }

    for (int i = sstable_count - 1; i >= 0; i--) {
        std::string value;
        if (SearchSSTable(i, sstable_files[i], key_str, value)) {
            result = value;
            return (char*) result.c_str();
        }
    }

    result = "NOTFOUND";
    return (char*) result.c_str();
}

EMSCRIPTEN_KEEPALIVE
void flush_table() { Flush(); }

EMSCRIPTEN_KEEPALIVE
void compact_table() { Compact(); }

}
