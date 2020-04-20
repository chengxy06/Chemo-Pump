#!/usr/bin/env ruby
#将../lib添加到库路径
$LOAD_PATH<<(File.expand_path('../../', __FILE__)+"/lib")

#添加库
#require "profile" #使用这个库可以分析性能
#require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }
#require 'fileutils'
#require 'pathname'  
require 'main_common' #分析一些共同的命令行参数以及添加一些通用的函数等
#require_relative "your.rb"

#设置帮助,版本等信息
SELF_NAME = File.basename(__FILE__)
SELF_ABSOLUTE_PATH = File.expand_path(".", __FILE__)
SELF_DIR = File.dirname(SELF_ABSOLUTE_PATH)
HELP_STR = %Q{This is used to create run_all_test_suite.c
Usage: #{SELF_NAME} [option]... <file>...	
Note: <file> can use wildcard, #{CommonOptionParser::WILDCARD_USE_HELP}}
EXAMPLE_STR = %Q{Example:
#{SELF_NAME} file1 file2 
#{SELF_NAME} :file* 
#{SELF_NAME} 'file*' }		
VERSION_STR = "1.0.0"
ERROR_INFO_WHEN_NO_IN_ARG = "no input file"

#初始化所有option
$options = OpenStruct.new
$options.out_file_name = nil   #输出文件名字, 如果不定义此变量,则命令选项-o,--outfile没有
$options.error_out_file_name = nil   #错误输出文件名字,如果不定义此变量,则命令选项--error-outfile没有
$options.list_file_name = nil  #包含文件/目录名列表的文件名字,在列表中的文件/目录名不能包含通配符,也不能递归查找,
								#如果不定义此变量,则命令选项--listfile没有
$options.is_recursive = nil  #是否也处理在子目录中的输入文件,如果不定义此变量,则命令选项-r,--recursive没有
$options.is_allow_wildcard = true #是否允许参数为通配符
$options.action_when_no_normal_arg = "help" #当没有普通参数时的动作,"error":报错, "help":打印帮助,nil:正常
$options.is_use_stdin_as_inputfile = nil #是否从stdin读取输入内容,如果不定义此变量,则命令选项--from-stdin没有
$options.normal_args = []  #所要处理的输入参数(此参数数组中不包含options,参数可能是文件,目录名或其他)
#其他选项

#配置选项并根据选项解析参数
#根据$options的值添加选项: -o, --outfile=<file>,--error-outfile=<file>, --listfile=<file>,
#	       -r,--recursive,-h,--help,-v,--version
CommonOptionParser.analyze_opt($options, HELP_STR, EXAMPLE_STR, VERSION_STR, ERROR_INFO_WHEN_NO_IN_ARG) do |opts|
	#添加自定义的选项
	# # 第一项是Short option，第二项是Long option，第三项是对Option的描述
	# opts.on("-d", "--dir=<dir>",
	# "the dit to store the test files") do |file_name|
	# 	$options.src_dir = file_name
	# end

	# opts.on("--warn-file=<file>",
	# "output warn to this file") do |file_name|
	# 	$options.out_file_name = file_name
	# 	CommonOptionParser.param_error "name is not right"
	# end 
end

#函数定义

#检查参数
#exit_p "file is not exist"
$suite_names = []
CommonOptionParser.out_file.puts %Q|#ifdef TEST
#include "greatest.h"

|
#处理所有普通输入参数
#num:当前处理的是第几个,  arg:当前处理的参数的名字(可能是文件,目录或其他)
#out_file:输出文件,已经打开 	其中如果输出文件名$options.out_file_name是nil那就是使用了终端$stdout
#error_file:错误输出文件,已经打开 其中如果错误输出文件名$options.error_file_name是nil那就是使用了终端$stderr
#另外out_file和error_file可以在each_file函数中传入,例CommonOptionParser.each_file($stdout, $stderr) ...
CommonOptionParser.each_arg() do |num, arg, out_file, error_file|
	#puts Benchmark.measure{
	in_file = nil
	in_file_name = arg
	if arg == $stdin
		in_file = arg
		in_file_name  = "<stdin>"
	elsif File.directory? arg
		error_file.puts "#{num}:#{arg} is directory"
		next
	else
		in_file = File.open arg,"r:utf-8"
	end
	if File.basename(arg) =~ /test_(\w+)/
		suite_name  = $1
	else
		suite_name = File.basename(arg)
	end
	$suite_names << suite_name

	setup_name = nil
	teardown_name = nil
	test_fun_names = []
	in_file.each_line do |line|
		if line =~ /static\s+void\s+(test_\w+_setup)/
			setup_name = $1
		elsif line =~ /static\s+void\s+(test_\w+_teardown)/
			teardown_name = $1
		elsif line =~ /static\s+void\s+test_(\w+)/
			test_fun_names << $1
		end
	end

	test_file_dir = File.dirname(File.expand_path(arg))
	suite_define_file = File.open "#{test_file_dir}/#{suite_name}_suite_def.h","w"
	suite_define_file.puts "#ifdef __cplusplus"
	suite_define_file.puts "extern \"C\" {"
	suite_define_file.puts "#endif"
	suite_define_file.puts "SUITE(#{suite_name}) {"
	if setup_name
		suite_define_file.puts "\tSET_SETUP(#{setup_name}, NULL);"
	end
	if teardown_name
		suite_define_file.puts "\tSET_TEARDOWN(#{teardown_name}, NULL);"
	end
	test_fun_names.each do |fun_name|
		suite_define_file.puts "\tRUN_TEST(#{fun_name});"
	end
	suite_define_file.puts "}"
	suite_define_file.puts "#ifdef __cplusplus"
	suite_define_file.puts "}"
	suite_define_file.puts "#endif"
	suite_define_file.puts 
	#}
end

CommonOptionParser.out_file.puts "void run_all_test_suite() {"
$suite_names.each do |suite_name|
	CommonOptionParser.out_file.puts "\tRUN_SUITE(#{suite_name});"
end
CommonOptionParser.out_file.puts "}\n#endif"