#pragma once
#include <apps/admin/master.h>

namespace apps {
	namespace admin {
		class users : public master {
		public:
			users(cppcms::service &s);
			void prepare();
			void add();
			void list();
			void list(std::string page);
			void list(int page=0);
			void edit(std::string sid);
			void del(std::string sid);
		};
	}
}
