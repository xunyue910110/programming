自动更新程序: patch-1.3.rar

使用方法：
一、安装
   1、拷贝patch/src 到bude环境中，编译产生，可执行程序：patch
   2、与可执行程序同目录下，建立patch.properties 把环境配置设置上去，如：
         MW_PATCHLEVEL=/gboss/tuxapp/etc/mw_patchlevel
        SO_PATCHINFO=/gboss/tuxapp/etc/mw_sopatchinfo
        LCU_PATCHINFO=/gboss/tuxapp/etc/mw_lcupatchinfo
        SQL_PATCHINFO=/gboss/tuxapp/etc/mw_sqlpatchinfo
        BOSS_LIB=/gboss/tuxapp/lculib/
        PATCH_ROOT=/gboss/tuxapp/patchroot
        E_MAIL=gboss@lianchuang.com
        NO=NO
   3、确认程序有权限读写上述目录，安装完毕   

二、发布新版本
 1、建立一个patchroot目录，按照发布次序，建立版本号为名称的目录
  patchroot\1.1
           \1.2
           \1.3
           \1.5
           \1.6
 2、把要发布的so放到版本目录下，如:libcsCommonMgr-050824.102323.so 拷贝到 \1.6\下                    
 3、把需要重新发布的LCU流程，先发布到所在的数据库中.
 4、把需要修改发布的DAO::SQL，更新到SQL所在的数据库表中.
 5、建立文件patch_lcu.txt 把刚才重新发布的LCU的交易编码列出来写入文件中，并拷贝到\1.6\下
 6、建立文件patch_sql.txt 把刚才重新发布的DAO::SQL名列出来写入文件中，并拷贝到\1.6\下
 7、进入可执行程序所在目录，确认patch.properties文件中的配置，把发布人的e_mail填写上去，以后可根据e_mail追查到发布人。
 8、执行命令： ./patch -p 1.6
 9、查看有没有报错，没有报错，就发布完成了。
 
发布原理：mw_sopatchinfo中会列出\1.6\下的所有so的名字，并把版本号转为:libcsCommonMgr-1.6.so 拷贝到 lculib目录下。
         mw_lcupatchinfo会添加\1.6\patch_lcu.txt 的内容
         mw_sqlpatchinfo会添加\1.6\patch_sql.txt 的内容
         mw_patchlevel会把版本号!patch 1.6 和您的大名一起记下，以供参考。
         boss系统后台，根据上面几个文件的信息更新系统，完毕。

三、回退到历史版本
  1、看上一个版本的发布时间，取那个时候的CVS上的LCU流程，重新发布一次.
  2、同样，取那个时候的DAO::SQL，重新发布一次.
  3、在准确完成上面两步骤的基础上，执行命令./patch -u
  4、没有报错的话，恭喜您回退成功。
  5、如果还想退到再前一个版本，请再次按步骤1~4去做。
回退原理：
     mw_patchlevel已经记录下了发布轨迹，可以搜索到当前版本和历史版本。如果当前是1.6想回到1.5.（要确定有过1.5这个版本）
     mw_sopatchinfo会注释掉，发布1.6时写入的\1.6\patch_lcu.txt里的信息。
     mw_lcupatchinfo，mw_sqlpatchinfo会重新列出，1.6和1.5时发布过的lcu名和sql名，触发系统去更新他们。
     因为有发布轨迹，所以可以不停地回到再早以前的版本。

四、附件里是源程序和测试例子
   程序写得很笨，并不美观。有任何出错或疑问请发mail与我联系。




居云飞  
2005-08-24
juyf@lianchuang.com

 