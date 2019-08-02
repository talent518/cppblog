#include <apps/admin/users.h>
#include <data/admin/users.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppdb/frontend.h>
#include <cppcms/session_interface.h>
#include <cppcms/http_response.h>

namespace apps {
namespace admin {

users::users(cppcms::service &s) : master(s)
{
	mapper().assign("list", "/list");
	dispatcher().assign("/list/?",&users::list,this);

	mapper().assign("list", "/list/{1}");
	dispatcher().assign("/list/(\\d+)/?",&users::list,this,1);

	mapper().assign("add","/add");
	dispatcher().assign("/add/?",&users::add,this);

	mapper().assign("del", "/del/{1}");
	dispatcher().assign("/del/(\\d+)/?",&users::del,this,1);

	mapper().assign("/{1}");
	dispatcher().assign("/(\\d+)/?",&users::edit,this,1);

	mapper().assign("");
	dispatcher().assign("/?",&users::prepare,this);
}

void users::add()
{
	data::admin::user c;

	if(request().request_method()=="POST") {
		c.user.load(context());
		if(c.user.validate()) {
			cppdb::transaction tr(sql());
			sql()<<"INSERT INTO users(username,password) values(?,?)" 
				<< c.user.username.value() << c.user.password.value() << cppdb::exec;
			tr.commit();
			response().set_redirect_header(url("/admin/users/list"));
			return;
		}
	}
	master::prepare(c);
	render("admin_skin","useradd",c);
}

void users::list()
{
	list(0);
}

void users::list(std::string page)
{
	list(atoi(page.c_str()));
}

void users::list(int page)
{
	static const int page_size = 5;
	cppdb::result r;
	data::admin::userlist c;

	r = sql() <<
		"SELECT id, username, password "
		"FROM users "
		"ORDER BY id ASC "
		"LIMIT ? OFFSET ? " 
		<< (page_size + 1) << (page * page_size);

	c.next_page = page + 1;
	c.page = page;
	c.prev_page = page > 0 ? page - 1 : 0;

	c.users.reserve(page_size);

	int current_pos = 0;
	while(r.next()) {
		if(current_pos >= page_size) {
			c.next_page = page + 1;
			break;
		}
		c.users.resize(current_pos + 1);
		data::admin::user_content &cur = c.users.back();
		r >> cur.id >> cur.username >> cur.password;
		current_pos ++;
	}

	master::prepare(c);

	render("admin_skin","userlist",c);
}

void users::edit(std::string sid)
{
	int id = atoi(sid.c_str());
	if(id==0) {
		response().make_error_response(404);
		return;
	}

	bool is_post = request().request_method()=="POST";

	data::admin::user c;

	c.id = id;
	cppdb::result r;
	if(is_post) {
		r = sql() << 
			"SELECT id "
			"FROM users "
			"WHERE id=?" << id << cppdb::row;
	} else {
		r = sql() << 
			"SELECT username, password "
			"FROM users "
			"WHERE id=?" << id << cppdb::row;
	}
	if(r.empty()) {
		response().make_error_response(404);
		return;
	}
	if(is_post) {
		c.user.load(context());
		if(c.user.validate()) {
			cppdb::transaction tr(sql());
			sql()<<"UPDATE users SET username=?,password=? WHERE id=?" 
				<< c.user.username.value() << c.user.password.value() << id << cppdb::exec;
			tr.commit();
			response().set_redirect_header(url("/admin/users/list"));
			return;
		}
	} else {
		c.user.username.value(r.get<std::string>(0));
		c.user.password.value(r.get<std::string>(1));
	}
	master::prepare(c);
	render("admin_skin","user",c);
}

void users::del(std::string sid)
{
	int id = atoi(sid.c_str());
	if(id==0 || id == session().get<int>("id")) {
		response().make_error_response(404);
		return;
	}

	cppdb::transaction tr(sql());
	sql()<<"DELETE FROM users WHERE id=?" << id << cppdb::exec;
	tr.commit();

	response().set_redirect_header(url("/admin/users/list"));
}

void users::prepare()
{
	data::admin::users c;

	session().reset_session();
	session().clear();

	if(request().request_method()=="POST") {
		c.login.load(context());
		if(!c.login.validate()) {
			c.login.password.clear();
		} else {
			cppdb::result r;
			r = sql() << 
				"SELECT id,username,password "
				"FROM users "
				"WHERE username = ?" 
				<< c.login.username.value() << cppdb::row;
			if(!r.empty()) {
				int id;
				std::string user;
				std::string pass;
				r >> id >> user >> pass;
				if(c.login.username.value() == user && c.login.password.value() == pass) {
					session().set("user",user);
					session().set<int>("id",id);
					session().expose("user");
					response().set_redirect_header(url("/admin/summary"));
					return;
				}
			}
			c.login.password.clear();
			c.login.password.valid(false);
			c.login.username.valid(false);
		}
	}
	master::prepare(c);
	render("admin_skin","users",c);
}


} // admin
} // apps
