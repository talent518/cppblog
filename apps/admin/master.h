#pragma once
#include <apps/basic_master.h>

#include <data/admin/master.h>

namespace apps {
	namespace admin {
		class master : public basic_master {
		public:
			master(cppcms::service &s) : basic_master(s) {
			}
		protected:
			void prepare(data::admin::master &c) {
				basic_master::prepare(c);

				if(session().is_set("user")) {
					c.loginId = session().get<int>("id");
					c.loginUser = session().get("user");
				} else {
					c.loginId = 0;
					c.loginUser = "";
				}
			}
		};
	}
};
