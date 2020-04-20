#!/usr/bin/env ruby
#require "profile" #使用这个库可以分析性能
#require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }

#添加系统中的库
#require 'fileutils'

if ARGV.size>0
	$file_name = ARGV[0]
else
	puts "this is used to create source file with template.\nfor example: create_new_file drv_key, it will create drv_key.h and drv_key.c file."
	exit 1
end

$src_names = $file_name.split('_')
def replace_file(infile, outfile)
	infile.each_line do |line|
		line = line.gsub("{name}", $file_name)
		line = line.gsub("{date}", Time.now.strftime("%Y-%m-%d"))
		line = line.gsub("{author}", ENV["USERNAME"])
		line = line.gsub("{upcase_name}", $file_name.upcase+"_H")
		line = line.gsub("{camelcase_name}") do |src_name|
			new_names = $src_names.map { |e| e.capitalize }
			new_names.join ""
		end
		line = line.gsub("{no_prefix_name}") do |src_name|
			new_names = $src_names[1..-1]
			new_names.join "_"
		end
		line = line.gsub("{no_suffix_prefix_upcase_name}") do |src_name|
			new_names = $src_names[1..-1]
			new_names.map! { |e| e.upcase }
			new_names.join "_"
		end
		line = line.gsub("{no_suffix_prefix_camelcase_name}") do |src_name|
			new_names = $src_names[1..-1]
			new_names.map! { |e| e.capitalize }
			new_names.join ""
		end		
		outfile.print line
	end
end
TEMPLATE_DIR = File.expand_path('../../', __FILE__)+"/share/create_new_scr"

$h_file = File.open("#{$file_name}.h","w")
$c_file = File.open("#{$file_name}.cpp","w")
infile = File.open "#{TEMPLATE_DIR}/template.h"
replace_file infile,$h_file
infile = File.open "#{TEMPLATE_DIR}/template.cpp"
replace_file infile,$c_file