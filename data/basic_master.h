#pragma once
#include <cppcms/view.h>
#include <booster/function.h>
#include <cppcms/serialization.h>
#include <math.h>

namespace cppcms {
	namespace json {
		class value;
	}
}

namespace data {
	struct general_info : public cppcms::serializable {
		std::string blog_title;
		std::string blog_description;
		std::string contact;
		std::string copyright_string;

		void serialize(cppcms::archive &a)
		{
			a & blog_title & blog_description & contact & copyright_string;
		}
	};
	struct basic_master :public cppcms::base_content {
		general_info info;
		std::string media;
		std::string host;
		std::string cookie_prefix;
		booster::function<std::string(std::string const &)> markdown2html;
		booster::function<std::string(std::string const &)> xss;
		std::string (*latex)(std::string const &);

		ulong counts;
		uint page_size;
		long page;
		long pages;

		void calc_pages() {
			pages = ceil((double) counts / (double) page_size);
			page = page < 0 ? 0 : (page >= pages ? pages - 1 : page);
		}

		std::string multi_page(std::string const &u);
		std::string multi_page(std::string const &u, int i);
	};

	void init_tex_filer(cppcms::json::value const &v);
}
