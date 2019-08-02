#pragma once 
#include <data/basic_master.h>

#include <cstdlib>
#include <ctime>

namespace data {
	namespace admin {
		class tinymce: public cppcms::widgets::textarea {
		private:
			std::string mode_;
		public:
			tinymce() : cppcms::widgets::textarea(),mode_("basic") {}
			virtual void render_attributes(cppcms::form_context &context) {
				context.out() << "tinymce=\"" << mode_ << "\" ";
				cppcms::widgets::textarea::render_attributes(context);
			}

			std::string mode() { return mode_;}
			void mode(std::string m) {mode_ = m;}
		};

		enum {
			NONE=0,
			CHINESE,
			ENGLISH
		};

		struct master : public basic_master {
			int nlang;
			int loginId;
			std::string loginUser;
		};
	}
}
