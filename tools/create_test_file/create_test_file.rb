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

require 'cpp_analyze/cpp_analyze'

#设置帮助,版本等信息
SELF_NAME = File.basename(__FILE__)
SELF_ABSOLUTE_PATH = File.expand_path(".", __FILE__)
SELF_DIR = File.dirname(SELF_ABSOLUTE_PATH)
HELP_STR = %Q{This is used to create test file from source file
Usage: #{SELF_NAME} [option]... <file>...	
Note: <file> can use wildcard, #{CommonOptionParser::WILDCARD_USE_HELP}}
EXAMPLE_STR = %Q{Example:
#{SELF_NAME} timer.c
#{SELF_NAME} :file* 
#{SELF_NAME} 'file*' }		
VERSION_STR = "1.0.0"
ERROR_INFO_WHEN_NO_IN_ARG = "no input file"

#初始化所有option
$options = OpenStruct.new
#$options.out_file_name = nil   #输出文件名字, 如果不定义此变量,则命令选项-o,--outfile没有
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
	# opts.on("-w", "--warn-file=<file>",
	# "output warn to this file") do |file_name|
	# 	$options.out_file_name = file_name
	# 	CommonOptionParser.param_error "name is not right"
	# end

	# opts.on("--warn-file=<file>",
	# "output warn to this file") do |file_name|
	# 	$options.out_file_name = file_name
	# 	CommonOptionParser.param_error "name is not right"
	# end 
end

#函数定义
$g_all_fun_name = []
#function=FunctionDefine,处理函数定义
def handle_function(cpp_analyze, function, handle_statement, handle_comments,block_stack)
	$g_all_fun_name<<function.name.content
end

#自定义的处理CppAnalyze解析出来的信息函数
def custom_handle(cpp_analyze, handle_type, handle_obj, handle_statement, handle_comments,block_stack)
	#处理解析出来的东西
	case handle_type
	when 'block_head'
		#handle_obj等于block
		if handle_obj.type == 'function'
			#打印函数
			#p handle_obj
			#cpp_analyze.print_warn(handle_obj.obj.name.line_num, "function define: #{handle_obj.obj}")
			handle_function(cpp_analyze, handle_obj.obj, handle_statement, handle_comments,block_stack)
		end
	when 'function_define'
		#handle_obj等于FunctionDefine
		#函数声明
		#cpp_analyze.print_warn(handle_obj.name.line_num, "function define: #{handle_obj}")
		handle_function(cpp_analyze, handle_obj, handle_statement, handle_comments,block_stack)
	else
	end
end


#替换文件中的一些预置信息
def replace_file(infile, outfile, file_name,case_define,case_add)
	infile.each_line do |line|
		line = line.gsub("{file_name}", file_name)
		line = line.gsub("{case_define}", case_define)
		line = line.gsub("{case_add}", case_add)
		outfile.print line
	end
end

#检查参数
#exit_p "file is not exist"

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
		in_file = File.open arg
	end

	#依次解析各个文件
	#str = in_file.read
	cpp_lexer = CppLexer.new(in_file)
	cpp = CppAnalyze.new(in_file_name, cpp_lexer, out_file)
	cpp.check(&method(:custom_handle))
	
	#生成case define代码
	case_define = ""
	case_add = ""
	$g_all_fun_name.each do |fun_name|
		case_define += "static void test_#{fun_name}() {\n}\n\n"
		case_add += "    RUN_TEST(#{fun_name});\n"
	end


	#生成unit case文件
	file_name = File.basename(in_file_name,".*")
	unit_file_name = "test_#{file_name}.c"
	#p unit_file_name
	#system('pwd')
	unit_file = File.open unit_file_name,"w"
	template_file = File.open "#{File.dirname(__FILE__)}/test_template.c"
	replace_file template_file,unit_file,file_name,case_define,case_add
	#template_file.each_line do |variable|
		
	#end

	#}
end

#puts "Success!"