#ifndef TEST_PLUGIN_H_
#define TEST_PLUGIN_H_

#include <string>

#include <dcl/plugin.h>

class test_object: public dbp::disposable {
public:
	virtual std::string test() = 0;
};

#endif /*TEST_PLUGIN_H_*/
