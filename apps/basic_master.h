#pragma once
#include <cppcms/application.h>

namespace cppdb {
	class session;
}
namespace data {
	struct basic_master;
}
namespace apps {
	class database_is_not_configured_error : public std::runtime_error {
	public:
		database_is_not_configured_error() : 
			std::runtime_error("The database was not configured")
		{
		}
	};

	class database_version_error : public std::runtime_error {
	public:
		database_version_error() :
			std::runtime_error("Invalid database version")
		{
		}
	};


	class basic_master : public cppcms::application {
	public:
		basic_master(cppcms::service &s);
		~basic_master();
		
		virtual void clear();
	protected:
		std::string host_;
		cppdb::session &sql();
		void prepare(data::basic_master &);

		std::string get(const char *key);
		std::string get(const char *key, const char *def);
		int get(const char *key, int def);
		long int get(const char *key, long int def);

		std::string post(const char *key);
		std::string post(const char *key, const char *def);
		int post(const char *key, int def);
		long int post(const char *key, long int def);

		std::string post_or_get(const char *key);
		std::string post_or_get(const char *key, const char *def);
		int post_or_get(const char *key, int def);
		long int post_or_get(const char *key, long int def);

		bool mkdir(std::string path, bool isfile=false, int mode=0755);

		std::string tohex(const unsigned char *str, size_t len);
	private:
		std::auto_ptr<cppdb::session> sql_;
		std::string conn_str_;
		std::string media_;
		std::string cookie_;
	};
	
	void init_tex_filer(cppcms::json::value const &s);
}

