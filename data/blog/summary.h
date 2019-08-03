#pragma once

#include <data/blog/post.h>

namespace data {
namespace blog {
	struct summary : public master {
		std::string category_name;
		std::string q;
		int id;
		typedef std::vector<post_content> posts_type;
		posts_type posts;
	};

}
}
