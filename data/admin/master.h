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

		struct master : public basic_master {
			bool active_index;
			bool active_write_post;
			bool active_write_page;
			bool active_category;
			bool active_users;
			bool active_options;
			bool active_caching;
			int loginId;
			std::string loginUser;
			master(): active_index(false),active_write_post(false),active_write_page(false),active_category(false),active_users(false),active_options(false),active_caching(false),loginId(0){}
		};
	}
}
