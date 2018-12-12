#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <string>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <memory>
#include <algorithm>
#if defined(WIN32) || defined(MS_WINDOWS)
#include <io.h>
#include <fcntl.h>
#include "dirent-win.h"
#else
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "argh.h"

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::flush;

using std::ostringstream;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::ios;
using std::fstream;

using std::list;
using std::vector;


using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;

using std::runtime_error;
using std::exception;

#define min(a, b) ((a) < (b) ? (a) : (b))

struct result_t {
    int nsequences;
    string seqstrm;
    string logger;
    string summary;
};

#define ulong int
extern const int INT_SIZE;
extern const int FLOAT_SIZE;

typedef vector<int> vint;
typedef vector<vector<int>> vvint;
typedef vector<unsigned int> vuint;

extern vuint vuint_null;

bool file_exists(const string &name);

unsigned long file_size(fstream& file);

int* read_file(fstream& file, unsigned long flen);

int num_lines(const string &filename);

list<string> list_files(const string& folder, const string& prefix = "");

string random_id(const int len);

string get_temp_folder();

extern const string TMPDIR;

#endif
