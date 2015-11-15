/*
 * daemon_application.cpp
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
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

#include <csignal>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <dcl/daemon_application.h>
#include <dcl/delegate.h>
#include <dcl/singleton.h>
#include <dcl/strutils.h>

namespace dbp {
namespace local {

#ifndef HAVE_DAEMON
int daemon(int nochdir, int noclose) {
	int fd;
	switch (fork()) {
		case -1:
			return (-1);
		case 0:
			break;
		default:
			_exit(EXIT_SUCCESS);
	}
	if (setsid() == -1)
		return (-1);
	if (!nochdir)
		chdir("/");
	if (!noclose && (fd = open("/dev/null", O_RDWR, 0)) != -1) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO)
			close (fd);
	}
	return (0);
}
#endif

class daemon_application: public daemon_application_int,
  public singleton<daemon_application> {
	friend class singleton<daemon_application>;
public:
	virtual void set_name(const std::string &name) {
		_app.set_name(name);
	};
	virtual void set_description(const std::string &description) {
		_app.set_description(description);
	};
	virtual void register_cmdline_parameter(const cmdline_parameter &param,
	  cmdline_parameter_handler handler) {
		_app.register_cmdline_parameter(param, handler);
	};
	virtual void on_execute(on_execute_handler handler) {
		execute_handler = handler;
	};
	virtual void on_exception(on_exception_handler handler) {
		exception_handler = handler;
	};
	virtual int run(int argc, char *argv[]) {
		return _app.run(argc, argv);
	};
	virtual void on_reload_configuration(on_reload_configuration_handler handler) {
		reload_configuration_handler = handler;
	};
	virtual void on_pause(on_pause_handler handler) {
		pause_handler = handler;
	};
	virtual void on_resume(on_resume_handler handler) {
		resume_handler = handler;
	};
	virtual void on_terminate(on_terminate_handler handler) {
		terminate_handler = handler;
	};
	//! Returns the logger based on system logging mechanism
	virtual dbp::logger& get_logger()  {
		return logger;
	};
protected:
	daemon_application();
private:
	class daemon_logger: public dbp::logger {
	public:
		daemon_logger() {
			openlog(NULL, LOG_PID, LOG_DAEMON);
		}
		virtual ~daemon_logger() {
			closelog();
		}
		virtual void log(log_level::log_level level, const std::string &message) {
			syslog(level, "%s", message.c_str());
		};
	};
	application &_app;
	on_reload_configuration_handler reload_configuration_handler;
	on_pause_handler pause_handler;
	on_resume_handler resume_handler;
	on_terminate_handler terminate_handler;
	on_execute_handler execute_handler;
	on_exception_handler exception_handler;
	daemon_logger logger;
	static void signal_handler(int sig);
	int daemon_execute();
	void daemon_exception(const std::exception &e);
};

daemon_application::daemon_application(): _app(application::instance()) {
	_app.on_execute(create_delegate(this, &daemon_application::daemon_execute));
	_app.on_exception(create_delegate(this, &daemon_application::daemon_exception));
}

void daemon_application::signal_handler(int sig) {
	daemon_application &app =
	  singleton<daemon_application>::instance();
	switch(sig) {
		// reload daemon configuration file on SIGHUP signal
		case SIGHUP:
			if (app.reload_configuration_handler) {
				app.reload_configuration_handler();
				app.get_logger().info(_("configuration file reloaded"));
			}
			break;
		// terminate daemon on SIGTERM signal
		case SIGQUIT:
		case SIGTERM:
			if (app.terminate_handler)
				app.terminate_handler();
			break;
		// wait for all child processes
		case SIGCHLD:
			pid_t pid;
			int stat;
			while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {};
	}
}

int daemon_application::daemon_execute() {
	// run in the background
	if (daemon(false, false) != 0) {
		return -1;
	}
	int rslt = -1;
	get_logger().info(_("successfully started"));
	// catch signals
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGCHLD, signal_handler);
	// execute main application code
	try {
		if (execute_handler)
			rslt = execute_handler();
	}
	catch (std::exception &e) {
		if (exception_handler)
			exception_handler(e);
		else
			daemon_exception(e);
	}
	// close syslog
	get_logger().info(_("successfully terminated"));
	return rslt;
}

void daemon_application::daemon_exception(const std::exception &e) {
	get_logger().critical((format(_("Fatal error: {0}")) % e.what()).str());
}

}} // namespace
