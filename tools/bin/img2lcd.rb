#!/usr/bin/env ruby
#require "profile" #使用这个库可以分析性能
require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }

#添加系统中的库
#require 'fileutils'

#将../lib添加到库路径
$LOAD_PATH<<(File.expand_path('../', File.dirname(__FILE__))+"/lib")

#添加同目录的库
#下面的库可以分析一些共同的命令行参数
if File.file? File.dirname(__FILE__)+"/"+"common_option_parser.rb"
	require_relative "common_option_parser"  
else
	#如果同目录下没有就试试系统库$LOAD_PATH
	require 'common_option_parser'
end

require "image/bmp"

#文件名字
self_name = File.basename(__FILE__, ".rb")


#帮助信息
help_str = %Q{This is used to convert image to LCD code.
Usage: #{self_name} [option] [file]	}

#初始化所有option
$options = OpenStruct.new
$options.out_file_name = nil   #输出文件名字
$options.list_file_name = nil  #包含文件列表的文件名字
$options.is_recursive = nil  #是否也处理在子目录中的输入文件
$options.in_file_names = nil  #所要处理的所有文件
$options.is_allow_input_from_terminal = nil #是否允许从终端输入,如果没有输入文件
#默认模式
#    从第一行开始向右
# 每取8个点作为一个字
# 节，如果最后不足8个
# 点就补满8位。
#    取模顺序是从低到
# 高 ，即第一个点作为
# 最低位。如*-------
# 取为00000001
$options.is_row_for_lcd = true #取模是否是逐行模式, 否则就是逐列模式
$options.is_reverse_for_lcd = true #取模走向是否是逆向,即低位在前
$options.is_for_font = false
begin
	#解析option,其中-h,--help,-v,--version是自带支持的,可以不用添加
	OptionParser.new do |opts|
		#设置程序的帮助信息
		opts.banner = help_str
		#设置程序的版本信息
		opts.version = "1.0.0"
		#设置帮助里的输出信息
		#opts.separator ""

		#添加选项: -o out_file,--out=out_file,-l list_file,--list_file=list_file,-r,--recursive
		CommonOptionParser.analyze_opt opts,$options

		# # 第一项是Short option，第二项是Long option，第三项是对Option的描述
		opts.on( "--column_scan",
		"scan by the column") do 
			$options.is_row_for_lcd = false
		end

		opts.on( "--big_endian",
		"the draw is map to the byte bit") do 
			$options.is_reverse_for_lcd = false
		end

		opts.on( "--for_font",
		"the output is for font") do 
			$options.is_for_font = true
		end
		#添加信息,用于在帮助信息之后输出
		CommonOptionParser.add_end_text "Example:"
		CommonOptionParser.add_end_text "#{self_name} a.bmp b.bmp "	
	end.parse!
rescue OptionParser::ParseError =>err #命令行参数输入出错
	#输出错误信息并退出
	CommonOptionParser.param_error err  #也可以使用全局变量$!代替err
end

#将字符串所有内容重置为0
def reset_binary_str str
	0.upto str.size-1 do |index|
		str[index] = '0'
	end
	#puts str
end

#输出二进制字符串,返回输出字节总数
def output_binary_str out_file,str,count_of_output_bytes
	#一次的扫描位和字节位是否一致,如果不一致则反转
	if $options.is_reverse_for_lcd#是否是逆序
		str.reverse!
	end
	#如果不是第一次输出,则先输出分隔符
	if count_of_output_bytes!=0
		out_file.print ","
	end
	#将二进制字符串转换为16进制值输出
	v = str.to_i 2
	out_file.print "0x%02X" % v
	return count_of_output_bytes+1
end

#处理一个像素,输出对应位
def handle_pixel out_file,binary_str,pix,index, count_of_output_bytes
	#判断像素的值是否是白色,输出对应的位
	if pix.red == 255
		binary_str[index] = '0'
	else
		binary_str[index] = '1'
	end

	index += 1
	if index == binary_str.size
		#一个字节8个位已经填满,需要输出
		index = 0
		count_of_output_bytes = output_binary_str(out_file, binary_str,count_of_output_bytes)
	end
	return index,count_of_output_bytes
end

#补足剩余的空白位
def out_put_padding out_file,binary_str,index, count_of_output_bytes

	#如果不满一个字节,补足0后输出
	if index !=0
		index.upto(binary_str.size-1) do |i|
			binary_str[i] = '0'
			#puts i
		end
		count_of_output_bytes = output_binary_str(out_file, binary_str,count_of_output_bytes)
	end

	return 0,count_of_output_bytes
end

def generate_code_for_2gray_image img,out_file
	#一次输出8位
	binary_str = "00000000"
	count_of_output_bytes = 0 #总共输出的字节数

	
	if $options.is_row_for_lcd
		#扫描方式从左到右,一次扫描8位
		#从左向右,从上向下遍历所有像素
		0.upto(img.height-1) do |y|
			#重置索引值为0
			index = 0

			0.upto(img.width-1) do |x|
				#获取像素对象
				pix = img.get_pixel x,y
				#处理这个像素
				index,count_of_output_bytes = handle_pixel(out_file,binary_str, pix, index,count_of_output_bytes)
			end	

			#如果不满一个字节,补足0后输出
			index,count_of_output_bytes = out_put_padding(out_file,binary_str, index,count_of_output_bytes)
		end		
	else
		#扫描方式从上到下,一次扫描8位
		#从上向下,扫描8位后再从左向右遍历所有像素
		#然后再从上向下,扫描8位后再从左向右遍历所有像素
		
		height_start = 0
		while height_start<img.height
			height_end = height_start+7
			if height_end>img.height
				height_end = img.height
			end

			0.upto(img.width-1) do |x|
				#重置索引值为0
				index = 0

				#扫描8位
				height_start.upto(height_end) do |y|
					#获取像素对象
					pix = img.get_pixel x,y
					#处理这个像素
					index,count_of_output_bytes = handle_pixel(out_file,binary_str, pix, index,count_of_output_bytes)
				end		

				#如果不满一个字节,补足0后输出
				index,count_of_output_bytes = out_put_padding(out_file,binary_str, index,count_of_output_bytes)
			end
			height_start+=8
		end	
	end


end


#main
$options.in_file_names = ARGV
#num:当前处理的是第几个, 	in_file:当前处理的文件,已经打开	 in_file_name:当前处理的文件的名字 
#out_file:输出文件,已经打开 	out_file_name:输出文件名  其中如果文件名是nil那就是使用了终端
CommonOptionParser.each_file($options) do |num,in_file_name,out_file,error_file|
	#依次解析各个文件
	#if infile_name then	$outfile.puts infile_name+":" end
	#p infile_name
	#puts "handle file:#{in_file_name}"
	image = Bmp.new in_file_name
	#检查行必须是4的倍数
	# if image.width%4 != 0
	# 	warn "image's width[#{image.width}] is not multilpe of 4"
	# 	return
	# end
	# var_name = File.basename(in_file_name,File.extname(in_file_name))
	# var_name.gsub!(" ",'_')
	# var_name.gsub!("-",'_')
	# var_name.downcase!
	# var_name.gsub!(/\W/,'')
	# out_file.puts "const unsigned char icon_#{var_name}[] = {"

	out_file.puts "size=#{image.width}x#{image.height}"
	out_file.puts "bit_count=#{image.bit_count}"
	out_file.print "code="
	generate_code_for_2gray_image image,out_file
	# ch = 0
	# #p image.pixels
	# #exit
	# image.pixels.each_with_index do |color,index|
	# 	#p index
	# 	v1 = color.r/16
	# 	#p v1
	# 	if index%2 == 0
	# 		ch = v1
	# 	else
	# 		ch = ch*16+v1
	# 		out_file.print "0x%02X,"%ch
	# 	end

	# 	#如果满16个就换行
	# 	# if index%32 == 31
	# 	# 	out_file.puts
	# 	# end
	# end
	#out_file.puts "};"
	#out_file.puts 
end



