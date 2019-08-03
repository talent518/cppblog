#pragma once
#include <apps/admin/master.h>

namespace apps {
	namespace admin {
		class upload :public master {
		public:
			upload(cppcms::service &s);

		private:
			void prepare();
		};
	}
}

