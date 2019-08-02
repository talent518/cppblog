#pragma once

#include <data/admin/master.h>

namespace data {
	namespace admin {
		struct user_login : public master {
			struct login_form : public cppcms::form {
				cppcms::widgets::text username;
				cppcms::widgets::password password;
				cppcms::widgets::submit login;

				login_form()
				{
					username.non_empty();
					username.message(booster::locale::translate("Username"));
					password.non_empty();
					password.message(booster::locale::translate("Password"));
					login.value(booster::locale::translate("Login"));

					add(username);
					add(password);
					add(login);
				}
			};

			login_form login;
		};

		struct user_one : public master {
			struct user_form : public cppcms::form {
				cppcms::widgets::text username;
				cppcms::widgets::text password;
				cppcms::widgets::submit save;

				user_form()
				{
					username.non_empty();
					username.message(booster::locale::translate("Username"));
					password.non_empty();
					password.message(booster::locale::translate("Password"));
					save.value(booster::locale::translate("Save"));

					add(username);
					add(password);
					add(save);
				}
			};

			user_form user;

			int id;
		};

		struct user_content {
			int id;
			std::string username;
			std::string password;
			user_content() : id(0) {
			}
		};

		struct userlist : public master {
			std::vector<user_content> users;
		};
	}
}
