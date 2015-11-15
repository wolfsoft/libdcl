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

#include <iostream>

#include <windows.h>
#include <winsvc.h>

#include "dcl/singleton.h"
#include "dcl/daemon_application.h"
#include "dcl/strutils.h"

#include "exception_system.h"
#include "libdclbase_msgs.h"

namespace dbp {
namespace local {

using namespace std;
using namespace dbp;

static HINSTANCE own_instance = NULL;

string get_own_module_name() {
	char path[MAX_PATH];
	GetModuleFileName(own_instance, path, sizeof(path));
	return string(path);
}

class daemon_application: public daemon_application_int,
  public singleton<daemon_application> {
	friend class singleton<daemon_application>;
public:
	virtual ~daemon_application();
	virtual void set_name(const std::string &app_name) {
		_app.set_name(app_name);
	};
	virtual std::string get_name() {
		return _app.get_name();
	};
	virtual void set_description(const std::string &description) {
		_app.set_description(description);
	};
	virtual void register_cmdline_parameter(const cmdline_parameter &param,
	  application_int::cmdline_parameter_handler handler) {
		_app.register_cmdline_parameter(param, handler);
	};
	virtual void on_execute(on_execute_handler handler) {
		execute_handler = handler;
	};
	virtual void on_exception(on_exception_handler handler) {
		_app.on_exception(handler);
	};
	virtual int run(int argc, char *argv[]) {
		return _app.run(argc, argv);
	};
	virtual void log(const std::string &message,
	  log_level::log_level level = log_level::info);
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
private:
	daemon_application();
    // incapsulated class
	application &_app;
    // event handlers
	on_reload_configuration_handler reload_configuration_handler;
	on_pause_handler pause_handler;
	on_resume_handler resume_handler;
	on_terminate_handler terminate_handler;
	on_execute_handler execute_handler;
    // system specific variables
	SC_HANDLE _service_manager, _service;
	SERVICE_STATUS_HANDLE _status;
	HANDLE _event_log;
    // system specific functions
	static void svc_main(int argc, char *argv[]);
	static void svc_ctrl_handler(DWORD ctrl);
	void report_service_status(int state, int code);
    // default event handlers
	bool on_install_service(cmdline_parameter &param);
	bool on_uninstall_service(cmdline_parameter &param);
	void daemon_exception(const std::exception &e);
	int daemon_execute();
};


daemon_application::daemon_application(): _app(application::instance()),
  _service_manager(0), _service(0), _status(0), _event_log(0) {
	// initialize windoze service manager
	_service_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	// register additional command line parameters
	register_cmdline_parameter(cmdline_parameter('i',
	  "install", _("install Windows service")),
	  create_delegate(this, &daemon_application::on_install_service));
	register_cmdline_parameter(cmdline_parameter('u',
	  "uninstall", _("uninstall Windows service")),
	  create_delegate(this, &daemon_application::on_uninstall_service));
	// register custom event handlers
	_app.on_execute(create_delegate(this, &daemon_application::daemon_execute));
	_app.on_exception(create_delegate(this, &daemon_application::daemon_exception));
}

daemon_application::~daemon_application() {
	// cleaning up
	if (_service)
		CloseServiceHandle(_service);
	if (_service_manager)
		CloseServiceHandle(_service_manager);
	if (_event_log)
		DeregisterEventSource(_event_log);
}

bool daemon_application::on_install_service(cmdline_parameter&) {
	// service installation
	_service = CreateService(_service_manager, _app.get_name().c_str(),
	  _app.get_name().c_str(), SERVICE_ALL_ACCESS,
	  SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
	  _app.get_path().c_str(), 0, 0, 0, 0, 0);
	if (_service) {
		// if the service is created, update its description
		SERVICE_DESCRIPTION desc;
		desc.lpDescription = (char*)_app.get_description().c_str();
		ChangeServiceConfig2(_service, SERVICE_CONFIG_DESCRIPTION, &desc);
		// Register service event source in the Windows registry
        HKEY key;
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		  string(string("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\") +
		  _app.get_name()).c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL,
		  &key, NULL) != ERROR_SUCCESS) {
			throw exception_system(GetLastError(),
			_("can't create a Windows registry key"));
		}
		string dllname = get_own_module_name();
		if (RegSetValueEx(key, "EventMessageFile", 0, REG_EXPAND_SZ,
		  (LPBYTE)dllname.c_str(), dllname.length() + 1) != ERROR_SUCCESS) {
			RegCloseKey(key);
			throw exception_system(GetLastError(),
			_("can't write a value into the Windows registry key"));
		}
		DWORD ev_type = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | 
		  EVENTLOG_INFORMATION_TYPE;
		if (RegSetValueEx(key, "TypesSupported", 0, REG_DWORD,
		  (LPBYTE)&ev_type, sizeof(ev_type)) != ERROR_SUCCESS) {
			RegCloseKey(key);
			throw exception_system(GetLastError(),
			_("can't write a value into the Windows registry key"));
		}
		if (RegSetValueEx(key, "CategoryMessageFile", 0, REG_EXPAND_SZ,
		  (LPBYTE)dllname.c_str(), dllname.length() + 1) != ERROR_SUCCESS) {
			RegCloseKey(key);
			throw exception_system(GetLastError(),
			_("can't write a value into the Windows registry key"));
		}
		DWORD c_count = 1;
		if (RegSetValueEx(key, "CategoryCount", 0, REG_DWORD,
		  (LPBYTE)&c_count, sizeof(c_count)) != ERROR_SUCCESS) {
			RegCloseKey(key);
			throw exception_system(GetLastError(),
			_("can't write a value into the Windows registry key"));
		}
		RegCloseKey(key);
		// report about success
		cout << _("Windows service installed successfully.") << endl;
		return false;
	} else
		throw exception_system(GetLastError(), _("can't create Windows service"));
}

bool daemon_application::on_uninstall_service(cmdline_parameter&) {
	// service uninstallation
	if (!_service) {
		_service = OpenService(_service_manager, _app.get_name().c_str(), DELETE);
		if (!_service)
			throw exception_system(GetLastError(), _("can't open Windows service"));
	}
	if (DeleteService(_service)) {
		// Unregister service event source in the Windows registry
        RegDeleteKey(HKEY_LOCAL_MACHINE,
		  string(string("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\") +
		  _app.get_name()).c_str());
		// report about success
		cout << _("Windows service uninstalled successfully.") << endl;
		return false;
	} else
		throw exception_system(GetLastError(), _("can't delete Windows service"));
}

void daemon_application::daemon_exception(const std::exception &e) {
	// write to syslog
	string msg = (format(_("Fatal error: {0}")) % e.what()).str();
	if (_event_log)
		log(msg, log_level::error);
	else
		cerr << msg << endl;
}

void daemon_application::log(const std::string &message,
  log_level::log_level level) {
	int ev_type = EVENTLOG_ERROR_TYPE;
	DWORD msg = 0;
	switch (level) {
		case log_level::error:
			msg = MSG_ERROR;
			ev_type = EVENTLOG_ERROR_TYPE;
			break;
		case log_level::warning:
			msg = MSG_WARNING;
			ev_type = EVENTLOG_WARNING_TYPE;
			break;
		case log_level::info:
			msg = MSG_INFO;
			ev_type = EVENTLOG_INFORMATION_TYPE;
			break;
	};
	const char *str = message.c_str();
	ReportEvent(_event_log, ev_type, NULL, msg, NULL, 1, 0, &str, NULL);
}

int daemon_application::daemon_execute() {
	// initialize windoze event log
	_event_log = RegisterEventSource(NULL, get_name().c_str());
	// notify service dispatcher
	report_service_status(SERVICE_START_PENDING, NO_ERROR);
	SERVICE_TABLE_ENTRY table[] = {
		{(CHAR*)get_name().c_str(),
		  (LPSERVICE_MAIN_FUNCTION)svc_main},
		{NULL, NULL}
	};
	if (!StartServiceCtrlDispatcher(table))
		throw exception_system(GetLastError(), _("can't start Windows service"));
	return 0;
}

void daemon_application::svc_main(int, char**) {
	// obtain a application instance
	daemon_application &app =
	  singleton<daemon_application>::instance();
	// report about service starting
	app._status = RegisterServiceCtrlHandler(app.get_name().c_str(),
	  (LPHANDLER_FUNCTION)app.svc_ctrl_handler);
	if (!app._status)
		throw exception_system(GetLastError(),
		  _("can't register Windows service control handler"));
	app.report_service_status(SERVICE_RUNNING, NO_ERROR);
	app.log(_("successfully started"));
	// run the client main code section
	if (app.execute_handler)
	  app.execute_handler();
	// report about service termination
	app.log(_("successfully terminated"));
	app.report_service_status(SERVICE_STOPPED, NO_ERROR);
}

void daemon_application::svc_ctrl_handler(DWORD ctrl) {
	daemon_application &app =
	  singleton<daemon_application>::instance();
	switch (ctrl) {  
		case SERVICE_CONTROL_STOP: 
			app.report_service_status(SERVICE_STOP_PENDING, NO_ERROR);
			// signal the service to stop.
			if (app.terminate_handler)
			  app.terminate_handler();
			break;
		default:
			break;
	}
}

void WINAPI daemon_application::report_service_status(int state, int code) {
	static int check_point = 1;
	SERVICE_STATUS ss;
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
	ss.dwServiceSpecificExitCode = 0;
	ss.dwCurrentState = state;
	ss.dwWin32ExitCode = code;
	ss.dwWaitHint = 3000;
	if (state == SERVICE_START_PENDING)
		ss.dwControlsAccepted = 0;
	else
	  ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	if ((state == SERVICE_RUNNING) || (state == SERVICE_STOPPED))
		ss.dwCheckPoint = 0;
	else
	  ss.dwCheckPoint = check_point++;
	SetServiceStatus(_status, &ss);
}

}} // namespace

extern "C" {
	BOOL WINAPI DllMain(HINSTANCE instance, DWORD, LPVOID) {
	  dbp::local::own_instance = instance;
	  return true;
	}
}
