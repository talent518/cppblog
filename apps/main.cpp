#include <apps/captcha.h>
#include <apps/config.h>
#include <apps/blog/blog.h>
#include <apps/feed/feed.h>
#include <apps/admin/admin.h>

#include <cppcms/service.h>
#include <cppcms/applications_pool.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/json.h>

#include <iostream>
#include <ctime>

std::string strtime(time_t timep) {
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep));
	return tmp;
}

class blog : public cppcms::application{
public:
	blog(cppcms::service &s) : cppcms::application(s)
	{
		attach( new apps::captcha(s),
			"captcha",
			"/captcha{1}",
			"/captcha((/?.*))",1);
		
		attach( new apps::config(s),
			"config",
			"/config{1}",
			"/config((/?.*))",1);

		attach( new apps::admin::admin_master(s),
			"admin",
			"/admin{1}",
			"/admin((/.*)?)",1);

		attach( new apps::feed::feed_master(s),
			"feed",
			"/rss{1}",
			"/rss((/.*)?)",1);

		attach( new apps::blog::blog_master(s),
			"blog",
			"{1}",
			"((/.*)?)",1);

		mapper().root(settings().get<std::string>("blog.root"));
	}
	virtual void main(std::string path)
	{
		clock_t start = std::clock();
		time_t t = time(NULL);

		try {
			response().status(cppcms::http::response::ok);
			cppcms::application::main(path);
		}
		catch(apps::database_version_error const &e) {
			response().set_redirect_header(url("/config/version"));
		}
		catch(apps::database_is_not_configured_error const &e) {
			response().set_redirect_header(url("/config"));
		}

		if(request().query_string().length()) {
			path += "?" + request().query_string();
		}

		// print access log

		std::cout << request().remote_addr(); // IP
		std::cout << " " << strtime(t); // TIME
		std::cout << " " << request().request_method(); // METHOD
		std::cout  << " " << path << " " << request().server_protocol(); // URI
		std::cout << " " << request().content_length(); // LENGTH
		std::cout << " " << (double)(std::clock() - start)/CLOCKS_PER_SEC; // RUN TIME
		std::cout << " \"" << response().get_header("Status") << "\""; // STATUS
		std::cout << " \"" << request().http_user_agent() << "\"";
		std::cout << std::endl; // USER-AGENT
	}
};


int main(int argc,char **argv)
{
	try {
		cppcms::service srv(argc,argv);
		srv.applications_pool().mount(cppcms::applications_factory<blog>());
		apps::init_tex_filer(srv.settings());
		srv.run();
	} catch(std::exception const &e) {
		std::cerr << "Failed: " << e.what() << std::endl;
		std::cerr << booster::trace(e) << std::endl;
		return 1;
	}
	return 0;
}
