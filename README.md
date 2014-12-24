#Course Project of Compiler Theory
##TODO
project name


修改数据库部分的时候，发现了几个BUG，如下：

1.select * 出现错误
bug location: Parser
Parser fixed::??
select * from student where time > 11;
返回的statement中的prop_list 为空
---------这里空就是*了，不过还是给加上了一个“*”的property了

2.create table student(sid int, primary key (sid), age int, time int)
bug location: Parser
Parser fixed::??
返回的prop_list为[sid, sid, age, time]
然而
create table student(primary key (sid), age int, time int)（语法错误）
返回的prop_list为[sid, age, time]
----------已解决, 但是core没有处理好这个语法错误

3.大于符号“>”得到的解析符号是LT，比如出现
bug location: Lexer
Parser fixed::12-24, SunJiacheng

select sid, age, time  from student where time > 11;
得到的布尔树为： time LT 11

4.无法识别!=， 比如出现
bug location: Lexer|Parser
Lexer fixed: 12-24, SunJiacheng
Parser fixed::??

select sid, age, time  from student where time != 11;
直接崩掉
                                                                                                                ---栋梁：）
