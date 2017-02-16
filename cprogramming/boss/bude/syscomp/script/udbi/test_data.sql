-- UDBI_TEST 数据生成
insert into udbi_test(id, name, type, dtime)
values(1100000001, '联创科技公司', 'B0A', SYSDATE)
/
insert into udbi_test(id, name, type, dtime)
values(1100000002, '中国移动集团公司', 'B0B', SYSDATE)
/
insert into udbi_test(id, name, type, dtime)
values(1100000003, '代码生成器', 'B0C', SYSDATE)
/
insert into udbi_test(id, name, type, dtime)
values(1100000003, '测试测试名字', 'B0D', SYSDATE)
/

-- UDBI_TEST_DTY 测试数据生成
insert into udbi_test_dty(id, kind_id, vip_nbr, soc_nbr, sex, password, name, remark, state, state_date)
values(1100000001, 100, '110000000000', '11000000000', 'M', 'abcdef', '新疆移动公司', '这是备注', 'A0A', SYSDATE)
/
insert into udbi_test_dty(id, kind_id, vip_nbr, soc_nbr, sex, password, name, remark, state, state_date)
values(1100000002, 100, '120000000000', '12000000000', 'M', 'abcdef', '湖南移动公司', '这是备注', 'A0A', SYSDATE)
/
insert into udbi_test_dty(id, kind_id, vip_nbr, soc_nbr, sex, password, name, remark, state, state_date)
values(1100000003, 101, '130000000000', '13000000000', 'M', 'abcdef', '海南移动公司', '这是备注', 'A0A', SYSDATE)
/
insert into udbi_test_dty(id, kind_id, vip_nbr, soc_nbr, sex, password, name, remark, state, state_date)
values(1100000004, 102, '140000000000', '14000000000', 'M', 'abcdef', '天津移动公司', '这是备注', 'A0A', SYSDATE)
/
insert into udbi_test_dty(id, kind_id, vip_nbr, soc_nbr, sex, password, name, remark, state, state_date)
values(1100000005, 103, '150000000000', '15000000000', 'M', 'abcdef', '云南移动公司', '这是备注', 'A0A', SYSDATE)
/

-- UDBI_TEST_ALIAS 测试数据生成
insert into udbi_test_alias(kind_id, kind_name)
values(100, '类型一')
/
insert into udbi_test_alias(kind_id, kind_name)
values(101, '类型二')
/
insert into udbi_test_alias(kind_id, kind_name)
values(102, '类型三')
/
insert into udbi_test_alias(kind_id, kind_name)
values(103, '类型四')
/

-- UDBI_TEST_CUST 测试数据生成
insert into udbi_test_cust(cust_id, cust_seq_nbr, social_type_id, social_id, cust_type_id, cust_name, vip_flag, vip_nbr, sex, state, state_date)
values(1100000001, 1, 20, '998877665544332211', 30, '客户名称', 40, '1234567890', 'M', 'C0A', SYSDATE)
/
insert into udbi_test_cust(cust_id, cust_seq_nbr, social_type_id, social_id, cust_type_id, cust_name, vip_flag, vip_nbr, sex, state, state_date)
values(1100000002, 1, 20, '998877665544332211', 30, '客户名称111', 40, '1234567890', 'M', 'C0A', SYSDATE)
/
insert into udbi_test_cust(cust_id, cust_seq_nbr, social_type_id, social_id, cust_type_id, cust_name, vip_flag, vip_nbr, sex, state, state_date)
values(1100000003, 1, 20, '998877665544332211', 30, '客户名称222', 40, '1234567890', 'M', 'C0A', SYSDATE)
/
insert into udbi_test_cust(cust_id, cust_seq_nbr, social_type_id, social_id, cust_type_id, cust_name, vip_flag, vip_nbr, sex, state, state_date)
values(1100000004, 1, 20, '998877665544332211', 30, '客户名称333', 40, '1234567890', 'M', 'C0A', SYSDATE)
/
insert into udbi_test_cust(cust_id, cust_seq_nbr, social_type_id, social_id, cust_type_id, cust_name, vip_flag, vip_nbr, sex, state, state_date)
values(1100000005, 1, 20, '998877665544332211', 30, '客户名称444', 40, '1234567890', 'M', 'C0A', SYSDATE)
/


-- CODE_CODE 测试数据生成
insert into code_code(tab_name, sql_ref, sql_stmt)
values('UDBI_TEST_CUST', 'SEL_PK_UDBI_TEST_CUST',
'select cust_id, cust_seq_nbr, social_type_id, social_id, cust_type_id, cust_name, vip_flag, vip_nbr, sex, state, state_date 
  from udbi_test_cust
 where cust_id=:vcust_id
   and cust_seq_nbr=:vcust_seq_nbr')
/


拼SQL语句：
stmt_name: SELECT、UPDATE、DELETE FROM、INSERT INTO 
tab_name: TABLE_NAME
fields: SELECT: field1, field2, field3, field4, field5, ...
		UPDATE: field1=:vfield1, field2=:vfield2, field3=:vfield3, ...
		DELETE: null
		INSERT: (field1, field2, field3, field4, ...)
		VALUES: (:vfield1, :vfield2, :vfield3, :vfield4, ...)
		WHERE:  field1=:vfield1 and field2=:vfield2 and field3=:vfield3 and ...


