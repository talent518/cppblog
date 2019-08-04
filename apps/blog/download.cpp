#include <apps/blog/download.h>
#include <data/blog/master.h>

#include <cppdb/frontend.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/crypto.h>
#include <cppcms/util.h>
#include <cppcms/http_file.h>

#include <booster/perl_regex.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <fstream>
#include <iostream>

namespace apps {
namespace blog {

download::download(cppcms::service &s) : master(s)
{
	mapper().assign("/{1}/{2}/{3}");
	dispatcher().assign("/([0-9a-f]+)/(\\d+)/(.*)/?",&download::prepare,this,1,2,3);
}

void download::prepare(std::string md5, std::string size, std::string name)
{
	std::ostringstream url;
	std::ostringstream ss;
	data::blog::master c;
	booster::regex re("^[0-9a-zA-Z-_\\.]+$", booster::regex::perl);
	std::string docroot = settings().get("file_server.document_root", ".");

	master::prepare(c);

	url << c.media << "/upload/" << md5.substr(0,2) << "/" << md5.substr(2,2) << "/" << md5.substr(4) << "-" << std::hex << atol(size.c_str()) << name.substr(name.rfind("."));

	ss << "attachment; filename=\"" << name << "\"";

	if(!booster::regex_match(name, re))
		ss << "; filename*=utf-8''" << cppcms::util::urlencode(name);

	response().set_content_header("application/octet-stream");
	response().set_header("Content-Disposition", ss.str());

	ss.str("");
	ss << docroot << url.str();

	std::ifstream f(ss.str().c_str());
	if(!f)
		response().make_error_response(404);
	else
		response().out() << f.rdbuf();
}

} // admin
} // apps
