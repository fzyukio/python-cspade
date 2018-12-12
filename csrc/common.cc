#include <random>
#include "common.h"

const int INT_SIZE = sizeof(int);
const int FLOAT_SIZE = sizeof(float);

vuint vuint_null;
const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const int alphanumlen = sizeof(alphanum) - 1;

const string TMPDIR = get_temp_folder();

unsigned long file_size(fstream& file) {
    std::streampos fsize = 0;
    fsize = file.tellg();
    file.seekg( 0, std::ios::end);
    fsize = file.tellg() - fsize;
    file.clear();
    file.seekg( 0, std::ios::beg);
    return static_cast<unsigned long>(fsize);
}

int* read_file(fstream& file, unsigned long flen) {
    if (!flen) return nullptr;

    auto buffer = new char[flen];
    auto intbuf = (int *) buffer;
    file.read(buffer, flen);
    return intbuf;
}

bool file_exists(const string &name) {
    ifstream f(name.c_str());
    return f.good();
}

/* Reads a file and returns the number of lines in this file. */
int num_lines(const string &filename) {
    FILE *file = fopen(filename.c_str(), "r");
    int lines = 0;
    int c;
    int last = '\n';
    while (EOF != (c = fgetc(file))) {
        if (c == '\n' && last != '\n') {
            ++lines;
        }
        last = c;
    }
    fclose(file);
    return lines;
}


bool starts_with(const string &haystack, const string &needle) {
    return needle.length() <= haystack.length()
           && equal(needle.begin(), needle.end(), haystack.begin());
}


list<string> list_files(const string &folder, const string &prefix) {
    struct dirent *entry;
    list<string> retval;
    DIR *dir = opendir(folder.c_str());
    if (dir == nullptr) {
        return retval;
    }

    bool check_prefix = prefix.length() > 0;

    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        if (check_prefix && starts_with(filename, prefix)) {
            retval.push_back(filename);
        }
    }
    closedir(dir);

    return retval;
}


string random_id(const int len) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, alphanumlen - 1);

    auto *s = new char[len + 1];
    int rand_idx;
    for (int i = 0; i < len; ++i) {
        rand_idx = dis(gen);
        s[i] = alphanum[rand_idx];
    }
    s[len] = 0;
    string retval(s);
    delete [] s;
    return retval;
}

string get_temp_folder() {
#if defined(WIN32) || defined(MS_WINDOWS)
    string buffer;
    buffer.resize(1000);
    const auto new_size = GetTempPathA(buffer.size(), &buffer[0]); //deal with newsize == 0
    buffer.resize(new_size);
    return buffer;
#else
    return "/tmp/";
#endif
}