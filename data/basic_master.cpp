#include <data/basic_master.h>
#include <string>


namespace data {
	#define MULTI_PAGE_LINK(url,cls,page) \
		ss << "<a href=\"" << (url); \
		std::string s = app().request().query_string(); \
		if(!s.empty()) ss << "?" << s; \
		ss << "\" class=\"" << cls << "\">" << page << "</a>" << std::endl

	#define MULTI_PAGE(args...) \
		std::ostringstream ss; \
		ss << "<div class=\"multi-page\">" << std::endl; \
		\
		ss << "<span class=\"desc\">" << cppcms::locale::format(app().translate("Total {1} pages, {2} records.")) % (cppcms::filters::escape(page_total)) % (cppcms::filters::escape(page_records)) << "</span>" << std::endl; \
		\
		if(page_total > 1) { \
			int b = curpage - 5, e; \
			if(b < 0) b = 0; \
			\
			e = b + 9; \
			if(e >= page_total) e = page_total - 1; \
			\
			if(b > 0) { \
				MULTI_PAGE_LINK(app().url(args), "page first", "&laquo;"); \
			} \
			\
			for(;b<=e;b++) { \
				if(b == curpage) \
					ss << "<span class=\"page cur\">" << b+1 << "</span>" << std::endl; \
				else { \
					MULTI_PAGE_LINK(b == 0 ? app().url(args) : app().url(args, b), "page", b+1); \
				} \
			} \
			\
			if(e < page_total -1) { \
				MULTI_PAGE_LINK(app().url(args, page_total - 1), "page last", "&raquo;"); \
			} \
		} else { \
			ss << "<span class=\"page cur\">1</span>" << std::endl; \
		} \
		\
		ss << "</div>"; \
		\
		return ss.str()

	std::string basic_master::multi_page(std::string const &u) {
		// std::cout << u << std::endl;
		MULTI_PAGE(u);
	}

	std::string basic_master::multi_page(std::string const &u, int i) {
		// std::cout << u << " - " << i << std::endl;
		MULTI_PAGE(u, i);
	}

	void str_replace(std::string & str, const std::string & strsrc, const std::string &strdst) {
		std::string::size_type pos = 0; //位置
		std::string::size_type srclen = strsrc.size(); //要替换的字符串大小
		std::string::size_type dstlen = strdst.size(); //目标字符串大小
		while ((pos = str.find(strsrc, pos)) != std::string::npos) {
			str.replace(pos, srclen, strdst);
			pos += dstlen;
		}
	}

	void mysql_like(std::string &str) {
		str_replace(str, "\\", "\\\\");
		str_replace(str, "_", "\\_");
		str_replace(str, "|", "\\|");
		str_replace(str, "%", "\\%");
	}

}
