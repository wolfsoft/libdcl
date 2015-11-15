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

#include <windows.h> 

#include "exception_system.h"
#include "dcl/strutils.h"
#include "dcl/process.h"

namespace dbp {
namespace local {

using namespace std;

#define BUF_SIZE 4096

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
		DWORD rslt;
		// initialization
		HANDLE pipe_in_r = NULL;
		HANDLE pipe_in_w = NULL;
		HANDLE pipe_out_r = NULL;
		HANDLE pipe_out_w = NULL;
		HANDLE pipe_err_r = NULL;
		HANDLE pipe_err_w = NULL;
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&si, sizeof(STARTUPINFO));
		try {
			try {
				// create IO pipes
				SECURITY_ATTRIBUTES saAttr;
				saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
				saAttr.bInheritHandle = TRUE;
				saAttr.lpSecurityDescriptor = NULL;
				if (!CreatePipe(&pipe_out_r, &pipe_out_w, &saAttr, 0))
					throw exception_system(GetLastError(), _("CreatePipe failed"));
				if (!SetHandleInformation(pipe_out_r, HANDLE_FLAG_INHERIT, 0))
					throw exception_system(GetLastError(), _("SetHandleInformation failed"));
				if (!CreatePipe(&pipe_err_r, &pipe_err_w, &saAttr, 0))
					throw exception_system(GetLastError(), _("CreatePipe failed"));
				if (!SetHandleInformation(pipe_err_r, HANDLE_FLAG_INHERIT, 0))
					throw exception_system(GetLastError(), _("SetHandleInformation failed"));
				if (!CreatePipe(&pipe_in_r, &pipe_in_w, &saAttr, 0))
					throw exception_system(GetLastError(), _("CreatePipe failed"));
				if (!SetHandleInformation(pipe_in_w, HANDLE_FLAG_INHERIT, 0))
					throw exception_system(GetLastError(), _("SetHandleInformation failed"));
				// create child process
				si.cb = sizeof(STARTUPINFO);
				si.hStdError = pipe_err_w;
				si.hStdOutput = pipe_out_w;
				si.hStdInput = pipe_in_r;
				si.dwFlags |= STARTF_USESTDHANDLES;
				string cmdline = path;
				if (!args.empty())
					cmdline += " " + args;
				if (!CreateProcess(NULL,
				  (CHAR*)cmdline.c_str(),  // command line 
				  NULL,                    // process security attributes 
				  NULL,                    // primary thread security attributes 
				  TRUE,                    // handles are inherited 
				  0,                       // creation flags 
				  NULL,                    // use parent's environment 
				  NULL,                    // use parent's current directory 
				  &si,                     // STARTUPINFO pointer 
				  &pi))                    // receives PROCESS_INFORMATION
					throw exception_system(GetLastError(), _("CreateProcess failed"));
				// write data from the input stream to the pipe of the created process
				char buffer[BUF_SIZE];
				DWORD cnt;
				while (_in->good()) {
					_in->read(buffer, BUF_SIZE);
					if (!WriteFile(pipe_in_w, buffer, _in->gcount(), &cnt, NULL))
						throw exception_system(GetLastError(), _("WriteFile failed"));
				}
				CloseHandle(pipe_in_w);
				pipe_in_w = NULL;
				// close the write end of the pipe before reading from the 
				// read end of the pipe, to control child process execution.
				CloseHandle(pipe_out_w);
				pipe_out_w = NULL;
				CloseHandle(pipe_err_w);
				pipe_err_w = NULL;
				// the process have to be finished here
				GetExitCodeProcess(pi.hProcess, &rslt);
				// read from the pipe to the output stream
				BOOL bSuccess;
				for (;;) {
					bSuccess = ReadFile(pipe_out_r, buffer, BUF_SIZE, &cnt, NULL);
					if (!bSuccess || cnt == 0) break;
					_out->write(buffer, cnt);
					if (!_out->good()) break;
				}
				// read from the pipe to the error stream
				for (;;) {
					bSuccess = ReadFile(pipe_err_r, buffer, BUF_SIZE, &cnt, NULL);
					if (!bSuccess || cnt == 0) break;
					_err->write(buffer, cnt);
					if (!_err->good()) break;
				}
			} catch (...) {
				// Close all handles
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				CloseHandle(pipe_in_r);
				CloseHandle(pipe_in_w);
				CloseHandle(pipe_out_r);
				CloseHandle(pipe_out_w);
				CloseHandle(pipe_err_r);
				CloseHandle(pipe_err_w);
				throw;
			}
			// Close all handles
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			CloseHandle(pipe_in_r);
			CloseHandle(pipe_in_w);
			CloseHandle(pipe_out_r);
			CloseHandle(pipe_out_w);
			CloseHandle(pipe_err_r);
			CloseHandle(pipe_err_w);
		} catch (exception_system &e) {
			throw process_exception(e.what());
		}
		return rslt;
	}
private:
	std::istream *_in;
	std::ostream *_out;
	std::ostream *_err;
};

}} // namespace
