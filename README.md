# JWT扩展

## Requirements

- PHP 7.0 or later
- Linux/MacOS/Windows
- GCC 4.8 or later
- PHP-X 0.02 or later

## Build libphpx.so
```shell
cmake .
make -j 4
sudo make install
```

## Build extension
```shell
git clone https://github.com/FindAvalon/jwt.git
cd jwt
make 
sudo make install
```

## Load your extension
Edit `php.ini`, add `extension=jwt.so`
