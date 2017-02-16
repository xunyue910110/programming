PS3='请选择:'
menu="新增 修改 删除 查找 安装 初始化"
select choice in $menu
do
    echo "你选择的是: $REPLY, 选择的项目: $choice"
done

select f in *
do 
    echo  "你选择的是: $REPLY, 文件: $f"
    break
done

