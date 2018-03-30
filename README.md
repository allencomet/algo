# algo
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

