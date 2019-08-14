#include <apps/admin/summary.h>
#include <data/admin/summary.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppdb/frontend.h>

namespace apps {
namespace admin {

summary::summary(cppcms::service &s) : master(s)
{
	mapper().assign("/{1}");
	dispatcher().assign("/(\\d+)/?",&summary::prepare,this,1);

	mapper().assign("");
	dispatcher().assign("/?",&summary::prepare,this);
}

void summary::prepare()
{
	prepare(0);
}

void summary::prepare(std::string page)
{
	prepare(atoi(page.c_str()));
}

void summary::prepare(int page)
{
	data::admin::summary c;
	master::prepare(c);
	cppdb::result r;

	c.active_index = true;

	// posts
	r= sql() << 
		"SELECT id,title "
		"FROM posts "
		"WHERE is_open = 0 ORDER BY id DESC";
	c.posts.reserve(10);
	while(r.next()) {
		c.posts.push_back(data::admin::summary::post());
		data::admin::summary::post &last = c.posts.back();
		r >> last.id >> last.title;
	}

	// posts published
	c.page_size = 10;
	c.curpage = page;
	sql() << "SELECT COUNT(id) FROM posts WHERE is_open = 1" << cppdb::row >> c.page_records;
	c.calc_pages();

	r= sql() << "SELECT id,title FROM posts WHERE is_open = 1 ORDER BY id DESC LIMIT ? OFFSET ?" << c.page_size << c.curpage * c.page_size;
	c.publisheds.reserve(c.page_size);
	while(r.next()) {
		c.publisheds.push_back(data::admin::summary::post());
		data::admin::summary::post &last = c.publisheds.back();
		r >> last.id >> last.title;
	}

	// pages
	c.pages.reserve(10);
	r = sql() <<"SELECT id,title,is_open FROM pages";
	while(r.next()) {
		c.pages.push_back(data::admin::summary::page());
		data::admin::summary::page &last = c.pages.back();
		r >> last.id >> last.title >> last.is_open;
	}

	// comments
	r = sql() <<                
		"SELECT id,post_id,author "
		"FROM comments "
		"ORDER BY id DESC "
		"LIMIT 10";
	c.comments.reserve(10);
	while(r.next()) {
		c.comments.push_back(data::admin::summary::comment());
		data::admin::summary::comment &last = c.comments.back();
		r >> last.id >> last.post_id >> last.username;
	}

	render("admin_skin","summary",c);

}


} // admin
} // apps
