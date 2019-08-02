#include <data/basic_master.h>
#include <string>

namespace data {
	std::string basic_master::multi_page(std::string const &u) {

		if(pages <= 1) return "";

		int b = page - 5, e = page + 5;
		if(b < 0) b = 0;
		if(e >= pages) e = pages -1;

		std::ostringstream ss;

		ss << "<div class=\"multi-page\">" << std::endl;

		ss << "<span class=\"desc\">" << cppcms::locale::format(app().translate("Total {1} pages, total {2} records.")) % (cppcms::filters::escape(pages)) % (cppcms::filters::escape(counts)) << "</span>" << std::endl;

		for(;b<=e;b++) {
			if(b == page)
				ss << "<span class=\"page cur\">" << b+1 << "</span>" << std::endl;
			else
				ss << "<a href=\"" << app().url(u, b) << "\" class=\"page\">" << b+1 << "</a>" << std::endl;
		}

		ss << "</div>";

		return ss.str();
	}

	std::string basic_master::multi_page(std::string const &u, int i) {
		pages = ceil((double) counts / (double) page_size);

		if(pages <= 1) return "";

		page = page < 0 ? 0 : (page >= pages ? pages - 1 : page);

		int b = page - 5, e = page + 5;
		if(b < 0) b = 0;
		if(e >= pages) e = pages -1;

		std::ostringstream ss;

		ss << "<div class=\"multi-page\">" << std::endl;

		ss << "<span class=\"desc\">" << cppcms::locale::format(app().translate("Total {1} pages, total {2} records.")) % (cppcms::filters::escape(pages)) % (cppcms::filters::escape(counts)) << "</span>" << std::endl;

		for(;b<=e;b++) {
			if(b == page)
				ss << "<span class=\"page cur\">" << b+1 << "</span>" << std::endl;
			else
				ss << "<a href=\"" << app().url(u, i, b) << "\" class=\"page\">" << b+1 << "</a>" << std::endl;
		}

		ss << "</div>";

		return ss.str();
	}
}
