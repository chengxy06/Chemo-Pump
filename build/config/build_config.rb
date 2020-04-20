#!/usr/bin/env ruby
#require "profile" #使用这个库可以分析性能
#require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }

#添加系统中的库
require 'optparse' #这个库可以分析命令行参数
require 'ostruct'  #这个库可以创建结构体
#require 'fileutils'
require 'yaml'
require "pp"
#require "Marsha"

#添加同目录的库
#require_relative "main_common"  


#设置帮助,版本等信息
SELF_NAME = File.basename(__FILE__)
HELP_STR = %Q{This is used to output build config by YAML format
This config contain the common and necessary config(like macros, incs, libs) 
Usage: #{SELF_NAME} [option]
}
EXAMPLE_STR = %Q{Example:
#{SELF_NAME} --tool=VS 
#{SELF_NAME} --tool=rake 
}		
VERSION_STR = "1.0.0"

#初始化所有option
$options = OpenStruct.new
$options.out_file_name = nil   #输出文件名字
$options.normal_args = []  #所要处理的输入参数(此参数数组中不包含options,参数可能是文件,目录名或其他)
#其他选项
$options.build_tool = nil   #工具名
$options.compiler = nil   #编译器名
#$options.build_config = nil #生成配置
#$options.dest_platform = nil #目标平台
$options.is_not_out_src = nil #是否不搜索并输出源文件
$options.is_output_simple_src = nil #是否使用简单数组输出源文件
#解析参数
begin
	#解析option
	OptionParser.new do |opts|
		#设置程序的帮助信息
		opts.banner = HELP_STR
		#设置程序的版本信息
		opts.version = VERSION_STR
		#设置帮助里的输出信息
		#opts.separator ""

		# 第一项是Short option，第二项是Long option，第三项是对Option的描述
		opts.on("-o", "--out=<out_file>",
		"output to this file") do |file_name|
			$options.out_file_name = file_name
		end

		#帮助信息
        opts.on_tail("-h", "--help", "show help message") do
            puts opts
            puts EXAMPLE_STR
            exit
        end

        #版本信息
        opts.on_tail("-v", "--version", "show version") do
            puts opts.version
            exit
        end

		#第一项是Short option，第二项是Long option，第三项是对Option的描述
		opts.on("--tool=<tool>",
		"the tool to make") do |var|
			$options.build_tool = var.downcase
		end

		opts.on("--compiler=<compiler>",
		"the compiler") do |var|
			$options.compiler = var.downcase
		end

		opts.on("--no_src",
		"not find and out the srcs") do 
			$options.is_not_out_src = true
		end

		opts.on("--simple_src",
		"use src path array to output src,not src tree") do 
			$options.is_output_simple_src = true
		end
	end.parse!
rescue OptionParser::ParseError =>err #命令行参数输入出错
	#输出错误信息并退出
	$stderr.print "Input parameter error: #{err}"
	exit -1
end

#函数定义
def exit_p str
	puts str
	exit -1
end

def get_out_file 
	if $options.out_file_name
		return File.open $options.out_file_name,"w"
	else
		return $stdout
	end
end

#是否匹配数组中的某一项,如果数组为空则不匹配
def is_match_arr(need_match_str, match_pattern_arr)
	if match_pattern_arr
		match_pattern_arr.each do |pattern|
			if File.fnmatch?(pattern, need_match_str,File::FNM_CASEFOLD)
				return true
			end
		end
	end

	return false
end

def get_src_tree src_dir, included_files=nil, excluded_files=nil,excluded_dirs=nil
	src_tree = {}
	src_tree["name"] = File.basename(src_dir)
	src_tree["files"] = []
	src_tree["childs"] = []
	Dir["#{src_dir}/*"].each do |file_path|
		if File.directory?(file_path) 
			if !is_match_arr(File.basename(file_path), excluded_dirs)
				src_tree["childs"] << get_src_tree(file_path, included_files, excluded_files,excluded_dirs)
			end
		else

			if included_files==nil || included_files.empty?() ||
					is_match_arr(File.basename(file_path), included_files)

				if !is_match_arr(File.basename(file_path), excluded_files)
					src_tree["files"]  << file_path
				end
			end
		end
	end

	return src_tree
end

def get_srcs src_dir, included_files=nil, excluded_files=nil,excluded_dirs=nil
	srcs = []
	Dir["#{src_dir}/*"].each do |file_path|
		if File.directory?(file_path) 
			if !is_match_arr(File.basename(file_path), excluded_dirs)
				srcs += get_srcs(file_path, included_files, excluded_files, excluded_dirs)
			end
		else
			if included_files==nil || included_files.empty?() ||
					is_match_arr(File.basename(file_path), included_files)
				if !is_match_arr(File.basename(file_path), excluded_files)
					srcs  << file_path
				end
			end
		end		
	end

	return srcs
end

load File.dirname(__FILE__)+"/build_config_define.rb"
#检查参数
if $options.build_tool == nil
	exit_p "not specify tool!"
elsif TOOL_CONFIG_MAP[$options.build_tool]==nil
	exit_p "not support tool:#{$options.build_tool}! "
end

#处理所有普通输入参数		
$valid_configs = TOOL_CONFIG_MAP[$options.build_tool]
$all_config = []

#打开输出文件
$out_file = get_out_file()

#搜索源文件并添加到第一个配置
if !$options.is_not_out_src
	#进入此目录
	curr_dir = Dir.pwd
	Dir.chdir(File.dirname(__FILE__))

	#获取srcs并放到第一个config
	build_config = Marshal.load(Marshal.dump(BUILD_CONFIG))
	SRC_TREE_DIR.each do |v|
		if $options.is_output_simple_src
			build_config["src_files"] += get_srcs(v['dir'], v['included_files'], 
				v['excluded_files'],v['excluded_dirs'])
		else
			build_config["src_tree"] << get_src_tree(v['dir'], v['included_files'], 
				v['excluded_files'],v['excluded_dirs'])
		end
	end
	#第一个先添加带有源文件的配置
	$all_config<<build_config

	#回到所在目录
	Dir.chdir curr_dir
end

#添加其他配置
$valid_configs.each do |config|
	build_config = Marshal.load(Marshal.dump(BUILD_CONFIG))
	build_config["config"] = config[:config]
	build_config["platform"] = config[:platform]
	custom_config($options.build_tool, build_config["config"], build_config['platform'], build_config)

	$all_config << build_config
end


#输出所有config
#pp $all_config
$out_file.puts $all_config.to_yaml()