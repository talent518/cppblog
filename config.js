{
	
	"blog" : {
		"media" : "/media",
		"root" : "",
		"host" : "localhost:8080",
		//"connection_string" : "sqlite3:db=cppcms.db;@pool_size=10",
		"connection_string" : "mysql:database=cppblog;user=root;password=1qazXSW23edc;unix_socket=/opt/lampp/var/mysql/mysql.sock;set_charset_name=utf8;@pool_size=10;@use_prepared=on",
		"tex" : {
			"enable" : true,
			//"latex" : "/usr/bin/latex",
			// "dvigif" : "/usr/bin/dvigif",
			// "temp_dir" : "/tmp",
			// "output_dir" : "/var/www/media/tex"
		}
	},
	"service" : {
		"api" : "http",
		//"port" : 8080
		//"api" : "fastcgi",
		//"api" : "scgi",
		//"socket" : "/tmp/sock"
	},
	"http" : {
		"script" : "/mb.fcgi",
		"rewrite" : [
			{ "regex" : "/media(/.*)?", "pattern" : "$0" },
			{ "regex" : ".*" , "pattern" : "/mb.fcgi$0" }
		]
		
	},
	"views" : {
		"default_skin" : "contendend" ,
		//"default_skin" : "orangesky" ,
		"paths" : [ "./" ],
		"skins" : [ "contendend" , "orangesky" ],
	},
	"file_server" : {
		"enable" : false,
		"document_root" : "../",
		"allow_deflate" : true
	},
	"session" : {
		"expire" : "renew",
		"timeout" : 604800, // a week
		"location" : "both",
		"client" : {
			"hmac" : "sha1",
			"hmac_key" : "232074faa0fd37de20858bf8cd0a7d10"
		},
		"server" : {
			"storage" : "files"
		}
	},
	"cache" : {
		"backend" : "thread_shared",
		"limit" : 100,
	},
	"localization" : {
		"messages" : {
			"paths" : [ "./locale" ],
			"domains" : [ "cppblog" ]
		},
		"locales" : [ 
			"zh_CN.UTF-8",
			"en_US.UTF-8",
		]
	},
	"security" : {
		"csrf" : {
			"enable" : true,
			"exposed": true
		}
	}
}
