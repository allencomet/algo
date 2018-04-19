# algo
这是一些常见数据结构的练习结合
















…or create a new repository on the command line
echo "# Stormcv-web" >> README.md
git init
git add README.md
git commit -m "first commit"
git remote add origin https://github.com/allencomet/algo.git
git push -u origin master

如果已经按照上述过程操作过，最后会包没有权限上传，这个时候我们应该重新修改这个git仓库的url地址:
git remote set-url origin https://uesrename:pwd@github.com/allencomet/algo.git

然后再去push操作：git push -u origin master



I got this error when I tried to clone a repo in bit bucket.
"(gnome-ssh-askpass:11720): Gtk-WARNING **: cannot open display:"

Run below command and then try again:
Solved: unset SSH_ASKPASS

