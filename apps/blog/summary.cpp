#include <apps/blog/summary.h>
#include <data/blog/summary.h>

#include <cppdb/frontend.h>
#include <cppcms/http_response.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/cache_interface.h>

namespace apps {
namespace blog {

summary::summary(cppcms::service &s) : master(s)
{
	mapper().assign("/");
	dispatcher().assign("/?",&summary::all,this);

	mapper().assign("from", "/from/{1}");
	dispatcher().assign("/from/(\\d+)/?",&summary::all,this,1);

	mapper().assign("category","/cat/{1}");
	dispatcher().assign("/cat/(\\d+)/?",&summary::category,this,1);

	mapper().assign("category","/cat/{1}/from/{2}");
	dispatcher().assign("/cat/(\\d+)/from/(\\d+)/?",&summary::category,this,1,2);
}

void summary::all()
{
	prepare(0,0);
}

void summary::all(std::string page)
{
	prepare(0,atoi(page.c_str()));
}

void summary::category(std::string id)
{
	prepare(atoi(id.c_str()),0);
}

void summary::category(std::string id,std::string page)
{
	prepare(atoi(id.c_str()),atoi(page.c_str()));
}

void summary::prepare(int cat_id,int page)
{
	cppdb::result r;
	std::string q = request().get("q");

	std::ostringstream key;
	key << "summary_" << cat_id <<"_"<<page<<"_"<<q;
	if(cache().fetch_page(key.str()))
		return;
	std::ostringstream ss;
	ss << "cat_" << cat_id;
	cache().add_trigger(ss.str());
	
	data::blog::summary c;
	
	c.q = q;
	c.page_size = 10;
	c.curpage = page;

	if(!q.empty()) {
		data::mysql_like(q);

		q = "%" + q + "%";
	}

	if(cat_id != 0) {
		r = sql() << "SELECT name FROM cats WHERE id=?" << cat_id << cppdb::row;
		if(r.empty()) {
			response().make_error_response(404);
			return;
		}
		r >> c.category_name;

		if(q.empty()) {
			sql() << "SELECT COUNT(post_id) FROM post2cat WHERE cat_id=? AND is_open=1" << cat_id << cppdb::row >> c.page_records;

			c.calc_pages();
			c.id = cat_id;

			r = sql() <<
				"SELECT posts.id,users.username,posts.title, "
				"	posts.abstract, length(posts.content), "
				"	posts.publish,posts.comment_count "
				"FROM	post2cat "
				"LEFT JOIN	posts ON post2cat.post_id=posts.id "
				"LEFT JOIN	users ON users.id=posts.author_id "
				"WHERE	post2cat.cat_id=? "
				"	AND post2cat.is_open=1 "
				"ORDER BY post2cat.publish DESC "
				"LIMIT ? OFFSET ?"
				<< cat_id << c.page_size << (c.curpage * c.page_size);
		} else {
			sql() << "SELECT COUNT(post_id) FROM post2cat LEFT JOIN	posts ON post2cat.post_id=posts.id WHERE post2cat.cat_id=? AND post2cat.is_open=1 AND (posts.title LIKE ? OR posts.abstract LIKE ? OR posts.content LIKE ?)" << cat_id << q << q << q << cppdb::row >> c.page_records;

			c.calc_pages();
			c.id = cat_id;

			r = sql() <<
				"SELECT posts.id,users.username,posts.title, "
				"	posts.abstract, length(posts.content), "
				"	posts.publish,posts.comment_count "
				"FROM	post2cat "
				"LEFT JOIN	posts ON post2cat.post_id=posts.id "
				"LEFT JOIN	users ON users.id=posts.author_id "
				"WHERE	post2cat.cat_id=? "
				"	AND post2cat.is_open=1 AND (posts.title LIKE ? OR posts.abstract LIKE ? OR posts.content LIKE ?) "
				"ORDER BY post2cat.publish DESC "
				"LIMIT ? OFFSET ?"
				<< cat_id << q << q << q << c.page_size << (c.curpage * c.page_size);
		}
	} else if(q.empty()) {
		sql() << "SELECT COUNT(id) FROM posts WHERE is_open=1" << cppdb::row >> c.page_records;

		c.calc_pages();
		c.id = 0;

		r = sql() <<
			"SELECT posts.id,users.username,posts.title, "
			"	posts.abstract, length(posts.content), "
			"	posts.publish,posts.comment_count "
			"FROM	posts "
			"LEFT JOIN "
			"	users ON users.id=posts.author_id "
			"WHERE	posts.is_open=1 "
			"ORDER BY posts.publish DESC "
			"LIMIT ? OFFSET ? " 
			<< c.page_size << (c.curpage * c.page_size);
	} else {
		sql() << "SELECT COUNT(id) FROM posts WHERE is_open=1 AND (title LIKE ? OR abstract LIKE ? OR content LIKE ?)" << q << q << q << cppdb::row >> c.page_records;

		c.calc_pages();
		c.id = 0;

		r = sql() <<
			"SELECT posts.id,users.username,posts.title, "
			"	posts.abstract, length(posts.content), "
			"	posts.publish,posts.comment_count "
			"FROM	posts "
			"LEFT JOIN "
			"	users ON users.id=posts.author_id "
			"WHERE	posts.is_open=1 AND (title LIKE ? OR abstract LIKE ? OR content LIKE ?) "
			"ORDER BY posts.publish DESC "
			"LIMIT ? OFFSET ? "
			 << q << q << q << c.page_size << (c.curpage * c.page_size);
	}

	c.posts.reserve(c.page_size);

	int current_pos = 0;
	while(r.next()) {
		current_pos ++;
		c.posts.resize(current_pos);
		data::blog::post_content &cur = c.posts.back();
		int content_size = 0;
		std::tm published=std::tm();
		r 	>> cur.id >> cur.author >> cur.title >> cur.abstract >> content_size 
			>> published >> cur.comment_count;
		cur.has_content = content_size != 0;
		cur.published = mktime(&published);
		std::ostringstream ss;
		ss << "post_" << cur.id;
		cache().add_trigger(ss.str());
	}

	master::prepare(c);

	if(!c.posts.empty()) {
		std::string in_close;
		in_close.reserve(c.posts.size() * 2);
		for(unsigned i=0;i<c.posts.size();i++)  {
			if(i == 0)
				in_close+="?";
			else
				in_close+=",?";
		}
		cppdb::statement st = sql() << 
			"SELECT   post_id, cat_id,cats.name "
			"FROM     post2cat "
			"JOIN     cats ON cats.id = post2cat.cat_id "
			"WHERE    post_id in (" + in_close + ") "
			"ORDER BY post_id";
		std::map<int,int> id2pos;
		for(unsigned i=0;i<c.posts.size();i++) {
			int post_id = c.posts[i].id;
			st.bind(post_id);
			id2pos[post_id] = i;
		}
		r = st.query();
		while(r.next()) {
			int post_id;
			data::blog::category cat;
			r  >> post_id >> cat.id >> cat.name;
			c.posts.at(id2pos[post_id]).categories.push_back(cat);
		}
	}

	render("summary",c);

	cache().store_page(key.str());
}



} // 
} // 
