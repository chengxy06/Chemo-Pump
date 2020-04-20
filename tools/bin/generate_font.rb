#!/usr/bin/env ruby
#require "profile" #使用这个库可以分析性能
#require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }

#添加系统中的库
require 'optparse' #这个库可以分析命令行参数
require 'ostruct'  #这个库可以创建结构体
require 'fileutils'
require 'pathname'

require 'roo'
SELF_ABSOLUTE_PATH = Pathname.new(__FILE__).realpath
SELF_DIR = File.dirname(SELF_ABSOLUTE_PATH)
GENERATE_TYPE_MATRIX_BIN = "ruby "+SELF_DIR+"/generate_type_matrix.rb"
GENERATE_CODE_FROM_TYPE_MATRIX_BIN = "ruby "+SELF_DIR+"/generate_code_from_type_matrix.rb"

#文件名字
self_name = File.basename(__FILE__, ".rb")

#帮助信息
help_str = %Q{This is used to generate font c code.
Usage: #{self_name} [option] file1	}

#初始化所有option
$options = OpenStruct.new
$options.out_file_name = nil   #输出文件名字
$options.code_out_dir = nil   #代码输出目录
$options.bmp_out_dir = nil   #字库图输出目录
#$options.out_file_name_of_id = nil   #输出文件名字
$options.in_file_name = nil  #所要处理的文件名

begin
	#解析option,
	OptionParser.new do |opts|
		#设置程序的帮助信息
		opts.banner = help_str
		#设置程序的版本信息
		opts.version = "1.0.0"
		#设置帮助里的输出信息
		#opts.separator ""
		sample_str = %Q{#{self_name} font_str_define.ini  #it will generate generated_font_data.h and other font code}

		#帮助信息
        opts.on_tail("-h", "--help", "show help message") do
            puts opts
            puts("Example:")
            puts sample_str
            exit
        end

        opts.on("-o", "--out=out_file",
		"output to this file") do |file_name|
			$options.out_file_name = file_name
		end

        opts.on("-d", "--directory=out_dir",
		"output code to this dir") do |file_name|
			$options.code_out_dir = file_name
		end
        opts.on("-b", "--bmp_directory=out_dir",
		"output bmp to this dir") do |file_name|
			$options.bmp_out_dir = file_name
		end
		# opts.on("-i", "--id_out=out_file_for_id_define",
		# "output id definition to this file") do |file_name|
		# 	$options.out_file_name_of_id = file_name
		# end

	end.parse!
rescue OptionParser::ParseError =>err #命令行参数输入出错
	#输出错误信息并退出
	CommonOptionParser.param_error err  #也可以使用全局变量$!代替err
end


#function define
def exit_p str
	puts str
	exit -1
end

#将下划线命名法转换为驼峰命名法
def under_score_case_to_camel_case str
	words=str.split('_')
	new_words = words.map { |e|  e.capitalize}
	return new_words.join()
end

#return {section=>{key=>value,...},...
def read_ini_file ini_file
	section = {}
	last_section = ""
	section[last_section]={}
	File.open ini_file,"r:bom|utf-8" do |file|
		file.each_line do |line|
			if line[0] == ';' || line[0] == '\n'
				next
			end
			str = line.strip
			if str =~ /^\[(.*)\]/
				last_section = $1
				section[last_section] = {}
			#get key and value
			elsif str =~ /(\w+)=(.*)/
				key = $1
				value = $2
				section[last_section][key]=value
			else
				key = str
				value = nil
				section[last_section][key]=value
			end
		end
	end		

	return section
end

#get valid lang from lang.ini
def get_valid_lang file_name
	section = read_ini_file(file_name)
	#p section
	value = section[""]["lang"]

	lang_arr = []
	if value
		lang_arr = value.split ","
	end

	return lang_arr
end

#get id's value from excel sheet by lang and id
def get_str_by_id_and_lang sheet,lang,id
	str = ""

	lang_column = nil
	3.upto sheet.last_column do |column|
		if sheet.cell(1,column) == lang
			lang_column = column
			break()
		end
	end
	exit_p("no this lang") if !lang_column

	#p sheet.last_row
	2.upto(sheet.last_row) do |row|
		#p sheet.cell(row, 1),id
		if sheet.cell(row, 1) == id
			str=sheet.cell(row, lang_column)			
		end
	end
	return str
end

#get all id's value from excel sheet by lang
def get_str_by_lang sheet,lang
	str = ""

	lang_column = nil
	3.upto sheet.last_column do |column|
		if sheet.cell(1,column) == lang
			lang_column = column
			break()
		end
	end
	exit_p("no this lang") if !lang_column

	2.upto(sheet.last_row) do |row|
		if sheet.cell(row, lang_column)
			str+=sheet.cell(row, lang_column)
		end
	end
	return str
end

#get str map {id=>{lang,str},....}
def get_str_map lang_arr, lang_xlsx, font_str_define_ini
	if File.exist?(lang_xlsx)
		begin
			#read excel file
			excel = Roo::Excelx.new(lang_xlsx)  
		rescue Exception => e
			exit_p("#{lang_xlsx} not exist or format is wrong, detail error:[#{e}]")
		end
		sheet0 = excel.sheet(0)	
	else
		sheet0 = nil
	end

	str_map ={}
	sections_of_font_str = read_ini_file(font_str_define_ini)
	#p sections_of_font_str
	sections_of_font_str[""].each do |key,value|		
		str_map[key] = {}
		lang_arr.each do |lang|
			if key == 'str_id_all'
				str_map[key][lang] = get_str_by_lang(sheet0, lang)
			elsif key.index('id') != nil
				#p key
				str = ""
				#p value
				value.split(",").each do |id|
					#p id
					str+=get_str_by_id_and_lang(sheet0, lang, id)
				end
				str_map[key][lang] = str
			else
				str_map[key][lang] = value
			end 
		end
	end

	return str_map
end

#get str from the ids str
def get_str_by_idstr_and_lang_from_str_map idstr,lang, str_map
	str =""
	idstr.split(',').each do |id|
		if id.size >0
			str += 	str_map[id][lang]
		end
	end

	return str
end


#generate font data by tool
def generate_font_data font,code_out_dir, bmp_out_dir

	text_file_path = "text_for_font.txt"
	#puts font['text']
	File.open text_file_path,"w:utf-8" do |file|
		file.print font['text']
	end
	#text_file_path = '"'+text_file_path+'"'
	if code_out_dir
		save_file_path = code_out_dir+"/"
	else
		save_file_path = ""
	end
	save_file_path += "#{font['name_at_code']}_#{font['style_at_code']}_#{font['height']}.c"
	if bmp_out_dir
		type_matrix_dir = bmp_out_dir + "/"
	else
		type_matrix_dir = ""
	end
	type_matrix_dir += "#{font['name']}_#{font['style']}_#{font['height']}"

	cmd = "#{GENERATE_TYPE_MATRIX_BIN} -f \"#{font['name']}\" -s #{font['style']} --height=#{font['height']} " +
		 "--average_width=#{font['average_width']} " + text_file_path + " -o #{type_matrix_dir}"
	#cmd = "echo ok"
	puts cmd
	#exit
	if !system(cmd.encode("GBK"))
		exit_p("run [#{cmd}] fail")
	end
	File.delete text_file_path if ENV["debug"]!="1"

	cmd = "#{GENERATE_CODE_FROM_TYPE_MATRIX_BIN} -f \"#{font['name_at_code']}\" -s #{font['style']} --height=#{font['height']}" +
		" -o #{save_file_path}" + " #{type_matrix_dir}/*.bmp"
	puts cmd

	if !system(cmd)
		exit_p("run [#{cmd}] fail")
	end
end

#main

#check parameters
if(ARGV.size == 0)
	exit_p "no input file"
else
	$options.in_file_name = ARGV[0]
end

#input file
font_str_define_ini = $options.in_file_name
font_dir = File.dirname(font_str_define_ini)+"/font"
lang_ini = File.dirname(font_str_define_ini)+"/lang.ini"
lang_xlsx = File.dirname(font_str_define_ini)+"/lang.xlsx"

#output file
# code_out_dir = "font"
# if $options.code_out_dir
# 	code_out_dir = $options.code_out_dir
# end

#get valid lang
lang_arr = get_valid_lang(lang_ini)
#p lang_arr

#get str for lang file
str_map = get_str_map(lang_arr, lang_xlsx, font_str_define_ini)

if $options.code_out_dir && !File.exist?($options.code_out_dir)
	Dir.mkdir($options.code_out_dir)
end
if $options.bmp_out_dir && !File.exist?($options.bmp_out_dir)
	Dir.mkdir($options.bmp_out_dir)
end
fonts = {}
lang_arr.each do |lang_name|
	fonts[lang_name] = []
	font_set = read_ini_file(font_dir+"/#{lang_name}.ini")
	#p font_set
	font_set.each do |section_name, keys|
		if section_name.size != 0
			font = {}
			font['name_at_code'] = section_name.split(' ')[0]
			font['style_at_code'] = section_name.split(' ')[1].downcase
			font['height'] = section_name.split(' ')[2]
			font['name'] = keys['font_name']
			font['average_width'] = 0;
			if keys['average_width']
				font['average_width'] = keys['average_width']
			end
			font['style'] = section_name.split(' ')[1]
			#p keys['text_at_font']
			font['text'] = get_str_by_idstr_and_lang_from_str_map(keys['text_at_font'], lang_name, str_map) 
			#p font['text']
			generate_font_data(font, $options.code_out_dir, $options.bmp_out_dir)
			fonts[lang_name] <<font
		end
	end
end



if $options.out_file_name
	out_file_name = $options.out_file_name
else
	out_file_name = "generated_font_data.h"
end
#open outfile for write
outfile = File.open out_file_name,"w"
outfile.puts "//generated by #{self_name}, donot edit it"

#genarate var define
#extern GUI_CONST_STORAGE GUI_FONT GUI_Fontfontfangzhenlantingxihei_regular_32;
lang_arr.each do |lang|
	fonts[lang].each do |font|
		var_name = "g_font_#{font['name_at_code']}_#{font['style_at_code']}_#{font['height']}"
		outfile.puts "extern const Font #{var_name};"
	end	
end
outfile.puts ""

# TS_FONT_DATA g_font_data_english =
# {
#     {"songti", "regular", GUI_Fontsongti33},
#     {"yahei", "regular", GUI_Fontsongti33},
# }
lang_arr.each do |lang|
	outfile.puts "const static FontData g_font_data_#{lang}[] =\n{"
	fonts[lang].each do |font|
		var_name = "g_font_#{font['name_at_code']}_#{font['style_at_code']}_#{font['height']}"
		outfile.puts "\t{\"#{font['name_at_code']}\", \"#{font['style_at_code']}\", &#{var_name}},"
	end	
	outfile.puts "\t{NULL, NULL, NULL},"
	outfile.puts "};"
	outfile.puts ""
end

# const static TS_LANG_FONT_DATA g_lang_font_data[] = 
# {
# 	{E_LANG_ENGLISH, g_font_data_english},
# 	{E_LANG_SIMPLIFIED_CHINESE, g_font_data_simplified_chinese},
# };
outfile.puts "const static LangFontData g_lang_font_data[] = \n{"
lang_arr.each do |lang|
	outfile.puts "\t{kSszLang#{under_score_case_to_camel_case(lang)}, g_font_data_#{lang}},"
end
outfile.puts "};"


