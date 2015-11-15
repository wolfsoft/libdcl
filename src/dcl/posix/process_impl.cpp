/*
 * process_impl.cpp
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2010 Dennis Prochko <wolfsoft@mail.ru>
 *
 * DCL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * DCL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DCL; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "dcl/process.h"
#include "dcl/strutils.h"

namespace dbp {
namespace local {

using namespace std;

#define BUF_SIZE 4096

void str2argv(const string src, strings &argv) {
	string arg;
	enum state {
		SYMBOL,
		SKIP,
		SKIP_UNTIL
	} s = SYMBOL;
	for (string::const_iterator i = src.begin(); i != src.end(); ++i) {
		switch (*i) {
			case '\\':
				s = SKIP;
				break;
			case '"':
				if (s == SKIP_UNTIL)
					s = SYMBOL;
				else
					s = SKIP_UNTIL;
				break;
			case ' ':
				switch (s) {
					case SKIP:
						s = SYMBOL;
						arg += *i;
						break;
					case SKIP_UNTIL:
						arg += *i;
						break;
					default:
						argv.push_back(arg);
						arg.clear();
						break;
				}
				break;
			default:
				if (s == SKIP)
					s = SYMBOL;
				arg += *i;
				break;
		}
	}
	if (!arg.empty())
		argv.push_back(arg);
}

class process_impl: public process_int {
public:
	//! Constructor
	process_impl(): _in(NULL), _out(NULL), _err(NULL) {
	}
	//! Constructor
	process_impl(std::istream &in, std::ostream &out,
	  std::ostream &err): _in(&in), _out(&out), _err(&err) {
	}
	//! Destructor
	virtual ~process_impl() {
	}
	virtual int execute(const std::string &path, const std::string &args) {
		// initialize child-parent pipes
		int in[2], out[2], err[2];
		if (pipe(in) < 0)
			throw process_exception(_("pipe failed"));
		if (pipe(out) < 0) {
			close(in[0]);
			close(in[1]);
			throw process_exception(_("pipe failed"));
		}
		if (pipe(err) < 0) {
			close(in[0]);
			close(in[1]);
			close(out[0]);
			close(out[1]);
			throw process_exception(_("pipe failed"));
		}
		pid_t pid = vfork();
		if (pid == 0) {
			// child process, prepare argv[]
			strings s;
			str2argv(args, s);
			vector<char*> a;
			a.push_back(const_cast<char*>(path.c_str()));
			for (strings::iterator i = s.begin(); i != s.end(); ++i) {
				a.push_back(const_cast<char*>(i->c_str()));
			}
			a.push_back(NULL);
			// replace stdio handles to pipes
			dup2(in[0], STDIN_FILENO);
			dup2(out[1], STDOUT_FILENO);
			dup2(err[1], STDERR_FILENO);
			// close remaining pipe handles
			close(out[0]);
			close(out[1]); 
			close(err[0]);
			close(err[1]);
			close(in[0]);
			close(in[1]);
			// launch the process
			if (execv(a[0], &a[0]) < 0)
				throw process_exception(_("execv failed"));
		} else if (pid < 0) {
			throw process_exception(_("vfork failed"));
		} else {
			// parent process; close no needed pipe handles
			close(in[0]);
			close(out[1]);
			close(err[1]);
			// write input
			FILE* stream = fdopen(in[1], "w");
			char buffer[BUF_SIZE];
			if (_in)
				while (_in->good()) {
					_in->read(buffer, BUF_SIZE - 1);
					buffer[_in->gcount()] = 0;
					fputs(buffer, stream);
				}
			// close remaining handles
			fclose(stream);
			// wait for child process
			int status;
			waitpid(pid, &status, 0);
			// read child's output
			stream = fdopen(out[0], "r");
			if (_out)
				while (!feof (stream)
				  && !ferror (stream)
				  && fgets (buffer, BUF_SIZE, stream) != NULL) {
					_out->write(buffer, strlen(buffer));
					if (!_out->good()) break;
				};
			fclose(stream);
			// read process error output
			stream = fdopen(err[0], "r");
			if (_err)
				while (!feof (stream)
				  && !ferror (stream)
				  && fgets (buffer, BUF_SIZE, stream) != NULL) {
					_err->write(buffer, strlen(buffer));
					if (!_err->good()) break;
				};
			fclose(stream);
			return WEXITSTATUS(status);
		}
		return 0;
	}
private:
	std::istream *_in;
	std::ostream *_out;
	std::ostream *_err;
};

}} // namespace
