#pragma once

#include <data/blog/post.h>

namespace data {
namespace blog {
	struct summary : public master {
		std::string category_name;
		int id;
		typedef std::vector<post_content> posts_type;
		posts_type posts;
	};

}
}
