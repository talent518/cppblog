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

	struct stat st;
	if(lstat(ss.str().c_str(), &st))
		response().make_error_response(404);
	else {
		std::ostringstream orange;
		size_t b = 0, e = st.st_size - 1;

		if(!request().http_range().empty()) {
			sscanf(request().http_range().c_str(), "bytes=%lu-%lu", &b, &e);
			orange << "bytes " << b << "-" << e << "/" << st.st_size;
			response().content_range(orange.str());
		}

		size_t n = e-b+1;
		time_t t = time(NULL);

		response().content_length(n);
		response().accept_ranges("bytes");
		response().cache_control("max-age=604800");
		response().date(t);
		response().age(86400);
		response().last_modified(st.st_mtime);
		response().expires(t + 604800);
		response().vary("Accept-Encoding");

		std::ostringstream etag;
		etag << std::hex << st.st_size << "-" << std::hex << st.st_mtime;
		response().etag(etag.str());

		//response().io_mode(cppcms::http::response::asynchronous);
		if(n != (size_t) st.st_size) response().status(206);

		std::ifstream f(ss.str().c_str());
		f.seekg(b);
		response().out() << f.rdbuf();
	}
}

} // admin
} // apps
