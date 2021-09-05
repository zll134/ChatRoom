

### 一、语法

1.1 pwd : 获取当前目录

1.2 判断文件或文件夹是否存在

```shell
if [ ! -d "/data/" ];then
  mkdir /data
else
  echo "文件夹已经存在"
fi
文件是否存在用 -f
```

1.3 getops 解析输入的参数

1.4 if语句

```
if [ command ]; then
     符合该条件执行的语句
fi
```

