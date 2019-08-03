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

		int page_records;
		int page_size;
		int curpage;
		int page_total;

		basic_master(): latex(NULL), page_records(0), page_size(0), curpage(0), page_total(0) {}

		void calc_pages() {
			page_total = ceil((double) page_records / (double) page_size);
			curpage = curpage < 0 ? 0 : (curpage >= page_total ? page_total - 1 : curpage);
		}

		std::string multi_page(std::string const &u);
		std::string multi_page(std::string const &u, int i);
	};

	void init_tex_filer(cppcms::json::value const &v);
	void str_replace(std::string & str, const std::string & strsrc, const std::string &strdst);
	void mysql_like(std::string &str);
}
