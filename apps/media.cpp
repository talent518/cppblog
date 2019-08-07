#include <apps/media.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/json.h>
#include <cppcms/util.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <fstream>
#include <iostream>

namespace apps {

media::media(cppcms::service &s) : cppcms::application(s)
{
	mapper().assign("/{1}");
	dispatcher().assign("/(.+)/?",&media::prepare,this,1);

	std::string str = settings().get("file_server.document_root", ".");
	std::string::size_type pos = str.find_last_not_of('/');
	if(pos != std::string::npos) str.erase(pos + 1);

	document_root_ = str + "/media";

	mime_[".swf"]   =      "application/x-shockwave-flash";

	// video
	mime_[".mp4"]   =      "video/mp4";
	mime_[".mpeg"]  =      "video/mpeg";
	mime_[".mpg"]   =      "video/mpeg";
	mime_[".mov"]   =      "video/quicktime";
	mime_[".qt"]    =      "video/quicktime";
	mime_[".avi"]   =      "video/x-msvideo";
	mime_[".asf"]   =      "video/x-ms-asf";
	mime_[".asx"]   =      "video/x-ms-asf";
	mime_[".wmv"]   =      "video/x-ms-wmv";
	mime_[".flv"]   =      "video/x-flv";
	mime_[".f4v"]   =      "video/x-f4v";
	mime_[".webm"]  =      "video/webm";
	mime_[".ogv"]   =      "video/ogg";

	// image file
	mime_[".gif"]   =      "image/gif";
	mime_[".jpg"]   =      "image/jpeg";
	mime_[".jpeg"]  =      "image/jpeg";
	mime_[".png"]   =      "image/png";
	mime_[".bmp"]   =      "image/bmp";
	mime_[".svg"]   =      "image/svg+xml";
	mime_[".svgz"]  =      "image/svg+xml";
	mime_[".ico"]   =      "image/x-icon";

	// text file
	mime_[".js"]    =      "text/javascript";
	mime_[".css"]   =      "text/css";
	mime_[".html"]  =      "text/html";
	mime_[".htm"]   =      "text/html";
	mime_[".txt"]   =      "text/plain";
	mime_[".csv"]   =      "text/csv";

	// Compressed file format
	mime_[".gz"]    =      "application/x-gzip";
	mime_[".tgz"]   =      "application/x-tgz";
	mime_[".tar"]   =      "application/x-tar";
	mime_[".zip"]   =      "application/zip";
	mime_[".bz2"]   =      "application/x-bzip";
	mime_[".tbz"]   =      "application/x-bzip-compressed-tar";
	mime_[".rar"]   =      "application/x-rar-compressed";
	mime_[".7z"]    =      "application/x-7z-compressed";
	mime_[".cab"]   =      "application/vnd.ms-cab-compressed";
	mime_[".xz"]    =      "application/x-xz";

	// office file
	mime_[".doc"]   =      "application/msword";
	mime_[".docx"]  =      "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime_[".xls"]   =      "application/vnd.ms-excel";
	mime_[".xlsx"]  =      "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_[".ppt"]   =      "application/vnd.ms-powerpoint";
	mime_[".pptx"]  =      "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_[".pdf"]   =      "application/pdf";
}

void media::prepare(std::string path)
{
	if(path.find("..") != std::string::npos) {
		response().make_error_response(404);
		return;
	}

	std::ostringstream ss;

	ss << document_root_ << "/" << path;

	std::string ext;
	size_t pos = path.rfind('.');
	if(pos != std::string::npos)
		ext=path.substr(pos);

	mime_type::const_iterator p=mime_.find(ext);
	if(p!=mime_.end()) 
		response().content_type(p->second);
	else
		response().content_type("application/octet-stream");

	struct stat st;
	if(lstat(ss.str().c_str(), &st))
		response().make_error_response(404);
	else {
		std::ostringstream orange;
		size_t b = 0, e = st.st_size - 1;

		if(!request().http_range().empty()) {
			sscanf(request().http_range().c_str(), "bytes=%lu-%lu", &b, &e);
			orange << "bytes " << b << "-" << e << "/" << st.st_size;
			response().content_range(orange.str());
		}

		size_t n = e-b+1;
		time_t t = time(NULL);

		response().content_length(n);
		response().accept_ranges("bytes");
		response().cache_control("max-age=604800");
		response().date(t);
		response().age(86400);
		response().last_modified(st.st_mtime);
		response().expires(t + 604800);
		response().vary("Accept-Encoding");

		std::ostringstream etag;
		etag << std::hex << st.st_size << "-" << std::hex << st.st_mtime;
		response().etag(etag.str());

		//response().io_mode(cppcms::http::response::asynchronous);
		if(n != st.st_size) response().status(206);

		std::ifstream f(ss.str().c_str());
		f.seekg(b);
		response().out() << f.rdbuf();
	}
}

} // apps
