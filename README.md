```
soul generate --cpp echo.soul --output=echo.cpp
g++ -std=c++17 -L"/usr/local/lib/" main.cpp echo.cpp
./a.out
```
