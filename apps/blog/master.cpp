#include <apps/blog/master.h>
#include <data/blog/master.h>

#include <cppcms/json.h>
#include <cppcms/session_interface.h>
#include <cppcms/cache_interface.h>
#include <cppdb/frontend.h>


namespace apps {
namespace blog {



master::master(cppcms::service &s) : basic_master(s)
{
}

master::~master()
{
}

namespace {
	struct sidebar_init {
		master *m;
		data::blog::master  *data;
		void operator()() const
		{
			m->load_sidebar(*data);
		}
	};
}

void master::load_sidebar(data::blog::master &c)
{
	if(cache().fetch_data("sidebar_info",c.sidebar))
		return;
	cppcms::triggers_recorder recorder(cache());
	
	cppdb::result r;

	r=sql()<<
		"SELECT	id,title "
		"FROM	pages "
		"WHERE	is_open=1";
	
	cache().add_trigger("pages");

	c.sidebar.pages.reserve(16);
	while(r.next()) {
		c.sidebar.pages.resize(c.sidebar.pages.size()+1);
		r >> c.sidebar.pages.back().id >> c.sidebar.pages.back().title;
	}
	
	r=sql()<<
		"SELECT a.id,a.name,COUNT(b.post_id) "
		"FROM	cats a LEFT JOIN post2cat b ON a.id=b.cat_id AND b.is_open=1 GROUP BY a.id";
	c.sidebar.cats.reserve(16);
	while(r.next()) {
		c.sidebar.cats.resize(c.sidebar.cats.size()+1);
		data::blog::sidebar_info::cat &cat = c.sidebar.cats.back();
		r >> cat.id >> cat.name >> cat.posts;
	}

	cache().add_trigger("cats");
	cache().add_trigger("general_info");

	cache().store_data("sidebar_info",c.sidebar,recorder.detach());
}

void master::prepare(data::blog::master &c)
{
	basic_master::prepare(c);
	sidebar_init ini = { this, &c };
	c.load_sidebar = ini;
}

void master::init()
{
	if(session().is_set("user")) {
		user_ = session().get("user");
	}
	else {
		user_.clear();
	}
}

void master::clear()
{
	user_.clear();
	basic_master::clear();
}


} // blog
} // apps
