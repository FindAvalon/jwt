PHP_INCLUDE = `php-config --includes`
PHP_LIBS = `php-config --libs`
PHP_LDFLAGS = `php-config --ldflags`
PHP_INCLUDE_DIR = `php-config --include-dir`
PHP_EXTENSION_DIR = `php-config --extension-dir`

jwt.so: main.cpp
	c++ -DHAVE_CONFIG_H -g -o jwt.so -O0 -fPIC -shared main.cpp -std=c++11 -lphpx ${PHP_INCLUDE} -I${PHP_INCLUDE_DIR} -undefined dynamic_lookup
install: jwt.so
	cp jwt.so ${PHP_EXTENSION_DIR}/
clean:
	rm *.so
