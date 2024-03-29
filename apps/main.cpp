#include <apps/captcha.h>
#include <apps/config.h>
#include <apps/blog/blog.h>
#include <apps/feed/feed.h>
#include <apps/admin/admin.h>
#include <apps/media.h>

#include <cppcms/service.h>
#include <cppcms/applications_pool.h>
#include <cppcms/cache_interface.h>
#include <cppcms/http_context.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/json.h>
#include <cppcms/session_interface.h>
#include <cppcms/url_mapper.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>
#include <ctime>
#include <string>

// #define ACCESS_LOG 1

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

		if(!settings().get("file_server.enable", false) && settings().get("file_server.media", true)) attach( new apps::media(s), "media", "/media{1}", "/media(/.+)", 1);

		attach( new apps::blog::blog_master(s),
			"blog",
			"{1}",
			"((/.*)?)",1);

		mapper().root(settings().get<std::string>("blog.root"));
	}
	virtual void main(std::string path)
	{
	#ifdef ACCESS_LOG
		clock_t start = std::clock();
		time_t t = time(NULL);
	#endif
		std::string lang = request().http_accept_language();
		int nlang = 0;

		if(!lang.compare(0, 2, "zh") || !lang.compare(0, 2, "cn")) {
			nlang = 1;
			context().locale("zh.UTF-8");
		} else {
			nlang = 2;
			context().locale("en_US.UTF-8");
		}

		if(!session().is_set("nlang") || session().get<int>("nlang") != nlang) {
			session().set<int>("nlang", nlang);
			cache().clear();
		}

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

	#ifdef ACCESS_LOG
		if(request().query_string().length()) {
			path += "?" + request().query_string();
		}

		// print access log
		{
			std::ostringstream ss;

			ss << syscall(SYS_gettid); // TID
			ss << " " << request().remote_addr(); // IP
			ss << " " << strtime(t); // TIME
			ss << " " << request().request_method(); // METHOD
			ss << " " << path << " " << request().server_protocol(); // URI
			ss << " " << request().content_length(); // LENGTH
			ss << " " << (double)(std::clock() - start)/CLOCKS_PER_SEC; // RUN TIME
			ss << " \"" << response().get_header("Status") << "\""; // STATUS
			ss << " \"" << request().http_user_agent() << "\"";
			ss << std::endl;
			std::cout << ss.str();
		}
	#endif
	}
};


int main(int argc,char **argv)
{
	try {
		cppcms::service srv(argc,argv);
		srv.applications_pool().mount(cppcms::applications_factory<blog>());
		apps::init_tex_filer(srv.settings());
		srv.post([&srv](){
			std::cout << "Service started" << std::endl;
			cppcms::json::value api = srv.settings().find("service.api");
			cppcms::json::value port = srv.settings().find("service.port");
			if(port.is_undefined()) {
				port.number(8080);
			}
			if(api.is_undefined()) {
				cppcms::json::value list = srv.settings().find("service.list");
				if(!list.is_undefined()) {
					cppcms::json::array lst = list.array();
					for(unsigned i=0;i<lst.size();i++) {
						api = lst[i].get("api", "http");
						port = lst[i].get("port", "8080");
						std::cout << "[" << api.str() << "] Listen port is " << port.number() << std::endl;
					}
				}
			} else {
				std::cout << "[" << api.str() << "] Listen port is " << port.number() << std::endl;
			}
		});
		srv.run();
	} catch(std::exception const &e) {
		std::cerr << "Failed: " << e.what() << std::endl;
		std::cerr << booster::trace(e) << std::endl;
		return 1;
	}
	return 0;
}
