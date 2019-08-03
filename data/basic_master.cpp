#include <data/basic_master.h>
#include <string>


namespace data {
	#define MULTI_PAGE(url) \
		if(page_total <= 1) return ""; \
		\
		int b = curpage - 5, e; \
		std::ostringstream ss; \
		if(b < 0) b = 0; \
		\
		e = b + 9; \
		if(e >= page_total) e = page_total -1; \
		\
		ss << "<div class=\"multi-page\">" << std::endl; \
		\
		ss << "<span class=\"desc\">" << cppcms::locale::format(app().translate("Total {1} pages, {2} records.")) % (cppcms::filters::escape(page_total)) % (cppcms::filters::escape(page_records)) << "</span>" << std::endl; \
		\
		for(;b<=e;b++) { \
			if(b == curpage) \
				ss << "<span class=\"page cur\">" << b+1 << "</span>" << std::endl; \
			else \
				ss << "<a href=\"" << url << "\" class=\"page\">" << b+1 << "</a>" << std::endl; \
		} \
		\
		ss << "</div>"; \
		\
		return ss.str()

	std::string basic_master::multi_page(std::string const &u) {
		MULTI_PAGE(app().url(u, b));
	}

	std::string basic_master::multi_page(std::string const &u, int i) {
		MULTI_PAGE(app().url(u, i, b));
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
