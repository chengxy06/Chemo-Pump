#!/usr/bin/env ruby
#require "profile" #使用这个库可以分析性能
#require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }

#添加系统中的库
#require 'fileutils'
require 'pathname'

#将../lib添加到库路径
$LOAD_PATH<<(File.expand_path('../', File.dirname(__FILE__))+"/lib")

#添加同目录的库
#下面的库可以分析一些共同的命令行参数以及添加一些通用的函数等
if File.file? File.dirname(__FILE__)+"/"+"main_common.rb"
	require_relative "main_common"  
else
	#如果同目录下没有就试试系统库$LOAD_PATH
	require 'main_common'
end

#设置帮助,版本等信息
SELF_NAME = File.basename(__FILE__)
SELF_ABSOLUTE_PATH = Pathname.new(__FILE__).realpath #File.expand_path(SELF_NAME)
SELF_DIR = File.dirname(SELF_ABSOLUTE_PATH)
HELP_STR = %Q{This is used to generate code from the type matrix
Usage: #{SELF_NAME} [option]... <file>...	
Note: <file> can use wildcard, #{CommonOptionParser::WILDCARD_USE_HELP}}
EXAMPLE_STR = %Q{Example:
#{SELF_NAME} file1 file2 
#{SELF_NAME} :file* 
#{SELF_NAME} 'file*' }		
VERSION_STR = "1.0.0"
ERROR_INFO_WHEN_NO_IN_ARG = "no input file"
IMG2LCD_BIN_PATH = "ruby #{SELF_DIR}/img2lcd.rb"


#初始化所有option
$options = OpenStruct.new
$options.out_file_name = nil   #输出文件名字, 如果不定义此变量,则命令选项-o,--outfile没有
#$options.error_out_file_name = nil   #错误输出文件名字,如果不定义此变量,则命令选项--error-outfile没有
$options.list_file_name = nil  #包含文件/目录名列表的文件名字,在列表中的文件/目录名不能包含通配符,也不能递归查找,
								#如果不定义此变量,则命令选项--listfile没有
$options.is_recursive = nil  #是否也处理在子目录中的输入文件,如果不定义此变量,则命令选项-r,--recursive没有
$options.is_allow_wildcard = true #是否允许参数为通配符
$options.action_when_no_normal_arg = "help" #当没有普通参数时的动作,"error":报错, "help":打印帮助,nil:正常
$options.is_use_stdin_as_inputfile = nil #是否从stdin读取输入内容,如果不定义此变量,则命令选项--from-stdin没有
$options.normal_args = []  #所要处理的输入参数(此参数数组中不包含options,参数可能是文件,目录名或其他)
#其他选项
$options.font_name = nil
$options.font_height = nil
$options.font_style = nil  #表示斜体,粗体等 

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
	# 
	opts.on("-f", "--font=<font_name>",
		"specify the font name") do |name|
		$options.font_name = name
	end

	opts.on("", "--height=<font_height>",
		"specify the font height") do |v|
		$options.font_height = v.to_i
	end

	opts.on("-s", "--style=<font_style>",
		"specify the font style") do |v|
		$options.font_style = v
	end	
end

#函数定义
class TypeMatrix
	attr:ch #int, 字的unicode码
	#attr:font_name
	#attr:font_style
	attr:height
	attr:width
	attr:bytes #字模数组
	attr:byte_count_of_one_line

	def init_from_bmp ch,height,bmp_path,is_scan_row=true,is_little_endian=true
		@ch = ch
		@height = height.to_i
		cmd = "#{IMG2LCD_BIN_PATH} "
		if !is_scan_row
			cmd += "--column_scan "
		end
		if !is_little_endian
			cmd += "--big_endian "
		end
		cmd += bmp_path
		#puts cmd
		ret = `#{cmd}`
		if $? !=0
			puts "Run #{cmd} fail"
			exit -1
		end
		ret.each_line do |line|
			line.chomp!
			if line =~ /size=(\d+)x(\d+)/
				@width = $1.to_i
				@height = $2.to_i
			elsif line.include?("code=")
				@bytes = line[5..-1].split(",")
				break
			end
		end
		@byte_count_of_one_line = @bytes.size/@height
	end

end

def get_gbk_char_code_str_from_unicode(ch_value)
	char_str = ch_value.chr(Encoding::UTF_16LE)
	#puts char_str.encode("gbk")
	char_gbk_code = char_str.encode("gbk").ord
	char_code_str = "0x%04x"%char_gbk_code
	return char_code_str
end

#将字节转换为类似于XXXXXXXX,_XX_____的字符串
def convert_byte_to_XXX(byte)
	binary_str = "%8s" % byte.to_s(2)
	ret = ""
	binary_str.each_char do |ch|
		if ch == " " || ch == "0"
			ret += "_"
		else
			ret += "X"
		end
	end

	return ret
end
#检查参数
#exit_p "file is not exist"

$type_matrixs = []
#处理所有普通输入参数
#num:当前处理的是第几个,  arg:当前处理的参数的名字(可能是文件,目录或其他)
#out_file:输出文件,已经打开 	其中如果输出文件名$options.out_file_name是nil那就是使用了终端$stdout
#error_file:错误输出文件,已经打开 其中如果错误输出文件名$options.error_file_name是nil那就是使用了终端$stderr
#另外out_file和error_file可以在each_file函数中传入,例CommonOptionParser.each_file($options, $stdout, $stderr) ...
CommonOptionParser.each_arg() do |num, arg, out_file, error_file|
	#puts Benchmark.measure{
	ch = File.basename(arg,".*").to_i(16)
	type_matrix = TypeMatrix.new
	#逐行模式,顺向
	type_matrix.init_from_bmp ch,$options.font_height,arg,true, false
	$type_matrixs << type_matrix

	#}
end

#输出代码
if $options.out_file_name
	out_file = File.open($options.out_file_name, "w:UTF-8")
else
	out_file = $stdout
end

out_file.puts %Q{//this file is auto generated, donot edit it
#include "font_data.h"
#include "constant_byte_define.h"


}
var_name_prefix = "g_font_#{$options.font_name}_#{$options.font_style}_#{$options.font_height}"

$type_matrixs.each do |type_matrix|
	#char_code_str = get_gbk_char_code_str_from_unicode(type_matrix.ch)
	char_code_str = "0x%04x" % type_matrix.ch
	ch_str = type_matrix.ch.chr(Encoding::UTF_16LE).encode("UTF-8")
	out_file.puts "const unsigned char #{var_name_prefix}_#{char_code_str}[#{type_matrix.bytes.size()}] = {//#{ch_str} "

	type_matrix.bytes.each_with_index do |ch,index|
		if index != 0 && index%type_matrix.byte_count_of_one_line==0
			#需要换行
			out_file.puts
		end

		out_file.print convert_byte_to_XXX(ch.to_i(16))+","
	end
	out_file.puts "\n};"
	out_file.puts
end

out_file.puts "const CharInfo #{var_name_prefix}_char_info[#{$type_matrixs.size()}] = {"
$type_matrixs.each do |type_matrix|
	#char_code_str = get_gbk_char_code_str_from_unicode(type_matrix.ch)
	char_code_str = "0x%04x" % type_matrix.ch
	ch_str = type_matrix.ch.chr(Encoding::UTF_16LE).encode("UTF-8")
	out_file.puts "{#{char_code_str}, #{type_matrix.width()}, #{type_matrix.byte_count_of_one_line},  \
	 #{var_name_prefix}_#{char_code_str}},//#{ch_str} "
end
out_file.puts "};"
out_file.puts

out_file.puts %Q|const Font #{var_name_prefix} = {
  #{$options.font_height}, // height of font 
  #{$type_matrixs.size}, // count of char
  #{var_name_prefix}_char_info
};
|
out_file.puts