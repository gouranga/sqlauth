# sqlauth for [3proxy](https://github.com/z3APA3A/3proxy/)

Untested alfa version, use at your own risk.

## Requirements
- *nix, gcc with c99 support
- mysql, postgre or sqlite database with *text* usernames and passwords
- [libzdb-dev](http://www.tildeslash.com/libzdb/)

## How to compile

### Debian / Ubuntu / deb-based
```sh
sudo apt-get install libzdb-dev
git clone https://github.com/z3APA3A/3proxy.git
cd 3proxy/src/plugins
git clone https://github.com/gouranga/sqlauth.git
cd ../../
cp Makefile.Linux Makefile.Linux.bak
sed '/^PLUGINS =/ s/$/ sqlauth/' Makefile.Linux.bak > Makefile.Linux
make -f Makefile.Linux
```

## How to use

1. Load plugin in config
```
plugin "/path/to/sqlauth.ld.so" load [debug]
```

`debug` parameter is optional

2. Setup SQL options
```
sqlopts max_connections INT
sqlopts connection_timeout INT
sqlopts sweep_interval INT
sqlopts dsn "<protocol>://<authority><path>?<query>"
sqlopts query "SELECT `password` FROM `auth` WHERE `username` = ? LIMIT 1"
```

 - `max_connections` [max connections](http://www.tildeslash.com/libzdb/api-docs/ConnectionPool_8h.html#a68ac953c1bc1915b3ac6ae0b7b4c64a4) within the pool
 - `connection_timeout` [connection inactive timeout](http://www.tildeslash.com/libzdb/api-docs/ConnectionPool_8h.html#a143b4171cc03227b1bca5cd10e724bc5) in seconds
 - `sweep_interval` number of seconds between sweeps of the [reaper thread](http://www.tildeslash.com/libzdb/api-docs/ConnectionPool_8h.html#a756268df598fb93caeef570b775e6768)
 - `dsn` data source name in [libzdb format](http://www.tildeslash.com/libzdb/api-docs/URL_8h.html)
 - `query` prepared select statement with 1 parameter (username) and 1 result column with password

For example, with MySQL table `auth` in database `users`:

```sql
CREATE TABLE `auth` (
	`id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
	`username` VARCHAR(255) NULL DEFAULT NULL,
	`password` VARCHAR(255) NULL DEFAULT NULL,
	PRIMARY KEY (`id`),
	UNIQUE INDEX `username` (`username`)
)
COLLATE='utf8_general_ci' ENGINE=InnoDB;
```

You can setup like this
```
sqlopts max_connections 25
sqlopts connection_timeout 10
sqlopts sweep_interval 10
sqlopts dsn "mysql://localhost:3306/users?charset=utf8&user=USERNAME&password=PASSWORD"
sqlopts query "SELECT `password` FROM `auth` WHERE `username` = ? LIMIT 1"
```

3. Start the pool
```
sqlconnect
```

3. Set up authcache and method.
```
authcache user,password 300
auth cache strong sqlauth
```

If you don't change passwords frequently you can setup higher cache time.

4. Run 3proxy and enjoy!

## Security notice

Passwords must be stored plain-text, so use with precaution. Also remember that most of proxy protocols send usernames/passwords unencrypted.

## License

MIT
