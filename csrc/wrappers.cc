#include "common.h"
#include "wrappers.h"
#include "Env.h"
#include "makebin.h"
#include "getconf.h"
#include "SpadeArguments.h"
#include "Sequence.h"
#include "exttpose.h"

void clean_up(const string& tmpprefix, ostream& logger) {
    list<string> tmpfiles = list_files("/tmp", tmpprefix);
    for (string& tmpfile : tmpfiles) {
        string filepath = "/tmp/" + tmpfile;
        if(remove(filepath.c_str()) != 0) {
            logger << "Error deleting file " << filepath << endl;
        }
        else {
            logger << "Cleaned up successful: " << filepath << endl;
        }
    }
}

result_t runSpade(const string &filename, spade_arg_t args, const string& tmpdir) {
    shared_ptr<Env> envptr = make_shared<Env>();
    Env &env(*envptr);

    if (!file_exists(filename)) {
        throw runtime_error("File " + filename + " does not exist.");
    }

    if (args.support <= 0 || args.support > 1) {
        throw runtime_error("Support must be a floating point in range (0-1]");
    }

    if (args.mingap > 0 && args.maxgap > 0 && args.maxgap < args.mingap) {
        args.mingap = args.maxgap;
    }

    int nrows = num_lines(filename);
    ostringstream opt;

    auto nop = static_cast<int>(ceil((nrows + 2 * nrows) * sizeof(long) / pow(4, 10) / 5));
    if (args.memsize > 0) {
        opt << " -m " << args.memsize;
        nop = static_cast<int>(ceil(nop * 32 / float(args.memsize)));
    }

    if (args.numpart > 0) {
        if (args.numpart < nop) {
            env.logger << "numpart less than recommended\n";
        }
        nop = args.numpart;
    }

    string random_suffix = random_id(16);
    string tmpprefix = "cspade-" + random_suffix;
    string otherfile = tmpdir + tmpprefix;
    string datafile = otherfile + ".data";

    ostringstream makebin_args;
    ostringstream getconf_args;
    ostringstream exttpose_args;
    ostringstream spade_args;

    makebin_args << "makebin " << filename << " " << datafile;
    getconf_args << "getconf -i " << otherfile << " -o " << otherfile;
    exttpose_args << "exttpose -i " << otherfile << " -o " << otherfile << " -p " << nop << " -l -x -s " << args.support;

    if (args.maxsize > 0) {
        opt << " -Z " << args.maxsize;
    }
    if (args.maxlen > 0) {
        opt << " -z " << args.maxlen;
    }
    if (args.mingap > 0) {
        opt << " -l " << args.mingap;
    }
    if (args.maxgap > 0) {
        opt << " -u " << args.maxgap;
    }
    if (args.maxwin > 0) {
        opt << " -w " << args.maxwin;
    }
    if (not args.bfstype) {
        opt << " -r";
    }
    if (args.tid_lists) {
        opt << " -y";
    }

    spade_args << "spade -i " << otherfile << " -s " << args.support << opt.str() << " -e " << nop << " -o";

    try {
        makebinWrapper(makebin_args.str(), envptr);
        getconfWrapper(getconf_args.str(), envptr);
        exttposeWrapper(exttpose_args.str(), envptr);
        result_t result = sequenceWrapper(spade_args.str(), envptr);

        clean_up(tmpprefix, env.logger);
        return result;
    }
    catch (runtime_error& e) {
        clean_up(tmpprefix, env.logger);
        cerr << e.what();
        throw e;
    }
    catch (std::exception& e) {
        clean_up(tmpprefix, env.logger);
        ostringstream message;
        message << "Caught '" << typeid(e).name() << "' exception: " << e.what() << endl;
        cerr << message.str();
        throw runtime_error(message.str());
    }
    catch (...) {
        ostringstream message;
        message << "sequence: Caught unknown exception" << endl;
        cerr << message.str();
        throw runtime_error(message.str());
    }
}
