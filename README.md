# cppblog
CppBlog is a high performance blog engine based on CppCMS technology

# SQL
The default encoding of the connection to your SQL server may not be UTF-8. To ensure that the data which is stored as UTF-8 in your database is retrieved appropriately, make sure to configure the connection to use UTF-8. With mysql, add 'set_charset_name=utf8' to your connection string, like this:
```
string connection_string("mysql:host=127.0.0.1;database=foo;user=bar;password=foobar;set_charset_name=utf8");  
cppdb::session session(connection_string);
```
