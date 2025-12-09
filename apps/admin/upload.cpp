#include <apps/admin/upload.h>
#include <data/admin/master.h>

#include <cppdb/frontend.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/crypto.h>
#include <cppcms/util.h>
#include <cppcms/http_file.h>

#include <booster/perl_regex.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <fstream>
#include <iostream>

#include <Magick++.h>

namespace apps {
namespace admin {

upload::upload(cppcms::service &s) : master(s)
{
	mapper().assign("");
	dispatcher().assign("/?",&upload::prepare,this);
}

void upload::prepare()
{
	bool is_post = request().request_method() == "POST";
	std::string md5 = post_or_get("md5");
	std::string name = post_or_get("name");
	size_t size = post_or_get("size", 0l);
	int rotation = post_or_get("rotation", 0) % 360;
	size_t chunk = post_or_get("chunk", 0l);
	size_t chunks = post_or_get("chunks", 1l);
	cppcms::json::value json;
	booster::regex rehex("^[0-9a-fA-F]+$");
	booster::regex reext("^.*\\.(gif|jpe?g|bmp|png|mp3|mp4|avi|flv|f4v|webm|ogv|bz2|xz|gz|tar|rar|zip|7z|cab|tgz|tbz|docx?|xlsx?|pptx?|pdf|csv|txt)$", booster::regex::perl|booster::regex::icase);

	if(md5.empty() || !booster::regex_match(md5, rehex) || name.empty() || size == 0) {
		json["status"].boolean(false);
		json["message"] = translate("Request parameter error.");
	} else if(!booster::regex_match(name, reext)) {
		json["status"].boolean(false);
		json["message"] = translate("File type is not supported.");
	} else {
		std::ostringstream bpath;
		std::ostringstream fpath;
		std::ostringstream furl;
		std::string docroot = settings().get("file_server.document_root", ".");
		std::string ext(name.substr(name.rfind(".")));
		data::admin::master c;
		struct stat st;
		bool exists = false;

		std::string::size_type pos = docroot.find_last_not_of('/');
		if(pos != std::string::npos) docroot.erase(pos + 1);

		master::prepare(c);

		bpath << c.media << "/upload/" << md5.substr(0, 2) << "/" << md5.substr(2,2) << "/" << md5.substr(4) << "-" << std::hex << size;
		fpath << docroot << bpath.str() << ext;
		furl << bpath.str() << ext;

		if(is_post) {
			std::ostringstream cpath;
			bool is_completed = true;

			cppcms::http::request::files_type files = request().files();

			if(files.size() != 1) {
				json["status"].boolean(false);
				json["message"] = translate("No file!");
				goto end;
			}

			std::unique_ptr<cppcms::crypto::message_digest> md5digest = cppcms::crypto::message_digest::md5();
			std::ifstream ifs;
			char buffer[2048];
			size_t n;

			if(chunks == 1) {
				mkdir(fpath.str(), true);

				files[0]->save_to(fpath.str());

				ifs.open(fpath.str().c_str());
				while(!ifs.eof()) {
					ifs.read(buffer, sizeof(buffer));
					n = ifs.gcount();
					md5digest->append(buffer, n);
				}
				ifs.close();
			} else {
				cpath << docroot << bpath.str() << "-" << chunk << ".part.lock";

				mkdir(cpath.str(), true);
				files[0]->save_to(cpath.str());
				{
					std::string _s = cpath.str();
					rename(cpath.str().c_str(), _s.substr(0, _s.size() - 5).c_str());
				}

				for(size_t i=0; i<chunks; i++) {
					cpath.str("");
					cpath << docroot << bpath.str() << "-" << i << ".part";

					if(access(cpath.str().c_str(), F_OK) != 0) {
						is_completed = false;
						break;
					}
				}

				if(is_completed) {
					std::ofstream of(fpath.str().c_str());

					for(size_t i=0; i<chunks; i++) {
						cpath.str("");
						cpath << docroot << bpath.str() << "-" << i << ".part";

						ifs.open(cpath.str().c_str());
						while(!ifs.eof()) {
							ifs.read(buffer, sizeof(buffer));
							n = ifs.gcount();
							of.write(buffer, n);
							of.flush();
							md5digest->append(buffer, n);
						}
						ifs.close();
						unlink(cpath.str().c_str());
					}

					of.close();
				}
			}

			if(is_completed) {
				md5digest->readout(buffer);
				std::string _md5 = tohex((unsigned char*) buffer, md5digest->digest_size());

				if(md5.compare(_md5) || lstat(fpath.str().c_str(), &st) || (size_t) st.st_size != size) {
					json["status"].boolean(false);
					json["message"] = translate("File hash value or size not is equals!");

					unlink(fpath.str().c_str());
					goto end;
				}

				exists = true;
				json["exists"].boolean(true);
				json["fileUrl"] = furl.str();
				json["fileName"] = name;
				json["fileSize"] = size;
				json["status"].boolean(true);
				json["message"] = translate("Upload completed!");
			} else {
				json["status"].boolean(true);
				json["message"] = translate("File block saved successfully!");
			}
		} else {
			exists = lstat(fpath.str().c_str(), &st) == 0 && (size_t) st.st_size == size;

			json["exists"].boolean(exists);
			json["fileUrl"] = furl.str();
			json["fileName"] = name;
			json["fileSize"] = size;
			json["status"].boolean(true);

			if(exists) {
				json["message"] = translate("Successful transmission in seconds!");
			} else {
				json["message"] = translate("Files do not exist and can be uploaded.");
			}
		}

		if(exists && rotation) {
			std::string spath = fpath.str();

			int r = rotation/90+3;
			if(r >= 4) {
				r -= 4;
			}
			fpath.str("");
			fpath << docroot << bpath.str() << "-r" << r << ext;
			furl.str("");
			furl << bpath.str() << "-r" << r << ext;

			if(access(fpath.str().c_str(), F_OK)) {
				std::ofstream f(fpath.str().c_str());
				Magick::Image image(spath);
				Magick::Blob b;

				image.rotate(rotation);
				image.quality(90);
				image.write(&b, ext.substr(1));

				f.write((char*)b.data(), b.length());
				f.close();
			}

			json["fileUrl"] = furl.str();
		}
	}

	end:

	response().set_content_header("application/json; charset=UTF-8");
	response().out() << json;
}

} // admin
} // apps
