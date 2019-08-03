#include <data/basic_master.h>
#include <string>


namespace data {
	#define MULTI_PAGE(url) \
		if(pages <= 1) return ""; \
		\
		int b = page - 5, e; \
		std::ostringstream ss; \
		if(b < 0) b = 0; \
		\
		e = b + 9; \
		if(e >= pages) e = pages -1; \
		\
		ss << "<div class=\"multi-page\">" << std::endl; \
		\
		ss << "<span class=\"desc\">" << cppcms::locale::format(app().translate("Total {1} pages, total {2} records.")) % (cppcms::filters::escape(pages)) % (cppcms::filters::escape(counts)) << "</span>" << std::endl; \
		\
		for(;b<=e;b++) { \
			if(b == page) \
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
}
