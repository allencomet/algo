1、作用
（1）将前一个命令的标准输出传递给下一个命令，作为它的参数，xargs的默认命令是echo，空格是默认定界符
（2）将多行输入转换为单行

2、使用模式：front command | xargs -option later command
front command：  前一个命令
-option： xargs的选项
later command：  后一个命令

3、xargs常用选项
-n： 指定一次处理的参数个数
-d： 自定义参数界定符
-p： 询问是否运行 later command 参数
-t ： 表示先打印命令，然后再执行
-i ： 逐项处理

4、使用实例
测试文本：xargs.txt
[python] view plain copy
a b c d e f g
h i j k l m n
o p q
r s t
u v w x y z

（1）多行输入单行输出
cat xargs.txt | xargs
a b c d e f g h i j k l m n o p q r s t u v w x y z

（2）指定一次处理的参数个数：指定为5，多行输出
cat xargs.txt | xargs -n 5
a b c d e
f g h i j
k l m n o
p q r s t
u v w x y
z

（3）自定义参数界定符：指定界定符为'm'
cat xargs.txt | xargs -d m
a b c d e f g
h i j k l   n
o p q
r s t
u v w x y z

（4）询问是否运行 later command 参数
cat xargs.txt | xargs -n 5 -p
/bin/echo a b c d e ?...
/bin/echo f g h i j ?...
/bin/echo k l m n o ?...
/bin/echo p q r s t ?...
/bin/echo u v w x y ?...
/bin/echo z ?...

（5）将所有文件重命名，逐项处理每个参数
ls *.txt |xargs -t -i mv {} {}.bak

（6）用find找出文件以txt后缀，并使用xargs将这些文件删除
-bash-3.2# find /root/ -name "*.txt" -print        #查找
/root/2.txt
/root/1.txt
/root/3.txt
/root/4.txt

-bash-3.2# find /root/ -name "*.txt" -print0 | xargs -0 rm -rf   #查找并删除
-bash-3.2# find /root/ -name "*.txt" -print          #再次查找没有

（7）查找普通文件中包括thxy这个单词的
-bash-3.2# find /root/ -type f -print | xargs grep "thxy"
/root/1.doc:thxy

（8）查找权限为644的文件，并使用xargs给所有加上x权限
-bash-3.2# find /root/ -perm 644 -print
/root/1.c
/root/5.c
/root/2.doc
/root/3.doc
/root/1.doc
/root/2.c
/root/4.doc
/root/4.c
/root/3.c

-bash-3.2# find /root/ -perm 644 -print | xargs chmod a+x

-bash-3.2# find /root/ -perm 755 -print
/root/1.c
/root/5.c
/root/2.doc
/root/3.doc
/root/1.doc
/root/2.c
/root/4.doc
/root/4.c
/root/3.c

（9）ps -ef|grep LOCAL=NO|grep -v grep|cut -c 9-15|xargs kill -9
管道符"|"用来隔开两个命令，管道符左边命令的输出会作为管道符右边命令的输入。

"ps -ef" 是linux里查看所有进程的命令。这时检索出的进程将作为下一条命令"grep LOCAL=NO"的输入。
"grep LOCAL=NO" 的输出结果是，所有含有关键字"LOCAL=NO"的进程。
"grep -v grep" 是在列出的进程中去除含有关键字"grep"的进程。
"cut -c 9-15" 是截取输入行的第9个字符到第15个字符，而这正好是进程号PID。
"xargs kill -9" 中的 xargs 命令是用来把前面命令的输出结果（PID）作为"kill -9"命令的参数，并执行该命令。"kill -9"会强行杀掉指定进程。

另一种方法，使用awk
ps x|grep gas|grep -v grep |awk '{print $1}'|xargs kill -9


5、xargs与管道|的区别
| 用来将前一个命令的标准输出传递到下一个命令的标准输入，xargs 将前一个命令的标准输出传递给下一个命令，作为它的参数

#使用管道将ls的结果显示出来，ls标准输出的结果作为cat的标准输入
ls | cat    ##就是现实文件列表

#使用xargs将ls的结果作为cat的参数，ls的结果为文件名，所以cat 文件名即查看文件内容
ls | xargs cat  ##查看每个文件的内容