#pragma once
#include <apps/blog/master.h>

namespace apps {
	namespace blog {
		class download :public master {
		public:
			download(cppcms::service &s);

		private:
			void prepare();
			void prepare(std::string md5, std::string size, std::string name);
		};
	}
}
