# logging_without_asking
Логирование функций из libc

1) Склонировать проект
```
git clone https://github.com/my-govnocode/logging_without_asking.git logging_without_asking

cd logging_without_asking
```

2) Собрать проект
```
make
```

3) Запустить демонов для логирования в файлы логов
```
./logd.out 2 logs/io.log logs/mem.log
```

4) Запустить тестовый пример программы
```
LD_PRELOAD=./liblogger.so ./test.out
```

Уничтожить демонов

```
sudo kill 15 $(cat /var/run/io_logd.pid)
```

```
sudo kill 15 $(cat /var/run/mem_logd.pid)
```