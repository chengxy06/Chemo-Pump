#!/usr/bin/env ruby
#require "profile" #使用这个库可以分析性能
#require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }

#添加系统中的库
require 'fileutils'
#require 'inifile'
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
SELF_ABSOLUTE_PATH = Pathname.new(__FILE__).realpath
SELF_DIR = File.dirname(SELF_ABSOLUTE_PATH)
HELP_STR = %Q{This is used to generate type matrix to bmp
Usage: #{SELF_NAME} [option]... <file>...	
Note: <file> can use wildcard, #{CommonOptionParser::WILDCARD_USE_HELP}}
EXAMPLE_STR = %Q{Example:
#{SELF_NAME} string.txt -o out_dir 
}		
VERSION_STR = "1.0.0"
ERROR_INFO_WHEN_NO_IN_ARG = "no input file"
TYPE_MATRIX_BIN = SELF_DIR+"/type_matrix.exe"

#初始化所有option
$options = OpenStruct.new
#$options.out_file_name = nil   #输出文件名字, 如果不定义此变量,则命令选项-o,--outfile没有
#$options.error_out_file_name = nil   #错误输出文件名字,如果不定义此变量,则命令选项--error-outfile没有
#$options.list_file_name = nil  #包含文件/目录名列表的文件名字,在列表中的文件/目录名不能包含通配符,也不能递归查找,
								#如果不定义此变量,则命令选项--listfile没有
#$options.is_recursive = nil  #是否也处理在子目录中的输入文件,如果不定义此变量,则命令选项-r,--recursive没有
#$options.is_allow_wildcard = true #是否允许参数为通配符
$options.action_when_no_normal_arg = "help" #当没有普通参数时的动作,"error":报错, "help":打印帮助,nil:正常
#$options.is_use_stdin_as_inputfile = nil #是否从stdin读取输入内容,如果不定义此变量,则命令选项--from-stdin没有
$options.normal_args = []  #所要处理的输入参数(此参数数组中不包含options,参数可能是文件,目录名或其他)
#其他选项
$options.out_dir = nil
$options.font_name = nil
$options.font_height = nil
$options.font_style = nil  #表示斜体,粗体等 
$options.average_width = 0

#配置选项并根据选项解析参数
#根据$options的值添加选项: -o, --outfile=<file>,--error-outfile=<file>, --listfile=<file>,
#	       -r,--recursive,-h,--help,-v,--version
CommonOptionParser.analyze_opt($options, HELP_STR, EXAMPLE_STR, VERSION_STR, ERROR_INFO_WHEN_NO_IN_ARG) do |opts|
	#添加自定义的选项
	# # 第一项是Short option，第二项是Long option，第三项是对Option的描述
	opts.on("-o", "--out_dir=<dir>",
	"output to this dir") do |file_name|
		$options.out_dir = file_name
	end

	opts.on("-f", "--font=<font_name>",
		"specify the font name") do |name|
		$options.font_name = name
	end

	opts.on("", "--height=<font_height>",
		"specify the font height") do |v|
		$options.font_height = v.to_i
	end

	opts.on("", "--average_width=<average_width>",
		"specify the average width") do |v|
		$options.average_width = v.to_i
	end

	opts.on("-s", "--style=<font_style>",
		"specify the font style") do |v|
		$options.font_style = v
	end
	# 
end

#函数定义
#返回map, 无Section
def read_ini(file_stream)
	ret = {}
	file_stream.each_line do |line|
		if line =~ /(\w+)=(.*)/
			ret[$1] = $2
		end
	end
	return ret
end


#创建每个字的位图
def create_font_bmp font,style, height,average_width, text,out_dir
	if !Dir.exist? out_dir
		Dir.mkdir out_dir
	end

	#生成type_matrix的输入文件
	#tmp_file_path = ENV["TMP"] + "\\type_matrix_#{Process.pid}"
	tmp_file_path = ENV["TMP"] + "/type_matrix_input"
	#tmp_file = File.new tmp_file_path,"w", "BOM|UTF-16LE" #,fmode:"BOM|UTF-16LE"
	tmp_file = File.new tmp_file_path,mode:"w" #,fmode:"BOM|UTF-16LE"
	#puts tmp_file.external_encoding
	tmp_file.putc 0xFF.chr
	tmp_file.putc 0xFE.chr
	tmp_file.set_encoding "UTF-16LE"
	#tmp_file.write 0xFF.chr
	#tmp_file.write 0xFE.chr
	tmp_file.puts "font:#{font}" #.encode("BOM|UTF-16LE")
	tmp_file.puts "style:#{style}" #.encode("BOM|UTF-16LE")
	tmp_file.puts "height:#{height}" #.encode("UTF-16LE")
	tmp_file.puts "average_width:#{average_width}" #.encode("UTF-16LE")
	tmp_file.puts "output:#{out_dir}"
	tmp_file.puts "text:#{text}"
	tmp_file.close

	#调用type_matrix生成图片
	puts "#{TYPE_MATRIX_BIN} #{tmp_file_path}"
	ret = system("#{TYPE_MATRIX_BIN} #{tmp_file_path}")
	#puts ret
	if ret != true
		puts "Run #{TYPE_MATRIX_BIN} #{tmp_file_path} fail"
		exit -1
	else
		# FileUtils.cp(tmp_file_path, "D:/chemo_pump/Src/#{height}")
		File.delete tmp_file_path if ENV["debug"]!="1"
	end

	# text.each_char do |ch|
	# 	tmp_path = "out_dir/%02x.bmp" % ch.ord
	# 	puts tmp_path 
	# end
end

#检查参数
#exit_p "file is not exist"
if $options.font_style==nil
	$options.font_style = "regular"
end
if $options.out_dir ==nil
	$options.out_dir = $options.font_name+"_"+$options.font_style+"_"+$options.font_height.to_s
end

#处理所有普通输入参数
#num:当前处理的是第几个,  arg:当前处理的参数的名字(可能是文件,目录或其他)
#out_file:输出文件,已经打开 	其中如果输出文件名$options.out_file_name是nil那就是使用了终端$stdout
#error_file:错误输出文件,已经打开 其中如果错误输出文件名$options.error_file_name是nil那就是使用了终端$stderr
#另外out_file和error_file可以在each_file函数中传入,例CommonOptionParser.each_file($options, $stdout, $stderr) ...
CommonOptionParser.each_arg() do |num, arg, out_file, error_file|
	#puts Benchmark.measure{
	in_file = File.open arg,"r:utf-8"

	str = in_file.read()
	#str.encode!("UTF-16LE")
	
	#将所有文字先转换为数组,然后排列,去除重复
	unicode_arr = []
	str.each_char do |v|
		unicode_arr << v.ord
		#puts unicode_arr.last
	end
	unicode_arr.sort!()
	unicode_arr.uniq!()

	#将数组转为字符串
	text = ""
	unicode_arr.each do |v|
		text += v.chr(Encoding::UTF_16LE)
		#puts text.encode("UTF-8")
	end
	text.encode!("UTF-8")
	
	#将所有文字转换为图片
	create_font_bmp($options.font_name, $options.font_style, $options.font_height,$options.average_width,
	 text, $options.out_dir)

	#}
end
