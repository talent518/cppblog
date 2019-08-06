#pragma once
#include <cppcms/application.h>
#include <string>
#include <map>

namespace apps {
	class media :public cppcms::application {
	public:
		media(cppcms::service &s);

	private:
		void prepare(std::string path);
		typedef std::map<std::string,std::string> mime_type;
		std::string document_root_;
		mime_type mime_;
	};
}
